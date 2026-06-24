/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       sleep_monitor.c
 * @brief      Sleep state detection via accelerometer RMS + OLED visualization
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "sleep_monitor.h"
#include "oled.h"
#include "oled_i2c.h"
#include <stdio.h>

/* -----------------------------------------------------------------------
 * Global state
 * ----------------------------------------------------------------------- */
SleepMonitorState SleepMon = {
    0, 0,           /* sum_sq, sample_count */
    SLEEP_STATE_AWAKE,
    0, 0, 0, 0, 0,  /* session stats */
    {{0}}, 0, 0,    /* history ring */
    1               /* monitoring ON by default */
};

/* -----------------------------------------------------------------------
 * Integer square root (Newton's method)
 * Input and working variables are uint64 to handle the large mag_sq values
 * computed from 16-bit accelerometer samples (max ~3.2e9 before sqrt).
 * ----------------------------------------------------------------------- */
static uint32 isqrt64(uint64 n)
{
    if (n == 0) return 0;
    uint64 x = n;
    uint64 y = (x + 1) / 2;
    while (y < x) { x = y; y = (x + n / x) / 2; }
    return (uint32)x;
}

/* -----------------------------------------------------------------------
 * SleepMonitor_Update
 * Called every 10ms.
 * ----------------------------------------------------------------------- */
void SleepMonitor_Update(int16 ax, int16 ay, int16 az, uint32 rtc_seconds)
{
    if (!SleepMon.monitoring) return;

    /* ---- Compute |a| in LSB ---- */
    int64 ax64 = (int64)ax, ay64 = (int64)ay, az64 = (int64)az;
    uint64 mag_sq = (uint64)(ax64*ax64 + ay64*ay64 + az64*az64);
    uint32 mag = isqrt64(mag_sq);       /* |a| in LSB              */

    /* Remove gravity component: motion = ||a| - 1g| */
    uint32 motion;
    if (mag >= 16384)
        motion = mag - 16384;
    else
        motion = 16384 - mag;

    /* Accumulate into RMS window */
    SleepMon.sum_sq += (uint64)motion * motion;
    SleepMon.sample_count++;

    /* ---- Evaluate window every SLEEP_WINDOW_SAMPLES (30 s × 100 Hz) ---- */
    /* We're called at 100 Hz (10 ms), so 3000 calls = 30 seconds */
    if (SleepMon.sample_count < SLEEP_WINDOW_SAMPLES)
        return;

    /* Compute RMS over the window */
    uint32 rms = isqrt64(SleepMon.sum_sq / SleepMon.sample_count);
    SleepMon.sum_sq      = 0;
    SleepMon.sample_count = 0;

    /* Classify state */
    SleepState new_state;
    if (rms > (uint32)SLEEP_AWAKE_THRESHOLD)
        new_state = SLEEP_STATE_AWAKE;
    else if (rms > (uint32)SLEEP_LIGHT_THRESHOLD)
        new_state = SLEEP_STATE_LIGHT;
    else
        new_state = SLEEP_STATE_DEEP;

    /* ---- Update statistics ---- */
    /* Count this 30-second slot toward sleep totals */
    if (new_state == SLEEP_STATE_LIGHT || new_state == SLEEP_STATE_DEEP)
    {
        SleepMon.total_sleep_s += SLEEP_WINDOW_SEC;
        if (new_state == SLEEP_STATE_DEEP)
            SleepMon.total_deep_s += SLEEP_WINDOW_SEC;
    }

    /* Detect turn (wake event during sleep) */
    if (SleepMon.was_sleeping && new_state == SLEEP_STATE_AWAKE)
        SleepMon.turn_count++;

    SleepMon.was_sleeping = (new_state != SLEEP_STATE_AWAKE) ? 1 : 0;
    SleepMon.current_state = new_state;

    /* ---- Push to history ring buffer ---- */
    SleepSlot slot;
    slot.state = new_state;
    SleepMon.history[SleepMon.hist_head] = slot;
    SleepMon.hist_head = (SleepMon.hist_head + 1) % SLEEP_HIST_SLOTS;
    if (SleepMon.hist_count < SLEEP_HIST_SLOTS)
        SleepMon.hist_count++;
}

/* -----------------------------------------------------------------------
 * SleepMonitor_Toggle / Clear
 * ----------------------------------------------------------------------- */
void SleepMonitor_Toggle(void)
{
    SleepMon.monitoring = !SleepMon.monitoring;
    if (SleepMon.monitoring)
    {
        /* Reset window accumulator */
        SleepMon.sum_sq       = 0;
        SleepMon.sample_count = 0;
    }
}

