/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       adc_scope.c
 * @brief      Dual-channel ADC oscilloscope with auto-measurement (Vpp/Freq/Avg)
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "adc_scope.h"
#include "oled.h"
#include "oled_i2c.h"
#include <stdio.h>

/* -----------------------------------------------------------------------
 * Global state
 * ----------------------------------------------------------------------- */
AdcScopeState AdcScope = {
    {0}, {0},           /* ch0_buf, ch1_buf */
    0, 0,               /* head, count */
    0, 1, 0,            /* paused, decimation, sample_tick */
    {{0,0,0,0,0,0},     /* meas[0] */
     {0,0,0,0,0,0}}     /* meas[1] */
};

/* -----------------------------------------------------------------------
 * AdcScope_Update — push one sample pair into the ring buffer
 * ----------------------------------------------------------------------- */
void AdcScope_Update(uint16 ch0, uint16 ch1)
{
    if (AdcScope.paused) return;

    AdcScope.sample_tick++;
    if (AdcScope.sample_tick < (uint32)AdcScope.decimation)
        return;
    AdcScope.sample_tick = 0;

    AdcScope.ch0_buf[AdcScope.head] = ch0;
    AdcScope.ch1_buf[AdcScope.head] = ch1;
    AdcScope.head = (AdcScope.head + 1) % ADC_SCOPE_BUF;
    if (AdcScope.count < ADC_SCOPE_BUF)
        AdcScope.count++;
}

/* -----------------------------------------------------------------------
 * Control
 * ----------------------------------------------------------------------- */
void AdcScope_Toggle_Pause(void) { AdcScope.paused = !AdcScope.paused; }

void AdcScope_Dec_Inc(void)
{
    if (AdcScope.decimation < ADC_SCOPE_DEC_MAX)
        AdcScope.decimation <<= 1;
}

void AdcScope_Dec_Dec(void)
{
    if (AdcScope.decimation > ADC_SCOPE_DEC_MIN)
        AdcScope.decimation >>= 1;
}

/* -----------------------------------------------------------------------
 * AdcScope_Measure
 *
 * Computes per channel over the entire ring buffer:
 *
 *  Vpp  = (max - min) × Vref / 4095  (mV)
 *  Avg  = mean(all samples) × Vref / 4095  (mV)
 *  Freq = rising-edge zero-crossing method
 *         threshold = (vmax + vmin) / 2  (adaptive, better for signals
 *         that don't cross the ADC midpoint)
 *         freq_hz = rising_crossings × 1000 / window_ms
 *
 * Window time = n × decimation × 10ms
 * ----------------------------------------------------------------------- */
void AdcScope_Measure(void)
{
    int n = AdcScope.count;
    if (n < 4) return;

    const uint16 *bufs[2] = { AdcScope.ch0_buf, AdcScope.ch1_buf };

    for (int ch = 0; ch < 2; ch++)
    {
        const uint16 *buf = bufs[ch];
        int head = AdcScope.head;

        /* --- Pass 1: min / max / sum --- */
        uint32 sum  = 0;
        uint16 vmin = 4095;
        uint16 vmax = 0;

        for (int i = 0; i < n; i++)
        {
            int    idx = (head - n + i + ADC_SCOPE_BUF) % ADC_SCOPE_BUF;
            uint16 v   = buf[idx];
            if (v < vmin) vmin = v;
            if (v > vmax) vmax = v;
            sum += v;
        }

        /* Adaptive threshold = midpoint between min and max */
        uint16 threshold = (uint16)((vmin + vmax) / 2);

        /* --- Pass 2: rising-edge zero-crossing count --- */
        uint32 crossings = 0;
        int    prev_above = -1;   /* -1 = unknown */

        for (int i = 0; i < n; i++)
        {
            int    idx   = (head - n + i + ADC_SCOPE_BUF) % ADC_SCOPE_BUF;
            uint16 v     = buf[idx];
            int    above = (v >= threshold) ? 1 : 0;
            if (prev_above == 0 && above == 1)
                crossings++;
            prev_above = above;
        }

        /* --- Vpp in mV --- */
        uint32 vpp_raw = (uint32)(vmax - vmin);
        uint32 vpp_mv  = (vpp_raw * (uint32)ADC_VREF_MV) / ADC_FULL_SCALE;

        /* --- Avg in mV --- */
        uint32 avg_raw = sum / (uint32)n;
        uint32 avg_mv  = (avg_raw * (uint32)ADC_VREF_MV) / ADC_FULL_SCALE;

        /* --- Frequency --- */
        uint32 freq_hz   = 0;
        uint8  freq_valid = 0;
        uint32 window_ms = (uint32)n * (uint32)AdcScope.decimation * 10;

        if (vpp_raw < 50)
        {
            /* Signal too flat — no meaningful frequency */
            freq_hz    = 0;
            freq_valid = 0;
        }
        else if (crossings >= 2)
        {
            /* freq = crossings / window_s = crossings * 1000 / window_ms */
            freq_hz    = (crossings * 1000UL) / window_ms;
            freq_valid = 1;
        }
        else
        {
            /* Only 0 or 1 rising edge — signal period is longer than window */
            freq_hz    = 0;
            freq_valid = 1;   /* valid measurement: result is "<1Hz" */
        }

        /* Store */
        AdcScope.meas[ch].vpp_mv     = vpp_mv;
        AdcScope.meas[ch].avg_mv     = avg_mv;
        AdcScope.meas[ch].freq_hz    = freq_hz;
        AdcScope.meas[ch].v_min      = vmin;
        AdcScope.meas[ch].v_max      = vmax;
        AdcScope.meas[ch].freq_valid = freq_valid;
    }
}

