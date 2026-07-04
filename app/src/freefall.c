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
    FF_STATE_IDLE,
    0,          /* fall_duration_ms  */
    0,          /* impact_window_ms  */
    0,          /* impact_peak_sq    */
    {{FF_EVENT_FREEFALL}}, /* events (element 0 .type set with enum, rest zero) */
    0,          /* event_head        */
    0,          /* event_count       */
    0,          /* total_falls       */
    0,          /* total_impacts     */
    0.0f,       /* max_impact_g      */
    0           /* beep_ms           */
};

/* ------------------------------------------------------------------
 * FreeFall_Update
 *
 * Call every 10 ms from the main loop.
 *
 * Timing design — wrap-safe relative counters:
 *
 *   fall_duration_ms  accumulates how long we have been continuously
 *   below the free-fall threshold in the current FALLING phase.
 *   It is reset to 0 when we enter FALLING and incremented by
 *   FF_CALL_INTERVAL_MS (10) each call while the condition holds.
 *   It never wraps: the maximum meaningful value is FF_FREEFALL_MIN_MS
 *   (80 ms) + one interval = 90 ms — far below uint32 range.
 *
 *   impact_window_ms uses the same pattern for the IMPACT timeout.
 *
 *   Old code used a single global `elapsed_ms` that incremented
 *   forever and computed durations as (elapsed_ms - fall_start_ms).
 *   After ~49.7 days the uint32 wrapped, producing a huge positive
 *   delta and triggering a phantom free-fall from IDLE.
 * ------------------------------------------------------------------ */
#define FF_CALL_INTERVAL_MS  10   /* fixed 10 ms call period */

void FreeFall_Update(int16 ax, int16 ay, int16 az, uint32 rtc_seconds)
{
    /* Compute magnitude squared (use int64 to avoid overflow on int16 inputs) */
    int64  ax64    = (int64)ax;
    int64  ay64    = (int64)ay;
    int64  az64    = (int64)az;
    uint64 mag_sq64 = (uint64)(ax64*ax64 + ay64*ay64 + az64*az64);

    /* Non-blocking beep countdown — runs every call regardless of state so the
     * buzzer is always silenced after FF_BEEP_DURATION_MS. Previously this lived
     * only in the IMPACT case, so an impact that ended IMPACT before the beep
     * elapsed left the buzzer stuck on until the next free-fall. */
    if (FFDet.beep_ms > 0)
    {
        FFDet.beep_ms = (FFDet.beep_ms > FF_CALL_INTERVAL_MS)
                        ? (FFDet.beep_ms - FF_CALL_INTERVAL_MS) : 0;
        if (FFDet.beep_ms == 0)
            Beep_Off();
    }

    switch (FFDet.state)
    {
    /* ---- IDLE: watch for free-fall onset ---- */
    case FF_STATE_IDLE:
        if (mag_sq64 < (uint64)FF_FREEFALL_THRESHOLD_SQ)
        {
            /* Enter FALLING: start a fresh relative duration counter.
             * No absolute timestamp needed — avoids uint32 wrap-around. */
            FFDet.state           = FF_STATE_FALLING;
            FFDet.fall_duration_ms = FF_CALL_INTERVAL_MS; /* count this sample */
        }
        break;

    /* ---- FALLING: confirm minimum duration then log event ---- */
    case FF_STATE_FALLING:
        if (mag_sq64 < (uint64)FF_FREEFALL_THRESHOLD_SQ)
        {
            /* Still below threshold — accumulate duration */
            FFDet.fall_duration_ms += FF_CALL_INTERVAL_MS;

            if (FFDet.fall_duration_ms >= FF_FREEFALL_MIN_MS)
            {
                /* Confirmed free-fall: log event and open impact window */
                FFDet.total_falls++;

                FreeFall_Event ev;
                ev.type                 = FF_EVENT_FREEFALL;
                ev.timestamp_s          = rtc_seconds;
                ev.freefall_duration_ms = FFDet.fall_duration_ms;
                ev.peak_mag_sq          = 0;
                ev.peak_g               = 0.0f;

                FFDet.events[FFDet.event_head] = ev;
                FFDet.event_head = (FFDet.event_head + 1) % FF_MAX_EVENTS;
                if (FFDet.event_count < FF_MAX_EVENTS)
                    FFDet.event_count++;

                /* Non-blocking beep */
                FFDet.beep_ms = FF_BEEP_DURATION_MS;
                Beep_On();

                /* Transition: reset IMPACT phase counter */
                FFDet.state            = FF_STATE_IMPACT;
                FFDet.impact_window_ms = 0;
                FFDet.impact_peak_sq   = 0;
            }
        }
        else
        {
            /* Recovered before minimum duration — false alarm, return to IDLE */
            FFDet.state            = FF_STATE_IDLE;
            FFDet.fall_duration_ms = 0;
        }
        break;

    /* ---- IMPACT: watch for high-g impact within 500 ms of fall end ---- */
    case FF_STATE_IMPACT:
        FFDet.impact_window_ms += FF_CALL_INTERVAL_MS;

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
            uint32 peak_lsb = isqrt32(FFDet.impact_peak_sq);
            float  peak_g   = (float)peak_lsb / 16384.0f;

            FFDet.total_impacts++;
            if (peak_g > FFDet.max_impact_g)
                FFDet.max_impact_g = peak_g;

            /* Annotate the most recent free-fall event with impact data */
            int last = (FFDet.event_head - 1 + FF_MAX_EVENTS) % FF_MAX_EVENTS;
            FFDet.events[last].type        = FF_EVENT_IMPACT;
            FFDet.events[last].peak_mag_sq = FFDet.impact_peak_sq;
            FFDet.events[last].peak_g      = peak_g;

            FFDet.state = FF_STATE_IDLE;
        }
        else if (FFDet.impact_window_ms >= 500)
        {
            /* Impact window expired without a significant hit */
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
    FFDet.event_count      = 0;
    FFDet.event_head       = 0;
    FFDet.total_falls      = 0;
    FFDet.total_impacts    = 0;
    FFDet.max_impact_g     = 0.0f;
    FFDet.state            = FF_STATE_IDLE;
    FFDet.fall_duration_ms = 0;
    FFDet.impact_window_ms = 0;
    FFDet.impact_peak_sq   = 0;
    FFDet.beep_ms          = 0;
    Beep_Off();
}

/* ------------------------------------------------------------------
 * Render helpers — thin aliases onto the shared framebuf module
 * ------------------------------------------------------------------ */
#include "framebuf.h"
#define dp(s,x,y)        fb_pixel((s),(x),(y))
#define dline_h(s,y,x0,x1) fb_hline((s),(y),(x0),(x1))
#define dc6(s,px,py,ch)  fb_char6((s),(px),(py),(ch))
#define ds6(s,px,py,str) fb_str6((s),(px),(py),(str))

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
    /* Use the shared global framebuffer (saves 1 KB of stack per call). */
    #define screen g_fb
    fb_clear(g_fb);

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
