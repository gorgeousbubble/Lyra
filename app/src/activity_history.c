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
    {{0}},  /* history */
    0,      /* history_len */
    0,      /* today_steps */
    0, 0, 0,/* last_saved date */
    0,      /* loaded */
    0       /* view_mode */
};

/* -----------------------------------------------------------------------
 * Flash helpers
 * ----------------------------------------------------------------------- */

/* Serialize state to 36-byte Flash record */
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
    /* Last saved date */
    buf[32] = Activity.history_len;
    buf[33] = Activity.last_saved_year;
    buf[34] = Activity.last_saved_month;
    buf[35] = Activity.last_saved_day;
}

/* Deserialize 36-byte Flash record into state */
static void deserialize(const uint8 buf[ACTIVITY_RECORD_SIZE])
{
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
    Activity.history_len   = buf[32];
    Activity.last_saved_year  = buf[33];
    Activity.last_saved_month = buf[34];
    Activity.last_saved_day   = buf[35];
    if (Activity.history_len > ACTIVITY_DAYS)
        Activity.history_len = ACTIVITY_DAYS;
}

static void flash_write(void)
{
    uint8 buf[ACTIVITY_RECORD_SIZE];
    serialize(buf);
    /* W25Q80 must erase sector before programming (sector = 4KB = 16 pages) */
    /* Page 2 is in Sector 0 (pages 0..15). Sector 0 already used by alarm/tense.
       We must NOT erase the whole sector.  W25Q80 page program can only set bits
       to 0; to set bits to 1 we need erase.
       Strategy: use a dedicated sector for activity data.
       Activity uses Page 2 only. To avoid erasing alarm (page 0) and tense (page 1),
       we store activity data on a SEPARATE sector:
         Sector 2 (pages 32..47) → Page 32  */
    /* Erase sector 2 (address 0x2000, size 4KB) */
    MAPS_Dock_W25Q80_Erase_Block(0x00002000, ERASE_SECTOR_SIZE);
    MAPS_Dock_W25Q80_Write_Page(32, 0, buf, ACTIVITY_RECORD_SIZE);
}

static void flash_read(uint8 buf[ACTIVITY_RECORD_SIZE])
{
    MAPS_Dock_W25Q80_Read_Page(32, 0, buf, ACTIVITY_RECORD_SIZE);
}

/* -----------------------------------------------------------------------
 * Activity_Load
 * ----------------------------------------------------------------------- */
void Activity_Load(void)
{
    uint8 buf[ACTIVITY_RECORD_SIZE];
    flash_read(buf);

    /* Check if Flash has been written before (first byte of first entry == 0xFF → erased) */
    if (buf[0] == 0xFF && buf[1] == 0xFF && buf[2] == 0xFF && buf[3] == 0xFF)
    {
        /* Fresh Flash: initialise to zeros */
        int i;
        for (i = 0; i < ACTIVITY_DAYS; i++)
            Activity.history[i].steps = 0;
        Activity.history_len   = 0;
        Activity.today_steps   = 0;
        Activity.last_saved_year  = 0;
        Activity.last_saved_month = 0;
        Activity.last_saved_day   = 0;
    }
    else
    {
        deserialize(buf);
    }
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

    if (y == Activity.last_saved_year &&
        m == Activity.last_saved_month &&
        d == Activity.last_saved_day)
        return; /* Same day, nothing to do */

    /* New day detected: push today_steps into history ring, reset pedometer */
    /* Shift history left (oldest falls off end) */
    int i;
    for (i = 0; i < ACTIVITY_DAYS - 1; i++)
        Activity.history[i] = Activity.history[i + 1];

    /* Write yesterday's count into [DAYS-1] */
    Activity.history[ACTIVITY_DAYS - 1].steps = Activity.today_steps;
    if (Activity.history_len < ACTIVITY_DAYS)
        Activity.history_len++;

    /* Update saved date */
    Activity.last_saved_year  = y;
    Activity.last_saved_month = m;
    Activity.last_saved_day   = d;

    /* Reset today's counter */
    Activity.today_steps = 0;
    Pedometer_Reset();

    /* Persist to Flash */
    flash_write();
}

