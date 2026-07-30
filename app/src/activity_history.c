/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       activity_history.c
 * @brief      7-day step count history with W25Q80 persistence
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "activity_history.h"
#include "maps_dock_w25q80.h"
#include "oled.h"
#include "oled_i2c.h"
#include "pedometer.h"
#include <stdio.h>

/* -----------------------------------------------------------------------
 * Global state
 * ----------------------------------------------------------------------- */
ActivityHistoryState Activity = {
    {{0}},  /* history        */
    0,      /* history_len    */
    0,      /* today_steps    */
    0, 0, 0,/* last_saved date */
    0,      /* loaded         */
    0,      /* view_mode      */
    0       /* last_flash_rtc */
};

/* Live daily step goal. Initialised to the factory default; overwritten by
 * Read_Configure_Adjust_StepGoal_E2PROM_To_Value() at boot and by the
 * Configure-Adjust menu at runtime. */
uint32 Activity_Step_Goal = ACTIVITY_GOAL_STEPS;

/* -----------------------------------------------------------------------
 * CRC-8/SMBUS (polynomial 0x07, init 0x00, no reflection)
 * Used to detect Flash data corruption on read-back.
 * ----------------------------------------------------------------------- */
static uint8 crc8(const uint8 *data, int len)
{
    uint8 crc = 0x00;
    int i, b;
    for (i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (b = 0; b < 8; b++)
            crc = (crc & 0x80) ? ((uint8)(crc << 1) ^ 0x07) : (uint8)(crc << 1);
    }
    return crc;
}

/* -----------------------------------------------------------------------
 * Flash helpers
 * ----------------------------------------------------------------------- */

/* Serialize state to ACTIVITY_RECORD_SIZE-byte Flash record.
 * Layout: bytes 0..35 = data, byte 36 = CRC-8 of bytes 0..35. */
static void serialize(uint8 buf[ACTIVITY_RECORD_SIZE])
{
    int i;
    for (i = 0; i < ACTIVITY_DAYS; i++)
    {
        uint32 s = Activity.history[i].steps;
        buf[i * 4 + 0] = (uint8)(s >> 24);
        buf[i * 4 + 1] = (uint8)(s >> 16);
        buf[i * 4 + 2] = (uint8)(s >>  8);
        buf[i * 4 + 3] = (uint8)(s      );
    }
    /* Today's partial count */
    uint32 t = Activity.today_steps;
    buf[28] = (uint8)(t >> 24);
    buf[29] = (uint8)(t >> 16);
    buf[30] = (uint8)(t >>  8);
    buf[31] = (uint8)(t      );
    /* Metadata */
    buf[32] = (uint8)Activity.history_len;
    buf[33] = Activity.last_saved_year;
    buf[34] = Activity.last_saved_month;
    buf[35] = Activity.last_saved_day;
    /* CRC-8 over bytes 0..35 */
    buf[36] = crc8(buf, 36);
}

/* Deserialize ACTIVITY_RECORD_SIZE-byte Flash record into state.
 * Returns 1 on success, 0 if CRC mismatch (data corrupted). */
static int deserialize(const uint8 buf[ACTIVITY_RECORD_SIZE])
{
    /* Verify CRC before touching any state */
    uint8 expected = crc8(buf, 36);
    if (buf[36] != expected)
        return 0; /* corrupted */

    int i;
    for (i = 0; i < ACTIVITY_DAYS; i++)
    {
        Activity.history[i].steps =
            ((uint32)buf[i*4+0] << 24) |
            ((uint32)buf[i*4+1] << 16) |
            ((uint32)buf[i*4+2] <<  8) |
             (uint32)buf[i*4+3];
    }
    Activity.today_steps =
        ((uint32)buf[28] << 24) |
        ((uint32)buf[29] << 16) |
        ((uint32)buf[30] <<  8) |
         (uint32)buf[31];
    Activity.history_len      = buf[32];
    Activity.last_saved_year  = buf[33];
    Activity.last_saved_month = buf[34];
    Activity.last_saved_day   = buf[35];
    if (Activity.history_len > ACTIVITY_DAYS)
        Activity.history_len = ACTIVITY_DAYS;
    return 1;
}

/* Low-level Flash erase + program.
 * Every call costs one W25Q80 sector erase cycle (endurance ~10,000×). */
static void flash_write(void)
{
    uint8 buf[ACTIVITY_RECORD_SIZE];
    serialize(buf);
    /* Activity data lives in Sector 2 (address 0x2000), Page 32.
     * Sectors 0-1 are used by alarm/tense config — do NOT erase them. */
    MAPS_Dock_W25Q80_Erase_Block(0x00002000, ERASE_SECTOR_SIZE);
    MAPS_Dock_W25Q80_Write_Page(32, 0, buf, ACTIVITY_RECORD_SIZE);
}

