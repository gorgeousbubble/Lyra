/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       health_monitor.c
 * @brief      Heart rate & SpO2 display with real-time IR pleth waveform (MAX30102)
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "health_monitor.h"
#include "health.h"
#include "oled.h"
#include "oled_i2c.h"
#include <stdio.h>

/* -----------------------------------------------------------------------
 * Globals
 * ----------------------------------------------------------------------- */
HealthWave HWave = {{0}, 0, 0, 0, 0, 0};

/* -----------------------------------------------------------------------
 * HealthMonitor_Update
 * Push one MAX30102 IR sample every 10ms.
 * Downsample by factor 10 so the waveform shows ~10 Hz.
 * ----------------------------------------------------------------------- */
void HealthMonitor_Update(uint32 ir_raw)
{
    HWave.downsample_cnt++;
    if (HWave.downsample_cnt < 10)
        return;
    HWave.downsample_cnt = 0;

    /* Write into ring buffer */
    HWave.ir_wave[HWave.head] = ir_raw;
    HWave.head = (HWave.head + 1) % HEALTH_WAVE_BUF;
    if (HWave.count < HEALTH_WAVE_BUF)
        HWave.count++;

    /* Recompute min/max for auto-scaling */
    uint32 mn = 0xFFFFFFFF, mx = 0;
    int n = HWave.count;
    for (int i = 0; i < n; i++)
    {
        int idx = (HWave.head - n + i + HEALTH_WAVE_BUF) % HEALTH_WAVE_BUF;
        uint32 v = HWave.ir_wave[idx];
        if (v < mn) mn = v;
        if (v > mx) mx = v;
    }
    HWave.min_val = mn;
    HWave.max_val = mx;
}

/* -----------------------------------------------------------------------
 * Frame-buffer helpers — thin aliases onto the shared framebuf module
 * ----------------------------------------------------------------------- */
#include "framebuf.h"
#define dp(s,x,y)        fb_pixel((s),(x),(y))
#define dline_h(s,y,x0,x1) fb_hline((s),(y),(x0),(x1))
#define dline_v(s,x,y0,y1) fb_vline((s),(x),(y0),(y1))
#define dc6(s,px,py,ch)  fb_char6((s),(px),(py),(ch))
#define ds6(s,px,py,str) fb_str6((s),(px),(py),(str))
}

/* Draw one row of a 12x24 character (row_idx = 0/1/2) */
static void draw_char12_row(uint8 screen[64][16], int px, int py,
                             int c_idx, int row_idx)
{
    for (int col = 0; col < 12; col++)
    {
        uint8 fb = Oled_FontLib_12x24[c_idx * 36 + row_idx * 12 + col];
        for (int bit = 0; bit < 8; bit++)
            if (fb & (1 << bit)) dp(screen, px + col, py + bit);
    }
}

static void draw_str12x24(uint8 screen[64][16], int px, int py, const char *s)
{
    for (; *s; s++, px += 12)
    {
        int c = (uint8)*s - 32;
        if (c < 0 || c >= 96) continue;
        draw_char12_row(screen, px, py,      c, 0);
        draw_char12_row(screen, px, py + 8,  c, 1);
        draw_char12_row(screen, px, py + 16, c, 2);
    }
}

/* Draw one row of an 8x16 character */
static void draw_char8_row(uint8 screen[64][16], int px, int py,
                            int c_idx, int row_idx)
{
    for (int col = 0; col < 8; col++)
    {
        uint8 fb = Oled_FontLib_8x16[c_idx * 16 + row_idx * 8 + col];
        for (int bit = 0; bit < 8; bit++)
            if (fb & (1 << bit)) dp(screen, px + col, py + bit);
    }
}

static void draw_str8x16(uint8 screen[64][16], int px, int py, const char *s)
{
    for (; *s; s++, px += 8)
    {
        int c = (uint8)*s - 32;
        if (c < 0 || c >= 96) continue;
        draw_char8_row(screen, px, py,     c, 0);
        draw_char8_row(screen, px, py + 8, c, 1);
    }
}

/* -----------------------------------------------------------------------
 * Waveform renderer
 * Draws HEALTH_WAVE_BUF samples in an (x:0..63, y:top..top+H-1) window.
 * Auto-scales to local min/max computed fresh from the buffer to avoid
 * the uint32 underflow risk from the global min_val state.
 * ----------------------------------------------------------------------- */