/* -----------------------------------------------------------------------
 * Activity_Save_Today  — KEY0: manual checkpoint
 * ----------------------------------------------------------------------- */
void Activity_Save_Today(void)
{
    Activity.today_steps = Pedometer.step_count;
    flash_write();
}

/* -----------------------------------------------------------------------
 * Activity_Toggle_View
 * ----------------------------------------------------------------------- */
void Activity_Toggle_View(void)
{
    Activity.view_mode = Activity.view_mode ? 0 : 1;
}

/* -----------------------------------------------------------------------
 * Render helpers
 * ----------------------------------------------------------------------- */
static void dp(uint8 screen[64][16], int x, int y)
{
    if ((unsigned)x < 128 && (unsigned)y < 64)
        screen[y][x >> 3] |= (0x01 << (7 - (x & 7)));
}

static void dline_h(uint8 screen[64][16], int y, int x0, int x1)
{
    for (int x = x0; x <= x1; x++) dp(screen, x, y);
}

static void dline_v(uint8 screen[64][16], int x, int y0, int y1)
{
    if (y0 > y1) { int t = y0; y0 = y1; y1 = t; }
    for (int y = y0; y <= y1; y++) dp(screen, x, y);
}

static void dc6(uint8 screen[64][16], int px, int py, char ch)
{
    uint8 c = (uint8)ch - 32;
    if (c >= 96) return;
    for (int col = 0; col < 6; col++)
    {
        uint8 fb = Oled_FontLib_6x8[c][col];
        for (int bit = 0; bit < 8; bit++)
            if (fb & (1 << bit)) dp(screen, px + col, py + bit);
    }
}

static void ds6(uint8 screen[64][16], int px, int py, const char *s)
{
    while (*s) { dc6(screen, px, py, *s++); px += 6; }
}

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
    uint8 screen[64][16];
    for (int i = 0; i < 64; i++)
        for (int c = 0; c < 16; c++)
            screen[i][c] = 0x00;

    char buf[24];

    if (Activity.view_mode == 0)
    {
        /* ---- Week bar chart ---- */
        ds6(screen, 10, 0, "ACTIVITY 7 DAYS");
        dline_h(screen, 9, 0, 127);

        /* Bars occupy y=10..51 (42px tall), x=4..123 for 7 bars */
        /* Find max step count for auto-scaling */
        uint32 max_steps = Activity.today_steps;
        for (int i = 0; i < Activity.history_len; i++)
            if (Activity.history[i].steps > max_steps)
                max_steps = Activity.history[i].steps;
        if (max_steps < 1) max_steps = 1;

        const int BAR_MAX_H  = 38;  /* max bar height in pixels */
        const int Y_BOT      = 50;  /* bottom of all bars */
        const int BAR_W      = 12;  /* bar width */
        const int SPACING    = 17;  /* center-to-center spacing */
        const int X_START    = 11;  /* center of first bar */

        /* Draw goal line (dashed) at height corresponding to 10000 steps */
        if (max_steps >= (uint32)ACTIVITY_GOAL_STEPS)
        {
            int goal_y = Y_BOT - (int)((uint64)ACTIVITY_GOAL_STEPS * BAR_MAX_H / max_steps);
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

        snprintf(buf, sizeof(buf), "/%d", ACTIVITY_GOAL_STEPS);
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
        uint32 pct = (Activity.today_steps >= (uint32)ACTIVITY_GOAL_STEPS)
                     ? 100
                     : (uint32)(Activity.today_steps * 100 / ACTIVITY_GOAL_STEPS);
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
        snprintf(buf, sizeof(buf), "%lu%%  /%d", (unsigned long)pct, ACTIVITY_GOAL_STEPS);
        ds6(screen, 20, 55, buf);
    }

    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