static void flash_read(uint8 buf[ACTIVITY_RECORD_SIZE])
{
    MAPS_Dock_W25Q80_Read_Page(32, 0, buf, ACTIVITY_RECORD_SIZE);
}

/* -----------------------------------------------------------------------
 * Reset RAM state to "factory fresh"
 * ----------------------------------------------------------------------- */
static void reset_state(void)
{
    int i;
    for (i = 0; i < ACTIVITY_DAYS; i++)
        Activity.history[i].steps = 0;
    Activity.history_len      = 0;
    Activity.today_steps      = 0;
    Activity.last_saved_year  = 0;
    Activity.last_saved_month = 0;
    Activity.last_saved_day   = 0;
}

/* -----------------------------------------------------------------------
 * Activity_Load
 * ----------------------------------------------------------------------- */
void Activity_Load(void)
{
    uint8 buf[ACTIVITY_RECORD_SIZE];
    flash_read(buf);

    /* Detect virgin Flash: all data bytes 0xFF means sector was never written */
    int all_ff = 1;
    int i;
    for (i = 0; i < 36; i++)
    {
        if (buf[i] != 0xFF) { all_ff = 0; break; }
    }

    if (all_ff)
    {
        /* Fresh Flash — initialise everything to zero */
        reset_state();
    }
    else if (!deserialize(buf))
    {
        /* CRC mismatch: Flash record is corrupted (partial write, bit-flip, etc.)
         * Reset to zero rather than loading garbage data. */
        reset_state();
    }
    /* else: deserialize succeeded, state is populated */

    Activity.loaded = 1;
}

/* -----------------------------------------------------------------------
 * Activity_Tick
 * Called every 100ms. Syncs today_steps from Pedometer.
 * Detects date rollover and rotates the ring buffer.
 * ----------------------------------------------------------------------- */
void Activity_Tick(int year, int month, int day)
{
    /* Always sync today's steps from pedometer */
    Activity.today_steps = Pedometer.step_count;

    if (!Activity.loaded) return;

    /* Detect new day */
    uint8 y = (uint8)(year - 1970);
    uint8 m = (uint8)month;
    uint8 d = (uint8)day;

    /* First boot or fresh Flash: just record today's date silently without
     * shifting history or writing Flash (nothing to save yet). */
    if (Activity.last_saved_day == 0)
    {
        Activity.last_saved_year  = y;
        Activity.last_saved_month = m;
        Activity.last_saved_day   = d;
        return;
    }

    if (y == Activity.last_saved_year &&
        m == Activity.last_saved_month &&
        d == Activity.last_saved_day)
        return; /* Same day, nothing to do */

    /* New day detected: push today_steps into history ring, reset pedometer */
    int i;
    for (i = 0; i < ACTIVITY_DAYS - 1; i++)
        Activity.history[i] = Activity.history[i + 1];

    Activity.history[ACTIVITY_DAYS - 1].steps = Activity.today_steps;
    if (Activity.history_len < ACTIVITY_DAYS)
        Activity.history_len++;

    Activity.last_saved_year  = y;
    Activity.last_saved_month = m;
    Activity.last_saved_day   = d;

    Activity.today_steps = 0;
    Pedometer_Reset();

    /* Date-rollover writes are NOT rate-limited (at most once per day).
     * Update last_flash_rtc so the next manual save starts a fresh window. */
    flash_write();
    /* Note: we cannot read RTC_Count here (would create a module dependency).
     * The throttle for manual saves is enforced in Activity_Save_Today via
     * the rtc_now argument passed by the caller. Reset by setting to 0 so
     * the next manual KEY0 press is always allowed after a date rollover. */
    Activity.last_flash_rtc = 0;
}

/* -----------------------------------------------------------------------
 * Activity_Save_Today  — KEY0: manual checkpoint, rate-limited
 *
 * rtc_now : current RTC_Count (seconds since Unix epoch).
 * Returns : 1 if Flash was written, 0 if the call was throttled.
 *
 * Throttle: at most one manual save per ACTIVITY_SAVE_THROTTLE_S seconds
 * (default 300 s = 5 min).  This protects W25Q80 endurance; at 10 000
 * erase cycles and one save per 5 min the chip lasts > 95 years.
 * ----------------------------------------------------------------------- */