static void draw_wave(uint8 screen[64][16], int x_off, int y_top)
{
    int n = HWave.count;
    if (n < 2) return;

    /* Compute local min/max from the ring buffer instead of relying on
     * the global HWave.min_val which could be 0xFFFFFFFF if never updated */
    uint32 local_min = 0xFFFFFFFFUL;
    uint32 local_max = 0;
    for (int i = 0; i < n; i++)
    {
        int idx = (HWave.head - n + i + HEALTH_WAVE_BUF) % HEALTH_WAVE_BUF;
        uint32 v = HWave.ir_wave[idx];
        if (v < local_min) local_min = v;
        if (v > local_max) local_max = v;
    }

    /* Clamp span to avoid division by near-zero */
    uint32 span = (local_max > local_min) ? (local_max - local_min) : 0;
    if (span < 1000) span = 1000;

    int prev_y = -1;
    for (int i = 0; i < n; i++)
    {
        int x = x_off + (HEALTH_WAVE_BUF - n) + i;
        if (x < x_off || x >= x_off + HEALTH_WAVE_BUF) continue;

        int idx = (HWave.head - n + i + HEALTH_WAVE_BUF) % HEALTH_WAVE_BUF;
        uint32 v = HWave.ir_wave[idx];

        /* Safe subtraction: v is always >= local_min by construction */
        uint32 v_off = (v >= local_min) ? (v - local_min) : 0;

        /* Scale to pixel (invert: higher value = higher on screen) */
        int pix = (int)(((uint64)v_off * (HEALTH_WAVE_H - 1)) / span);
        int y = y_top + (HEALTH_WAVE_H - 1) - pix;
        if (y < y_top) y = y_top;
        if (y > y_top + HEALTH_WAVE_H - 1) y = y_top + HEALTH_WAVE_H - 1;

        dp(screen, x, y);
        if (prev_y >= 0 && prev_y != y)
            dline_v(screen, x, y, prev_y);
        prev_y = y;
    }
}

/* -----------------------------------------------------------------------
 * Render_HealthMonitor
 *
 * Layout (128x64 OLED):
 *
 *   y= 0..1  : "HR" label (6x8) at x=0 | "SpO2" at x=88
 *   y= 2..25 : Heart rate in 12x24 large font, right of "HR" label
 *   y= 2..17 : SpO2 in 8x16 font, right side
 *   y=26..27 : separator line
 *   y=28..47 : IR plethysmography waveform (0..127, 20px high)
 *   y=48..55 : status text
 *   y=56..63 : "Waiting..." or last valid timestamp hint
 *
 * Heart rate: displayed as "---" until first valid reading
 * SpO2:       displayed as "--%" until first valid reading
 * ----------------------------------------------------------------------- */
void Render_HealthMonitor(void)
{
    /* Use the shared global framebuffer (saves 1 KB of stack per call). */
    #define screen g_fb
    fb_clear(g_fb);

    char buf[16];

    /* ---- Left: Heart Rate ---- */
    ds6(screen, 0, 0, "HR");

    if (Heart_Rate > 0 && Heart_Rate < 300)
        snprintf(buf, sizeof(buf), "%ld", (long)Heart_Rate);
    else
        snprintf(buf, sizeof(buf), "---");

    /* Center HR value: compute width then shift */
    int hr_len = 0;
    for (const char *p = buf; *p; p++) hr_len++;
    int hr_x = (60 - hr_len * 12) / 2;
    if (hr_x < 12) hr_x = 12;
    draw_str12x24(screen, hr_x, 2, buf);

    /* "BPM" unit below HR */
    ds6(screen, hr_x + hr_len * 12 - 18, 26, "BPM");

    /* ---- Right: SpO2 ---- */
    /* Vertical separator */
    for (int y = 0; y < 44; y++) dp(screen, 65, y);

    ds6(screen, 68, 0, "SpO2");

    if (SPO2 > 0 && SPO2 <= 100)
        snprintf(buf, sizeof(buf), "%ld%%", (long)SPO2);
    else
        snprintf(buf, sizeof(buf), "--%%");

    draw_str8x16(screen, 70, 10, buf);

    /* SpO2 quality indicator bar (0..100 mapped to 0..58px) */
    if (SPO2 > 0 && SPO2 <= 100)
    {
        int bar_w = (int)((long)SPO2 * 58 / 100);
        dline_h(screen, 28, 67, 67 + bar_w);
        dline_h(screen, 29, 67, 67 + bar_w);
        dline_h(screen, 30, 67, 67 + bar_w);

        /* Color zone markers */
        ds6(screen, 67, 32, "95  100%");
    }

    /* ---- Bottom: waveform ---- */
    dline_h(screen, 34, 0, 64);

    /* Waveform: x=0..63, y=35..54 (20px) */
    if (HWave.count > 1)
        draw_wave(screen, 0, 35);
    else
        ds6(screen, 4, 40, "Collecting...");

    /* Waveform border */
    dline_h(screen, 55, 0, 64);

    /* ---- Status ---- */
    if (Heart_Rate > 0 && SPO2 > 0)
        ds6(screen, 0, 57, "Valid reading OK");
    else
        ds6(screen, 0, 57, "Wait ~5s...");

    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
