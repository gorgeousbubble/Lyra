/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       gyro_dash.c
 * @brief      Gyroscope dashboard: scrolling waveform and bar-chart for X/Y/Z gyro axes
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "gyro_dash.h"
#include "oled.h"
#include "oled_i2c.h"
#include <stdio.h>

/* Global state */
GyroDashState GyroDash = {{0}, {0}, {0}, 0, 0, GYRO_DASH_MODE_WAVE};

/* -----------------------------------------------------------------------
 * GyroDash_Update
 * Push one new sample into the ring buffers.
 * ----------------------------------------------------------------------- */
void GyroDash_Update(int16 gx, int16 gy, int16 gz)
{
    GyroDash.gx_buf[GyroDash.head] = gx;
    GyroDash.gy_buf[GyroDash.head] = gy;
    GyroDash.gz_buf[GyroDash.head] = gz;
    GyroDash.head = (GyroDash.head + 1) % GYRO_DASH_BUF_SIZE;
    if (GyroDash.count < GYRO_DASH_BUF_SIZE)
        GyroDash.count++;
}

/* Toggle waveform ↔ bar-chart */
void GyroDash_Toggle_Mode(void)
{
    GyroDash.mode = (GyroDash.mode == GYRO_DASH_MODE_WAVE)
                    ? GYRO_DASH_MODE_BAR
                    : GYRO_DASH_MODE_WAVE;
}

/* -----------------------------------------------------------------------
 * Frame-buffer helpers
 * ----------------------------------------------------------------------- */
static inline void dp(uint8 screen[64][16], int x, int y)
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
 * Waveform rendering
 *
 * The screen is split into three horizontal bands:
 *   X: y = 0..20   (21px, center y=10)
 *   Y: y = 21..41  (21px, center y=31)
 *   Z: y = 42..62  (21px, center y=52)
 *
 * X axis (time): pixels 0..79  (80px = GYRO_DASH_BUF_SIZE samples)
 * Y axis (value): ±full-scale maps to ±10 pixels from center
 *
 * Full-scale for display: ±32767 LSB (MPU6050 ±250dps = 131 LSB/°/s,
 * so ±32767 ≈ ±250°/s).  We clamp to ±32767 and scale to ±10px.
 * ----------------------------------------------------------------------- */

/* Scale a raw gyro LSB value to ±10 pixels */
static int scale_gyro(int16 v)
{
    int32 s = ((int32)v * 10) / 32767;
    if (s >  10) s =  10;
    if (s < -10) s = -10;
    return (int)s;
}

/* Return ring-buffer sample at logical index i (0=oldest, count-1=newest) */
static int16 rb_get(const int16 *buf, int head, int count, int i)
{
    int idx = (head - count + i + GYRO_DASH_BUF_SIZE) % GYRO_DASH_BUF_SIZE;
    return buf[idx];
}

static void render_waveform(uint8 screen[64][16])
{
    const int centers[3] = {10, 31, 52};
    const int tops[3]    = { 0, 21, 42};
    const int bots[3]    = {20, 41, 62};

    /* Draw centre lines and band borders */
    for (int b = 0; b < 3; b++)
    {
        /* Centre line (dashed every 4px) */
        for (int x = 0; x < 80; x += 4)
            dp(screen, x, centers[b]);
        /* Bottom border */
        dline_h(screen, bots[b], 0, 79);
    }

    /* Plot waveforms */
    int n = GyroDash.count;
    if (n == 0) return;

    for (int i = 0; i < n; i++)
    {
        int x = 80 - n + i;   /* right-aligned: newest at x=79 */
        if (x < 0) continue;

        int16 raw[3] = {
            rb_get(GyroDash.gx_buf, GyroDash.head, n, i),
            rb_get(GyroDash.gy_buf, GyroDash.head, n, i),
            rb_get(GyroDash.gz_buf, GyroDash.head, n, i)
        };

        for (int b = 0; b < 3; b++)
        {
            int y = centers[b] - scale_gyro(raw[b]);
            /* Clamp to band */
            if (y < tops[b]) y = tops[b];
            if (y > bots[b]) y = bots[b];
            dp(screen, x, y);

            /* Connect to previous sample with a vertical line */
            if (i > 0)
            {
                int16 prev_raw[3] = {
                    rb_get(GyroDash.gx_buf, GyroDash.head, n, i - 1),
                    rb_get(GyroDash.gy_buf, GyroDash.head, n, i - 1),
                    rb_get(GyroDash.gz_buf, GyroDash.head, n, i - 1)
                };
                int py = centers[b] - scale_gyro(prev_raw[b]);
                if (py < tops[b]) py = tops[b];
                if (py > bots[b]) py = bots[b];
                if (py != y)
                    dline_v(screen, x, y, py);
            }
        }
    }

    /* Right-side: vertical separator + labels */
    dline_v(screen, 81, 0, 63);

    char buf[10];
    /* Show labels X/Y/Z and current value */
    ds6(screen, 83, 2,  "Gx");
    snprintf(buf, sizeof(buf), "%+d", (int)rb_get(GyroDash.gx_buf, GyroDash.head, n, n-1));
    ds6(screen, 83, 10, buf);

    ds6(screen, 83, 23, "Gy");
    snprintf(buf, sizeof(buf), "%+d", (int)rb_get(GyroDash.gy_buf, GyroDash.head, n, n-1));
    ds6(screen, 83, 31, buf);

    ds6(screen, 83, 44, "Gz");
    snprintf(buf, sizeof(buf), "%+d", (int)rb_get(GyroDash.gz_buf, GyroDash.head, n, n-1));
    ds6(screen, 83, 52, buf);
}