int Activity_Save_Today(uint32 rtc_now)
{
    /* Allow the save if:
     *   a) never saved before (last_flash_rtc == 0), OR
     *   b) sufficient time has elapsed since the last save.
     * Guard against RTC wrap-around: if rtc_now < last (clock reset),
     * treat as "enough time has passed" and allow the write. */
    uint32 elapsed = (rtc_now >= Activity.last_flash_rtc)
                     ? (rtc_now - Activity.last_flash_rtc)
                     : ACTIVITY_SAVE_THROTTLE_S; /* clock reset → allow */

    if (Activity.last_flash_rtc != 0 && elapsed < ACTIVITY_SAVE_THROTTLE_S)
        return 0; /* throttled — too soon */

    Activity.today_steps    = Pedometer.step_count;
    Activity.last_flash_rtc = rtc_now;
    flash_write();
    return 1;
}

/* -----------------------------------------------------------------------
 * Activity_Toggle_View
 * ----------------------------------------------------------------------- */
void Activity_Toggle_View(void)
{
    Activity.view_mode = Activity.view_mode ? 0 : 1;
}

/* -----------------------------------------------------------------------
 * Render helpers — thin aliases onto the shared framebuf module
 * ----------------------------------------------------------------------- */
#include "framebuf.h"
#define dp(s,x,y)        fb_pixel((s),(x),(y))
#define dline_h(s,y,x0,x1) fb_hline((s),(y),(x0),(x1))
#define dline_v(s,x,y0,y1) fb_vline((s),(x),(y0),(y1))
#define dc6(s,px,py,ch)  fb_char6((s),(px),(py),(ch))
#define ds6(s,px,py,str) fb_str6((s),(px),(py),(str))

/* Draw a filled vertical bar: x_center, bottom y, height in pixels */
static void draw_bar(uint8 screen[64][16], int x_c, int y_bot, int h, int w)
{
    int x0 = x_c - w / 2;
    int x1 = x_c + w / 2;
    if (h <= 0) { dp(screen, x_c, y_bot); return; }
    for (int x = x0; x <= x1; x++)
        dline_v(screen, x, y_bot - h + 1, y_bot);
}

/* -----------------------------------------------------------------------
 * Render_ActivityHistory
 *
 * Week view (view_mode == 0):
 *   Title:  "ACTIVITY (7 DAYS)"
 *   Bar chart: 7 bars (days), auto-scaled to max, today highlighted
 *   X labels: "M T W T F S S" (shifted by weekday)
 *   Bottom:  "TODAY: XXXXX  GOAL: 10000"
 *
 * Day detail (view_mode == 1):
 *   Shows today's progress + progress bar toward 10000 steps goal
 * ----------------------------------------------------------------------- */