void SleepMonitor_Clear(void)
{
    SleepMon.total_sleep_s = 0;
    SleepMon.total_deep_s  = 0;
    SleepMon.turn_count    = 0;
    SleepMon.was_sleeping  = 0;
    SleepMon.hist_count    = 0;
    SleepMon.hist_head     = 0;
    SleepMon.sum_sq        = 0;
    SleepMon.sample_count  = 0;
    SleepMon.current_state = SLEEP_STATE_AWAKE;
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

/* -----------------------------------------------------------------------
 * Render_SleepMonitor
 *
 * Layout (128×64 OLED):
 *
 *   y= 0.. 8  Title: "SLEEP MONITOR" + status [ON]/[OFF]
 *   y= 9       separator
 *   y=10..11  State badge:  AWAKE / LIGHT / DEEP  (large, left)
 *   y=10..19  Stats (right):  hh:mm sleep  N turns
 *   y=20       separator
 *   y=21..52  Hypnogram bar chart (last SLEEP_HIST_SLOTS slots, 32px tall)
 *              Each slot is 1 pixel wide.  Height mapping:
 *                AWAKE → top row (1px)
 *                LIGHT → middle (2px block)
 *                DEEP  → tall block (4px)
 *   y=53       separator
 *   y=54..63  Hints + deep sleep %
 * ----------------------------------------------------------------------- */
void Render_SleepMonitor(void)
{
    uint8 screen[64][16];
    for (int i = 0; i < 64; i++)
        for (int c = 0; c < 16; c++)
            screen[i][c] = 0x00;

    char buf[24];

    /* ---- Title ---- */
    ds6(screen, 2, 0, "SLEEP MONITOR");
    ds6(screen, 86, 0, SleepMon.monitoring ? "[ON]" : "[OFF]");
    dline_h(screen, 9, 0, 127);

    /* ---- Current state badge ---- */
    const char *state_str;
    switch (SleepMon.current_state)
    {
    case SLEEP_STATE_AWAKE: state_str = "AWAKE"; break;
    case SLEEP_STATE_LIGHT: state_str = "LIGHT"; break;
    default:                state_str = "DEEP "; break;
    }
    ds6(screen, 2, 11, "NOW:");
    ds6(screen, 26, 11, state_str);

    /* ---- Stats (right side) ---- */
    uint32 sleep_min = SleepMon.total_sleep_s / 60;
    uint32 sleep_h   = sleep_min / 60;
    uint32 sleep_m   = sleep_min % 60;
    snprintf(buf, sizeof(buf), "%lu:%02lu", (unsigned long)sleep_h, (unsigned long)sleep_m);
    ds6(screen, 80, 11, buf);

    snprintf(buf, sizeof(buf), "TRN:%lu", (unsigned long)SleepMon.turn_count);
    ds6(screen, 80, 19, buf);

    dline_h(screen, 27, 0, 127);

    /* ---- Hypnogram bar chart ---- */
    /* Display last min(hist_count, 128) slots, 1px wide each */
    /* Chart area: x=0..127, y=28..54 (27px tall) */
    const int CHART_TOP  = 28;
    const int CHART_BOT  = 54;
    const int CHART_H    = CHART_BOT - CHART_TOP + 1; /* 27px */

    /* Baseline at CHART_BOT, bars grow upward */
    dline_h(screen, CHART_BOT + 1, 0, 127);

    int n = SleepMon.hist_count;
    if (n > 128) n = 128;

    for (int i = 0; i < n; i++)
    {
        /* Map slot index to x position (newest on right) */
        int x = 128 - n + i;
        if (x < 0 || x > 127) continue;

        /* Get slot from ring buffer */
        int ring_idx = (SleepMon.hist_head - n + i + SLEEP_HIST_SLOTS) % SLEEP_HIST_SLOTS;
        SleepState st = SleepMon.history[ring_idx].state;

        /* Bar height: AWAKE=4px, LIGHT=12px, DEEP=24px */
        int h;
        switch (st)
        {
        case SLEEP_STATE_AWAKE: h = 4;  break;
        case SLEEP_STATE_LIGHT: h = 12; break;
        default:                h = 24; break;
        }
        if (h > CHART_H) h = CHART_H;
        dline_v(screen, x, CHART_BOT - h + 1, CHART_BOT);
    }

    if (n == 0)
        ds6(screen, 22, 38, "-- no data yet --");

    /* ---- Bottom stats ---- */
    dline_h(screen, 56, 0, 127);

    /* Deep sleep percentage */
    uint32 deep_pct = (SleepMon.total_sleep_s > 0)
                      ? (SleepMon.total_deep_s * 100 / SleepMon.total_sleep_s)
                      : 0;
    snprintf(buf, sizeof(buf), "DEEP:%lu%%", (unsigned long)deep_pct);
    ds6(screen, 2, 57, buf);

    /* Hint */
    ds6(screen, 68, 57, SleepMon.monitoring ? "K0:OFF K2:CLR" : "K0:ON  K2:CLR");

    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
