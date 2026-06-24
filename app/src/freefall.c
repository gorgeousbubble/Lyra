/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       freefall.c
 * @brief      Free-fall and impact detection + OLED display
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "freefall.h"
#include "beep.h"
#include "oled.h"
#include "oled_i2c.h"
#include <stdio.h>

/* ------------------------------------------------------------------
 * Integer sqrt — Newton's method, good enough for embedded use
 * ------------------------------------------------------------------ */
static uint32 isqrt32(uint32 n)
{
    if (n == 0) return 0;
    uint32 x = n;
    uint32 y = (x + 1) / 2;
    while (y < x)
    {
        x = y;
        y = (x + n / x) / 2;
    }
    return x;
}

/* Global detector state */
FreeFallDetector FFDet = {
    FF_STATE_IDLE, 0, 0, 0, 0,
    {{0}}, 0, 0,
    0, 0, 0.0f, 0, 0   /* elapsed_ms=0, beep_ms=0 */
};

/* ------------------------------------------------------------------
 * FreeFall_Update
 * ------------------------------------------------------------------ */
void FreeFall_Update(int16 ax, int16 ay, int16 az, uint32 rtc_seconds)
{
    FFDet.elapsed_ms += 10;

    /* Compute magnitude squared (use int64 to avoid overflow) */
    int64 ax64 = (int64)ax;
    int64 ay64 = (int64)ay;
    int64 az64 = (int64)az;
    uint64 mag_sq64 = (uint64)(ax64*ax64 + ay64*ay64 + az64*az64);

    /* Clamp to uint32 for thresholds — max useful value is ~(3g)² = 7.2e9,
       which overflows uint32 (4.3e9). Keep as uint64 for comparisons. */

    switch (FFDet.state)
    {
    /* ---- IDLE: watch for free-fall onset ---- */
    case FF_STATE_IDLE:
        if (mag_sq64 < (uint64)FF_FREEFALL_THRESHOLD_SQ)
        {
            FFDet.state        = FF_STATE_FALLING;
            FFDet.fall_start_ms = FFDet.elapsed_ms;
        }
        break;

    /* ---- FALLING: confirm duration then log event ---- */
    case FF_STATE_FALLING:
        if (mag_sq64 < (uint64)FF_FREEFALL_THRESHOLD_SQ)
        {
            /* Still below threshold — check if we've passed minimum duration */
            uint32 dur = FFDet.elapsed_ms - FFDet.fall_start_ms;
            if (dur >= FF_FREEFALL_MIN_MS)
            {
                /* Confirmed free-fall: log event and open impact window */
                FFDet.fall_duration_ms = dur;
                FFDet.total_falls++;

                FreeFall_Event ev;
                ev.type                = FF_EVENT_FREEFALL;
                ev.timestamp_s         = rtc_seconds;
                ev.freefall_duration_ms = dur;
                ev.peak_mag_sq         = 0;
                ev.peak_g              = 0.0f;

                FFDet.events[FFDet.event_head] = ev;
                FFDet.event_head = (FFDet.event_head + 1) % FF_MAX_EVENTS;
                if (FFDet.event_count < FF_MAX_EVENTS)
                    FFDet.event_count++;

                /* Non-blocking beep: set counter; beep will run for FF_BEEP_DURATION_MS */
                FFDet.beep_ms = FF_BEEP_DURATION_MS;
                Beep_On();

                FFDet.state           = FF_STATE_IMPACT;
                FFDet.impact_window_ms = 0;
                FFDet.impact_peak_sq   = 0;
            }
        }
        else
        {
            /* Recovered without reaching minimum duration — false alarm */
            FFDet.state = FF_STATE_IDLE;
        }
        break;

    /* ---- IMPACT: watch for high-g impact within 500ms of fall end ---- */
    case FF_STATE_IMPACT:
        /* Non-blocking beep: count down and turn off when expired */
        if (FFDet.beep_ms > 0)
        {
            FFDet.beep_ms = (FFDet.beep_ms > 10) ? (FFDet.beep_ms - 10) : 0;
            if (FFDet.beep_ms == 0)
                Beep_Off();
        }

        FFDet.impact_window_ms += 10;

        /* Track peak impact force */
        if (mag_sq64 > (uint64)FFDet.impact_peak_sq)
        {
            uint32 clamped = (mag_sq64 > 0xFFFFFFFFULL)
                             ? 0xFFFFFFFFUL
                             : (uint32)mag_sq64;
            FFDet.impact_peak_sq = clamped;
        }

        if (mag_sq64 >= (uint64)FF_IMPACT_THRESHOLD_SQ)
        {
            /* Impact detected: compute peak in g */
            /* |a| = sqrt(impact_peak_sq); g = |a| / 16384 */
            uint32 peak_lsb = isqrt32(FFDet.impact_peak_sq);
            float  peak_g   = (float)peak_lsb / 16384.0f;

            FFDet.total_impacts++;
            if (peak_g > FFDet.max_impact_g)
                FFDet.max_impact_g = peak_g;

            /* Update the most recent free-fall event with impact info */
            int last = (FFDet.event_head - 1 + FF_MAX_EVENTS) % FF_MAX_EVENTS;
            FFDet.events[last].type       = FF_EVENT_IMPACT;
            FFDet.events[last].peak_mag_sq = FFDet.impact_peak_sq;
            FFDet.events[last].peak_g      = peak_g;

            FFDet.state = FF_STATE_IDLE;
        }
        else if (FFDet.impact_window_ms >= 500)
        {
            /* Impact window expired: no significant impact detected */
            FFDet.state = FF_STATE_IDLE;
        }
        break;
    }
}