void Render_ActivityHistory(void)
{
    /* Use the shared global framebuffer (saves 1 KB of stack per call). */
    #define screen g_fb
    fb_clear(g_fb);

    char buf[24];

    if (Activity.view_mode == 0)
    {
        /* ---- Week bar chart ---- */
        ds6(screen, 10, 0, "ACTIVITY 7 DAYS");
        dline_h(screen, 9, 0, 127);

        /* Bars occupy y=10..51 (42px tall), x=4..123 for 7 bars */
        /* The chart has (ACTIVITY_DAYS-1) past-day slots plus one "today"
         * slot.  When history_len exceeds the number of past slots, the
         * oldest stored day(s) are NOT displayed (right-aligned to the newest
         * entries).  Auto-scaling must ignore those hidden entries, otherwise
         * a tall-but-invisible day would shrink every visible bar. */
        int first_shown = (Activity.history_len > ACTIVITY_DAYS - 1)
                          ? Activity.history_len - (ACTIVITY_DAYS - 1)
                          : 0;
        /* Find max step count for auto-scaling (displayed days only) */
        uint32 max_steps = Activity.today_steps;
        for (int i = first_shown; i < Activity.history_len; i++)
            if (Activity.history[i].steps > max_steps)
                max_steps = Activity.history[i].steps;
        if (max_steps < 1) max_steps = 1;

        const int BAR_MAX_H  = 38;  /* max bar height in pixels */
        const int Y_BOT      = 50;  /* bottom of all bars */
        const int BAR_W      = 12;  /* bar width */
        const int SPACING    = 17;  /* center-to-center spacing */
        const int X_START    = 11;  /* center of first bar */

        /* Draw goal line (dashed) at height corresponding to the step goal */
        if (max_steps >= Activity_Step_Goal)
        {
            int goal_y = Y_BOT - (int)((uint64)Activity_Step_Goal * BAR_MAX_H / max_steps);
            for (int x = 2; x < 126; x += 4)
                dp(screen, x, goal_y);
        }

        /* Draw baseline */
        dline_h(screen, Y_BOT + 1, 2, 125);

        /* Draw 7 bars: [0..history_len-1] = past days, then today */
        /* Total slots = ACTIVITY_DAYS; slot 6 = today, 0=oldest */
        int total_slots = ACTIVITY_DAYS;
        for (int slot = 0; slot < total_slots; slot++)
        {
            uint32 steps;
            if (slot < ACTIVITY_DAYS - 1)
            {
                /* Past days — offset to fill from left */
                int hist_idx = slot - (ACTIVITY_DAYS - 1 - Activity.history_len);
                if (hist_idx < 0 || hist_idx >= Activity.history_len)
                    steps = 0;
                else
                    steps = Activity.history[hist_idx].steps;
            }
            else
            {
                /* Today (rightmost bar) */
                steps = Activity.today_steps;
            }

            int bar_h = (int)((uint64)steps * BAR_MAX_H / max_steps);
            int x_c = X_START + slot * SPACING;

            /* Highlight today with filled bar, others with outline */
            if (slot == total_slots - 1)
            {
                /* Today: solid filled + frame */
                draw_bar(screen, x_c, Y_BOT, bar_h, BAR_W);
                /* Outline */
                dline_v(screen, x_c - BAR_W/2 - 1, Y_BOT - bar_h, Y_BOT);
                dline_v(screen, x_c + BAR_W/2 + 1, Y_BOT - bar_h, Y_BOT);
            }
            else if (bar_h > 0)
            {
                /* Past days: only fill interior */
                for (int x = x_c - BAR_W/2; x <= x_c + BAR_W/2; x++)
                    dline_v(screen, x, Y_BOT - bar_h + 1, Y_BOT);
            }

            /* Day label below bar */
            static const char day_labels[] = "MTWTFSS";
            dc6(screen, x_c - 3, Y_BOT + 3, day_labels[slot % 7]);
        }

        /* Bottom: today's count + goal */
        snprintf(buf, sizeof(buf), "TODAY:%lu", (unsigned long)Activity.today_steps);
        ds6(screen, 0, 57, buf);

        snprintf(buf, sizeof(buf), "/%lu", (unsigned long)Activity_Step_Goal);
        ds6(screen, 72, 57, buf);
    }
    else
    {
        /* ---- Day detail view ---- */
        ds6(screen, 28, 0, "TODAY'S STEPS");
        dline_h(screen, 9, 0, 127);

        /* Large step count (centered) */
        snprintf(buf, sizeof(buf), "%lu", (unsigned long)Activity.today_steps);
        int len = 0;
        for (const char *p = buf; *p; p++) len++;

        /* Draw using 12x24 font */
        int sx = (128 - len * 12) / 2;
        for (int i = 0; buf[i]; i++)
        {
            int c = (uint8)buf[i] - 32;
            if (c < 0 || c >= 96) continue;
            int px = sx + i * 12;
            for (int col = 0; col < 12; col++)
            {
                uint8 fb;
                fb = Oled_FontLib_12x24[c*36 + col];
                for (int bit = 0; bit < 8; bit++)
                    if (fb & (1 << bit)) dp(screen, px+col, 12+bit);
                fb = Oled_FontLib_12x24[c*36 + 12 + col];
                for (int bit = 0; bit < 8; bit++)
                    if (fb & (1 << bit)) dp(screen, px+col, 20+bit);
                fb = Oled_FontLib_12x24[c*36 + 24 + col];
                for (int bit = 0; bit < 8; bit++)
                    if (fb & (1 << bit)) dp(screen, px+col, 28+bit);
            }
        }

        /* "steps" unit */
        ds6(screen, 43, 37, "steps");

        /* Progress bar toward goal */
        uint32 pct = (Activity.today_steps >= Activity_Step_Goal)
                     ? 100
                     : (uint32)(Activity.today_steps * 100 / Activity_Step_Goal);
        int bar_w = (int)((uint32)pct * 116 / 100);

        dline_h(screen, 46, 6, 122);
        dline_h(screen, 47, 6, 122);
        /* Fill progress */
        for (int x = 6; x < 6 + bar_w; x++)
        {
            dp(screen, x, 46);
            dp(screen, x, 47);
            dp(screen, x, 48);
            dp(screen, x, 49);
        }
        /* Border */
        dline_v(screen, 5,  45, 50);
        dline_v(screen, 123,45, 50);
        dline_h(screen, 45, 5, 123);
        dline_h(screen, 50, 5, 123);

        /* Percentage */
        snprintf(buf, sizeof(buf), "%lu%%  /%lu", (unsigned long)pct, (unsigned long)Activity_Step_Goal);
        ds6(screen, 20, 55, buf);
    }

    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);

    #undef screen  /* release the framebuffer alias; keep it function-local */
}