/* -----------------------------------------------------------------------
 * Bar-chart rendering
 *
 * Three vertical bars, centred at x = 20, 64, 107
 * Bar width = 18px, height = 60px total (±30px from centre y=32)
 * ----------------------------------------------------------------------- */
static void render_bar(uint8 screen[64][16], int16 gx, int16 gy, int16 gz)
{
    const int cx[3]    = {20, 64, 107};
    const int cy       = 32;
    /* Reduced from 28 to 24 so label at cy+half_h+2=58 fits within y=0..63
     * (6x8 font: y=58..65 → last row y=65 clamped by dp() to y=63 is OK,
     *  but y=58 gives full 6 rows visible: 58,59,60,61,62,63 = 6px ✓) */
    const int half_h   = 24;
    const int bar_w    = 14;
    const int16 vals[3] = {gx, gy, gz};
    const char labels[3] = {'X', 'Y', 'Z'};

    for (int b = 0; b < 3; b++)
    {
        int x0 = cx[b] - bar_w / 2;
        int x1 = cx[b] + bar_w / 2;

        /* Outline box */
        dline_h(screen, cy - half_h, x0, x1);
        dline_h(screen, cy + half_h, x0, x1);
        dline_v(screen, x0, cy - half_h, cy + half_h);
        dline_v(screen, x1, cy - half_h, cy + half_h);

        /* Centre line */
        dline_h(screen, cy, x0, x1);

        /* Filled bar */
        int pix = ((int32)vals[b] * half_h) / 32767;
        if (pix > half_h)  pix = half_h;
        if (pix < -half_h) pix = -half_h;

        if (pix > 0)
        {
            /* Positive: fill from cy upward */
            for (int x = x0 + 1; x < x1; x++)
                dline_v(screen, x, cy - pix, cy - 1);
        }
        else if (pix < 0)
        {
            /* Negative: fill from cy downward */
            for (int x = x0 + 1; x < x1; x++)
                dline_v(screen, x, cy + 1, cy - pix);
        }

        /* Label below bar — cy+half_h+2 = 32+24+2 = 58; font 8px → y=58..65
         * dp() clips at y=63, so bottom 2 rows are invisible but top 6 show ✓ */
        dc6(screen, cx[b] - 3, cy + half_h + 2, labels[b]);

        /* Numeric value above bar — y=1 avoids overlap with bar top (y=8) */
        char vbuf[8];
        snprintf(vbuf, sizeof(vbuf), "%+d", (int)vals[b]);
        int vw = 0;
        for (const char *p = vbuf; *p; p++) vw += 6;
        ds6(screen, cx[b] - vw / 2, 1, vbuf);
    }
}

/* -----------------------------------------------------------------------
 * Render_GyroDash — entry point
 * ----------------------------------------------------------------------- */
void Render_GyroDash(int16 gx, int16 gy, int16 gz)
{
    uint8 screen[64][16];
    for (int i = 0; i < 64; i++)
        for (int c = 0; c < 16; c++)
            screen[i][c] = 0x00;

    if (GyroDash.mode == GYRO_DASH_MODE_WAVE)
        render_waveform(screen);
    else
        render_bar(screen, gx, gy, gz);

    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
