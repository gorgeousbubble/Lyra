/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       adc_scope.c
 * @brief      Dual-channel ADC oscilloscope display
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
AdcScopeState AdcScope = {{0}, {0}, 0, 0, 0, 1, 0};

/* -----------------------------------------------------------------------
 * AdcScope_Update
 * Push one sample pair, obeying the decimation factor.
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
void AdcScope_Toggle_Pause(void)
{
    AdcScope.paused = !AdcScope.paused;
}

void AdcScope_Dec_Inc(void)
{
    if (AdcScope.decimation < ADC_SCOPE_DEC_MAX)
        AdcScope.decimation <<= 1;   /* ×1 → ×2 → ×4 → ×8 */
}

void AdcScope_Dec_Dec(void)
{
    if (AdcScope.decimation > ADC_SCOPE_DEC_MIN)
        AdcScope.decimation >>= 1;   /* ×8 → ×4 → ×2 → ×1 */
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
 * Draw one waveform channel into the frame buffer.
 *
 * buf[]:   ring buffer of 12-bit ADC samples
 * head:    next-write index
 * count:   valid sample count
 * cy:      centre pixel row
 * half_h:  half-height in pixels (ADC_WAVE_H)
 * top/bot: clamping bounds
 * ----------------------------------------------------------------------- */
static void draw_channel(uint8 screen[64][16],
                         const uint16 *buf, int head, int count,
                         int cy, int half_h, int top, int bot)
{
    if (count < 2) return;

    int n = count;
    if (n > ADC_SCOPE_BUF) n = ADC_SCOPE_BUF;

    /* We display last n samples right-aligned in x = (128-n)..127 */
    int prev_y = -1;

    for (int i = 0; i < n; i++)
    {
        int x = (128 - n) + i;
        if (x < 0 || x > 127) continue;

        int ring_idx = (head - n + i + ADC_SCOPE_BUF) % ADC_SCOPE_BUF;
        uint16 v = buf[ring_idx];

        /* Map 0..4095 → cy+half_h..cy-half_h (invert: higher V = higher pixel) */
        int pix = cy + half_h - (int)((uint32)v * (2 * half_h) / 4095);
        if (pix < top)  pix = top;
        if (pix > bot)  pix = bot;

        dp(screen, x, pix);

        /* Connect to previous sample */
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
 *   y= 0..30  CH0 waveform area (ADC0_DP0)
 *             Left edge: "C0" label (6px) + centre dashed line
 *   y=31..32  separator
 *   y=33..63  CH1 waveform area (ADC0_DM0)
 *             Left edge: "C1" label
 *
 * Right margin (x=118..127): current voltage readout
 *   CH0 value at y=0, CH1 value at y=33
 *
 * Bottom right: pause indicator + decimation
 * ----------------------------------------------------------------------- */
void Render_AdcScope(void)
{
    uint8 screen[64][16];
    for (int i = 0; i < 64; i++)
        for (int c = 0; c < 16; c++)
            screen[i][c] = 0x00;

    char buf[10];

    /* ---- CH0 area ---- */
    /* Centre dashed line */
    for (int x = 12; x < 118; x += 4)
        dp(screen, x, ADC_CH0_CY);

    /* Label */
    ds6(screen, 0, 0, "C0");

    /* Waveform */
    draw_channel(screen, AdcScope.ch0_buf, AdcScope.head, AdcScope.count,
                 ADC_CH0_CY, ADC_WAVE_H, ADC_CH0_TOP, ADC_CH0_BOT);

    /* Current value (last sample) as raw code */
    if (AdcScope.count > 0)
    {
        int last0 = (AdcScope.head - 1 + ADC_SCOPE_BUF) % ADC_SCOPE_BUF;
        uint16 v0 = AdcScope.ch0_buf[last0];
        /* Convert 12-bit → millivolts: Vref=3300mV, 4095 steps */
        uint32 mv0 = ((uint32)v0 * 3300) / 4095;
        snprintf(buf, sizeof(buf), "%lumV", (unsigned long)mv0);
        ds6(screen, 118 - (int)(6 * (buf[0] ? 1:0) * 5), 0, buf);
    }

    /* ---- Separator ---- */
    dline_h(screen, 31, 0, 127);
    dline_h(screen, 32, 0, 127);

    /* ---- CH1 area ---- */
    /* Centre dashed line */
    for (int x = 12; x < 118; x += 4)
        dp(screen, x, ADC_CH1_CY);

    ds6(screen, 0, 33, "C1");

    draw_channel(screen, AdcScope.ch1_buf, AdcScope.head, AdcScope.count,
                 ADC_CH1_CY, ADC_WAVE_H, ADC_CH1_TOP, ADC_CH1_BOT);

    if (AdcScope.count > 0)
    {
        int last1 = (AdcScope.head - 1 + ADC_SCOPE_BUF) % ADC_SCOPE_BUF;
        uint16 v1 = AdcScope.ch1_buf[last1];
        uint32 mv1 = ((uint32)v1 * 3300) / 4095;
        snprintf(buf, sizeof(buf), "%lumV", (unsigned long)mv1);
        ds6(screen, 118 - (int)(6 * (buf[0] ? 1:0) * 5), 33, buf);
    }

    /* ---- Status overlay (bottom-right of CH1 area) ---- */
    /* Pause indicator */
    if (AdcScope.paused)
        ds6(screen, 0, 56, "PAUSE");

    /* Time scale: each pixel = decimation × 10ms */
    uint32 span_ms = (uint32)ADC_SCOPE_BUF * AdcScope.decimation * 10;
    if (span_ms < 1000)
        snprintf(buf, sizeof(buf), "x%u %lums", AdcScope.decimation, (unsigned long)span_ms);
    else
        snprintf(buf, sizeof(buf), "x%u %.1fs", AdcScope.decimation, (float)span_ms / 1000.0f);
    ds6(screen, 60, 56, buf);

    /* Hint */
    ds6(screen, 0, 57, "K0:PSE K2:- K3:+");

    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