/* ------------------------------------------------------------------
 * FreeFall_Clear
 * ------------------------------------------------------------------ */
void FreeFall_Clear(void)
{
    FFDet.event_count   = 0;
    FFDet.event_head    = 0;
    FFDet.total_falls   = 0;
    FFDet.total_impacts = 0;
    FFDet.max_impact_g  = 0.0f;
    FFDet.state         = FF_STATE_IDLE;
    FFDet.beep_ms       = 0;
    Beep_Off();
}

/* ------------------------------------------------------------------
 * Render helpers
 * ------------------------------------------------------------------ */
static void dp(uint8 screen[64][16], int x, int y)
{
    if ((unsigned)x < 128 && (unsigned)y < 64)
        screen[y][x >> 3] |= (0x01 << (7 - (x & 7)));
}

static void dline_h(uint8 screen[64][16], int y, int x0, int x1)
{
    for (int x = x0; x <= x1; x++) dp(screen, x, y);
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

/* ------------------------------------------------------------------
 * Render_FreeFall
 *
 * Layout (128x64):
 *
 *   Line 0  (y= 0): "FREE FALL DETECT"  title
 *   Line 1  (y= 9): separator
 *   Line 2  (y=11): "FALL:XX  IMPACT:XX"
 *   Line 3  (y=20): "MAX: X.XXg"
 *   Line 4  (y=29): separator
 *   Lines 5-7 (y=31..55): Last 3 events (newest first)
 *     "FF  XXs  XXXms"  for free-fall
 *     "IMP XXs  X.Xg"   for impact
 *   Line 8  (y=57): "KEY0:CLEAR"
 * ------------------------------------------------------------------ */
void Render_FreeFall(void)
{
    uint8 screen[64][16];
    for (int i = 0; i < 64; i++)
        for (int c = 0; c < 16; c++)
            screen[i][c] = 0x00;

    char buf[24];

    /* Title */
    ds6(screen, 4, 0, "FREE FALL DETECT");

    dline_h(screen, 9, 0, 127);

    /* Statistics */
    snprintf(buf, sizeof(buf), "FALL:%-3lu IMP:%-3lu",
             (unsigned long)FFDet.total_falls,
             (unsigned long)FFDet.total_impacts);
    ds6(screen, 4, 11, buf);

    snprintf(buf, sizeof(buf), "MAX IMPACT:%.2fg", FFDet.max_impact_g);
    ds6(screen, 4, 20, buf);

    dline_h(screen, 29, 0, 127);

    /* Status indicator */
    if (FFDet.state == FF_STATE_FALLING)
        ds6(screen, 4, 31, ">> FALLING...");
    else if (FFDet.state == FF_STATE_IMPACT)
        ds6(screen, 4, 31, ">> IMPACT WIN");
    else
    {
        /* Show last 3 events (newest first).
         * Ring buffer: events[event_head-1] is newest, events[event_head-2] is second newest, etc.
         * Only walk back as far as event_count entries (don't read uninitialised slots). */
        int shown = 0;
        int available = (FFDet.event_count < 3) ? FFDet.event_count : 3;
        for (int i = 0; i < available; i++)
        {
            /* i=0 → newest, i=1 → second newest, ... */
            int idx = (FFDet.event_head - 1 - i + FF_MAX_EVENTS) % FF_MAX_EVENTS;
            FreeFall_Event *ev = &FFDet.events[idx];
            int y = 31 + shown * 10;

            if (ev->type == FF_EVENT_FREEFALL)
            {
                snprintf(buf, sizeof(buf), "FF  %lus %lums",
                         (unsigned long)(ev->timestamp_s % 1000),
                         (unsigned long)ev->freefall_duration_ms);
            }
            else
            {
                snprintf(buf, sizeof(buf), "IMP %lus %.1fg",
                         (unsigned long)(ev->timestamp_s % 1000),
                         ev->peak_g);
            }
            ds6(screen, 4, y, buf);
            shown++;
        }

        if (FFDet.event_count == 0)
            ds6(screen, 4, 31, "-- no events --");
    }

    /* Hint */
    ds6(screen, 22, 56, "KEY0:CLEAR");

    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