/* -----------------------------------------------------------------------
 * Frame-buffer helpers
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
 * draw_channel — render one waveform in x=[ADC_WAVE_X0..ADC_WAVE_X1]
 * ----------------------------------------------------------------------- */
static void draw_channel(uint8 screen[64][16],
                         const uint16 *buf, int head, int count,
                         int cy, int half_h, int top, int bot)
{
    if (count < 2) return;
    int n       = (count > ADC_WAVE_W) ? ADC_WAVE_W : count;
    int x_start = ADC_WAVE_X0 + (ADC_WAVE_W - n);  /* right-align */
    int prev_y  = -1;

    for (int i = 0; i < n; i++)
    {
        int x = x_start + i;
        if (x < ADC_WAVE_X0 || x > ADC_WAVE_X1) continue;

        int ring_idx = (head - n + i + ADC_SCOPE_BUF) % ADC_SCOPE_BUF;
        uint16 v = buf[ring_idx];

        int pix = cy + half_h - (int)((uint32)v * (2 * half_h) / ADC_FULL_SCALE);
        if (pix < top) pix = top;
        if (pix > bot) pix = bot;

        dp(screen, x, pix);
        if (prev_y >= 0 && prev_y != pix)
            dline_v(screen, x, pix, prev_y);
        prev_y = pix;
    }
}

/* Format a mV value compactly: <1000 → "XXXmV", ≥1000 → "X.XXV" */
static void fmt_mv(char *dst, int dst_sz, uint32 mv)
{
    if (mv < 1000)
        snprintf(dst, dst_sz, "%lumV", (unsigned long)mv);
    else
        snprintf(dst, dst_sz, "%.2fV", (float)mv / 1000.0f);
}

/* Format frequency: 0 → "<1Hz", <1000 → "XXXHz", ≥1000 → "X.XkHz" */
static void fmt_freq(char *dst, int dst_sz, uint32 hz, uint8 valid)
{
    if (!valid)
        snprintf(dst, dst_sz, "---");
    else if (hz == 0)
        snprintf(dst, dst_sz, "<1Hz");
    else if (hz < 1000)
        snprintf(dst, dst_sz, "%luHz", (unsigned long)hz);
    else
        snprintf(dst, dst_sz, "%.1fkHz", (float)hz / 1000.0f);
}

/* -----------------------------------------------------------------------
 * Render_AdcScope
 *
 * Screen layout (128×64):
 *
 *  x= 0..11  : "C0" / "C1" label (6px × 2 chars)
 *  x=12..82  : waveform (71px, right-aligned)
 *  x=83      : vertical separator
 *  x=84..127 : measurement panel (44px = ~7 chars × 6px)
 *
 *  y= 0..25  : CH0 zone
 *    y= 0..8   Vpp row  "P 1.65V"
 *    y= 9..17  Avg row  "A 0.82V"
 *    y=18..25  Freq row "F 50Hz"
 *  y=26..27  : horizontal separator
 *  y=28..52  : CH1 zone (same layout, offset +28)
 *  y=53..63  : status bar
 *    "PSE" if paused | timebase | "K0:P K2/K3"
 * ----------------------------------------------------------------------- */
