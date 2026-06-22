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
AdcScopeState AdcScope = {{0}, {0}, 0, 0, 0, 1, 0, {{0,0,0,0,0,0},{0,0,0,0,0,0}}};

/* -----------------------------------------------------------------------
 * AdcScope_Update
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
 * AdcScope_Toggle_Pause / Dec_Inc / Dec_Dec
 * ----------------------------------------------------------------------- */
void AdcScope_Toggle_Pause(void)
{
    AdcScope.paused = !AdcScope.paused;
}

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
 * Computes for each channel over the entire ring buffer:
 *   Vpp  = (max - min) converted to mV
 *   Avg  = arithmetic mean converted to mV
 *   Freq = zero-crossing count method
 *          Crossing threshold = ADC_ZC_THRESHOLD (midpoint)
 *          Count rising crossings in the buffer window.
 *          Period = window_time / crossings
 *          Freq = 1 / Period
 *
 * Window time = count × decimation × 10ms
 * ----------------------------------------------------------------------- */
void AdcScope_Measure(void)
{
    int n = AdcScope.count;
    if (n < 4) return;   /* Not enough samples yet */

    const uint16 *bufs[2] = { AdcScope.ch0_buf, AdcScope.ch1_buf };

    for (int ch = 0; ch < 2; ch++)
    {
        const uint16 *buf = bufs[ch];
        int head  = AdcScope.head;

        uint32 sum   = 0;
        uint16 vmin  = 4095;
        uint16 vmax  = 0;
        uint32 crossings = 0;
        int prev_above = -1;   /* -1 = unknown, 0 = below, 1 = above threshold */

        for (int i = 0; i < n; i++)
        {
            int idx = (head - n + i + ADC_SCOPE_BUF) % ADC_SCOPE_BUF;
            uint16 v = buf[idx];

            /* Min / Max / Sum */
            if (v < vmin) vmin = v;
            if (v > vmax) vmax = v;
            sum += v;

            /* Rising zero-crossing detection (crosses threshold upward) */
            int above = (v >= ADC_ZC_THRESHOLD) ? 1 : 0;
            if (prev_above == 0 && above == 1)
                crossings++;
            prev_above = above;
        }

        /* Vpp in mV */
        uint32 vpp_raw = (uint32)(vmax - vmin);
        uint32 vpp_mv  = (vpp_raw * (uint32)ADC_VREF_MV) / ADC_FULL_SCALE;

        /* Avg in mV */
        uint32 avg_raw = sum / (uint32)n;
        uint32 avg_mv  = (avg_raw * (uint32)ADC_VREF_MV) / ADC_FULL_SCALE;

        /* Frequency: window_ms = n × decimation × 10 */
        uint32 freq_hz = 0;
        uint8  freq_valid = 0;
        if (crossings >= 2)
        {
            /* window_ms in units of 1ms */
            uint32 window_ms = (uint32)n * (uint32)AdcScope.decimation * 10;
            /* freq = crossings / window_s = crossings × 1000 / window_ms */
            freq_hz   = (crossings * 1000UL) / window_ms;
            freq_valid = 1;
        }
        else if (crossings == 1 && n == ADC_SCOPE_BUF)
        {
            /* Signal is too slow for this window — show "<1Hz" */
            freq_hz   = 0;
            freq_valid = 1;
        }

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
 * Draw one channel waveform (x = ADC_WAVE_X0..ADC_WAVE_X1)
 * ----------------------------------------------------------------------- */
static void draw_channel(uint8 screen[64][16],
                         const uint16 *buf, int head, int count,
                         int cy, int half_h, int top, int bot)
{
    if (count < 2) return;

    int n = count;
    if (n > ADC_WAVE_W) n = ADC_WAVE_W;   /* Clip to visible width */

    int x_start = ADC_WAVE_X0 + (ADC_WAVE_W - n); /* right-aligned */
    int prev_y  = -1;

    for (int i = 0; i < n; i++)
    {
        int x = x_start + i;
        if (x < ADC_WAVE_X0 || x > ADC_WAVE_X1) continue;

        /* Get sample from ring buffer (newest at right) */
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

/* -----------------------------------------------------------------------
 * Render_AdcScope
 *
 * Layout (128×64):
 *
 *  x=0..11 : labels ("C0", "C1")
 *  x=12..82: waveforms (71px wide)
 *  x=83    : vertical separator
 *  x=84..127: measurement readout (44px = 7 chars × 6px + 2px margin)
 *
 *  y=0..25  : CH0 area
 *  y=26..27 : separator
 *  y=28..52 : CH1 area
 *  y=53..63 : status bar
 *
 * Measurement area per channel (3 rows × 8px):
 *   Row 0 (y=0 / y=28):  Vpp: X.XXV
 *   Row 1 (y=8 / y=36):  Avg: X.XXV
 *   Row 2 (y=16/ y=44):  Frq: XXXHz
 * ----------------------------------------------------------------------- */
void Render_AdcScope(void)
{
    /* Run measurements every frame (fast, pure math on buffer) */
    AdcScope_Measure();

    uint8 screen[64][16];
    for (int i = 0; i < 64; i++)
        for (int c = 0; c < 16; c++)
            screen[i][c] = 0x00;

    char buf[12];

    /* ---- Vertical separator between waveform and measurements ---- */
    for (int y = 0; y < 53; y++) dp(screen, 83, y);

    /* ====== CH0 ====== */
    ds6(screen, 0, 0, "C0");

    /* Centre dashed line */
    for (int x = ADC_WAVE_X0; x <= ADC_WAVE_X1; x += 4)
        dp(screen, x, ADC_CH0_CY);

    /* Waveform */
    draw_channel(screen, AdcScope.ch0_buf, AdcScope.head, AdcScope.count,
                 ADC_CH0_CY, ADC_WAVE_H, ADC_CH0_TOP, ADC_CH0_BOT);

    /* Measurements CH0 (x=85) */
    if (AdcScope.count >= 4)
    {
        /* Vpp */
        uint32 vpp0 = AdcScope.meas[0].vpp_mv;
        if (vpp0 < 1000)
            snprintf(buf, sizeof(buf), "P%lumV", (unsigned long)vpp0);
        else
            snprintf(buf, sizeof(buf), "P%.2fV", (float)vpp0 / 1000.0f);
        ds6(screen, 85, 0, buf);

        /* Avg */
        uint32 avg0 = AdcScope.meas[0].avg_mv;
        if (avg0 < 1000)
            snprintf(buf, sizeof(buf), "A%lumV", (unsigned long)avg0);
        else
            snprintf(buf, sizeof(buf), "A%.2fV", (float)avg0 / 1000.0f);
        ds6(screen, 85, 9, buf);

        /* Freq */
        if (AdcScope.meas[0].freq_valid)
        {
            uint32 f = AdcScope.meas[0].freq_hz;
            if (f == 0)
                snprintf(buf, sizeof(buf), "F<1Hz");
            else if (f < 1000)
                snprintf(buf, sizeof(buf), "F%luHz", (unsigned long)f);
            else
                snprintf(buf, sizeof(buf), "F%.1fk", (float)f / 1000.0f);
        }
        else
            snprintf(buf, sizeof(buf), "F:---");
        ds6(screen, 85, 18, buf);
    }
    else
    {
        ds6(screen, 85, 0, "wait");
    }

    /* ---- Separator ---- */
    dline_h(screen, 26, 0, 127);
    dline_h(screen, 27, 0, 127);

    /* ====== CH1 ====== */
    ds6(screen, 0, 28, "C1");

    for (int x = ADC_WAVE_X0; x <= ADC_WAVE_X1; x += 4)
        dp(screen, x, ADC_CH1_CY);

    draw_channel(screen, AdcScope.ch1_buf, AdcScope.head, AdcScope.count,
                 ADC_CH1_CY, ADC_WAVE_H, ADC_CH1_TOP, ADC_CH1_BOT);

    /* Measurements CH1 */
    if (AdcScope.count >= 4)
    {
        uint32 vpp1 = AdcScope.meas[1].vpp_mv;
        if (vpp1 < 1000)
            snprintf(buf, sizeof(buf), "P%lumV", (unsigned long)vpp1);
        else
            snprintf(buf, sizeof(buf), "P%.2fV", (float)vpp1 / 1000.0f);
        ds6(screen, 85, 28, buf);

        uint32 avg1 = AdcScope.meas[1].avg_mv;
        if (avg1 < 1000)
            snprintf(buf, sizeof(buf), "A%lumV", (unsigned long)avg1);
        else
            snprintf(buf, sizeof(buf), "A%.2fV", (float)avg1 / 1000.0f);
        ds6(screen, 85, 37, buf);

        if (AdcScope.meas[1].freq_valid)
        {
            uint32 f = AdcScope.meas[1].freq_hz;
            if (f == 0)
                snprintf(buf, sizeof(buf), "F<1Hz");
            else if (f < 1000)
                snprintf(buf, sizeof(buf), "F%luHz", (unsigned long)f);
            else
                snprintf(buf, sizeof(buf), "F%.1fk", (float)f / 1000.0f);
        }
        else
            snprintf(buf, sizeof(buf), "F:---");
        ds6(screen, 85, 46, buf);
    }
    else
    {
        ds6(screen, 85, 28, "wait");
    }

    /* ---- Status bar (y=53..63) ---- */
    dline_h(screen, 53, 0, 127);

    if (AdcScope.paused)
        ds6(screen, 0, 55, "PSE");

    /* Time scale */
    uint32 span_ms = (uint32)AdcScope.count * (uint32)AdcScope.decimation * 10;
    if (span_ms == 0) span_ms = (uint32)ADC_SCOPE_BUF * AdcScope.decimation * 10;
    if (span_ms < 1000)
        snprintf(buf, sizeof(buf), "x%u %lums", AdcScope.decimation, (unsigned long)span_ms);
    else
        snprintf(buf, sizeof(buf), "x%u %.1fs", AdcScope.decimation, (float)span_ms / 1000.0f);
    ds6(screen, 22, 55, buf);

    ds6(screen, 82, 55, "K0:P");

    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