void Render_AdcScope(void)
{
    /* Always recompute measurements (pure math, fast) */
    AdcScope_Measure();

    uint8 screen[64][16];
    for (int i = 0; i < 64; i++)
        for (int c = 0; c < 16; c++)
            screen[i][c] = 0x00;

    char buf[14];

    /* ---- Vertical separator ---- */
    for (int y = 0; y < 53; y++) dp(screen, 83, y);

    /* ============================== CH0 ============================== */
    ds6(screen, 0, 0, "C0");

    /* Dashed centre line */
    for (int x = ADC_WAVE_X0; x <= ADC_WAVE_X1; x += 4)
        dp(screen, x, ADC_CH0_CY);

    draw_channel(screen, AdcScope.ch0_buf, AdcScope.head, AdcScope.count,
                 ADC_CH0_CY, ADC_WAVE_H, ADC_CH0_TOP, ADC_CH0_BOT);

    /* Measurement panel — CH0 */
    if (AdcScope.count >= 4)
    {
        /* Row 0: Vpp */
        ds6(screen, 85, 0, "P:");
        fmt_mv(buf, sizeof(buf), AdcScope.meas[0].vpp_mv);
        ds6(screen, 97, 0, buf);

        /* Row 1: Avg */
        ds6(screen, 85, 9, "A:");
        fmt_mv(buf, sizeof(buf), AdcScope.meas[0].avg_mv);
        ds6(screen, 97, 9, buf);

        /* Row 2: Freq */
        ds6(screen, 85, 18, "F:");
        fmt_freq(buf, sizeof(buf),
                 AdcScope.meas[0].freq_hz,
                 AdcScope.meas[0].freq_valid);
        ds6(screen, 97, 18, buf);
    }
    else
    {
        ds6(screen, 85, 8, "...");
    }

    /* ---- Horizontal separator ---- */
    dline_h(screen, 26, 0, 127);
    dline_h(screen, 27, 0, 127);

    /* ============================== CH1 ============================== */
    ds6(screen, 0, 28, "C1");

    for (int x = ADC_WAVE_X0; x <= ADC_WAVE_X1; x += 4)
        dp(screen, x, ADC_CH1_CY);

    draw_channel(screen, AdcScope.ch1_buf, AdcScope.head, AdcScope.count,
                 ADC_CH1_CY, ADC_WAVE_H, ADC_CH1_TOP, ADC_CH1_BOT);

    /* Measurement panel — CH1 */
    if (AdcScope.count >= 4)
    {
        ds6(screen, 85, 28, "P:");
        fmt_mv(buf, sizeof(buf), AdcScope.meas[1].vpp_mv);
        ds6(screen, 97, 28, buf);

        ds6(screen, 85, 37, "A:");
        fmt_mv(buf, sizeof(buf), AdcScope.meas[1].avg_mv);
        ds6(screen, 97, 37, buf);

        ds6(screen, 85, 46, "F:");
        fmt_freq(buf, sizeof(buf),
                 AdcScope.meas[1].freq_hz,
                 AdcScope.meas[1].freq_valid);
        ds6(screen, 97, 46, buf);
    }
    else
    {
        ds6(screen, 85, 36, "...");
    }

    /* ============================== Status bar ============================== */
    dline_h(screen, 53, 0, 127);

    /* Pause indicator */
    if (AdcScope.paused)
        ds6(screen, 0, 55, "PSE");

    /* Timebase */
    uint32 span_ms = (uint32)AdcScope.count * (uint32)AdcScope.decimation * 10;
    if (span_ms == 0)
        span_ms = (uint32)ADC_SCOPE_BUF * AdcScope.decimation * 10;

    if (span_ms < 1000)
        snprintf(buf, sizeof(buf), "x%u %lums", AdcScope.decimation, (unsigned long)span_ms);
    else
        snprintf(buf, sizeof(buf), "x%u %.1fs", AdcScope.decimation, (float)span_ms / 1000.0f);
    ds6(screen, 24, 55, buf);

    /* Key hints */
    ds6(screen, 90, 55, "K0");

    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
