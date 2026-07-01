/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       pedometer.c
 * @brief      Step counter (pedometer) using MPU6050 accelerometer magnitude
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "pedometer.h"
#include "oled_i2c.h"
#include <stdio.h>

/* Global pedometer state:
 * step_count=0, last_step_ms=0, above_threshold=0,
 * dyn_min=MAX, dyn_max=0, threshold=INIT, win_count=0 */
PedometerState Pedometer = {0, 0, 0, 0xFFFFFFFFUL, 0, STEP_THRESHOLD_INIT, 0};

/* Internal elapsed time accumulator (ms) */
static uint32 s_elapsed_total_ms = 0;

/* Integer square root (Newton's method) for uint64 magnitude-squared values. */
static uint32 isqrt64(uint64 n)
{
    if (n == 0) return 0;
    uint64 x = n;
    uint64 y = (x + 1) / 2;
    while (y < x) { x = y; y = (x + n / x) / 2; }
    return (uint32)x;
}

/*
 * Pedometer_Update
 * ----------------
 * Called every 10ms from the main loop after MPU6050 is read.
 *
 * Adaptive threshold-crossing peak detection:
 *   1. Compute |a| = sqrt(ax^2+ay^2+az^2) in LSB.
 *   2. Track the min/max envelope of |a| over a 0.5s window; every window
 *      the detection threshold is refreshed to the envelope midpoint.
 *   3. An amplitude gate rejects windows whose peak-to-peak swing is too
 *      small to be real walking (rest, typing, tremor).
 *   4. A step is counted on a rising crossing of the threshold, subject to
 *      the STEP_MIN_INTERVAL_MS debounce.
 */
void Pedometer_Update(int16 ax, int16 ay, int16 az, uint32 elapsed_ms)
{
    s_elapsed_total_ms += elapsed_ms;

    // Magnitude squared (int64 to prevent overflow) then magnitude via isqrt.
    int64 ax64 = (int64)ax, ay64 = (int64)ay, az64 = (int64)az;
    uint64 mag_sq = (uint64)(ax64 * ax64 + ay64 * ay64 + az64 * az64);
    uint32 mag = isqrt64(mag_sq);

    // --- Update the envelope for the current window ---
    if (mag > Pedometer.dyn_max) Pedometer.dyn_max = mag;
    if (mag < Pedometer.dyn_min) Pedometer.dyn_min = mag;
    Pedometer.win_count++;

    if (Pedometer.win_count >= STEP_ADAPT_WINDOW)
    {
        uint32 amp = (Pedometer.dyn_max > Pedometer.dyn_min)
                     ? (Pedometer.dyn_max - Pedometer.dyn_min) : 0;
        if (amp >= STEP_MIN_AMPLITUDE)
        {
            // Enough movement: set threshold to the envelope midpoint.
            Pedometer.threshold = (Pedometer.dyn_min + Pedometer.dyn_max) / 2;
        }
        else
        {
            // Too little movement (rest/noise): raise threshold out of reach
            // so no steps are counted this period.
            Pedometer.threshold = 0xFFFFFFFFUL;
        }
        // Reset envelope for the next window.
        Pedometer.dyn_min   = 0xFFFFFFFFUL;
        Pedometer.dyn_max   = 0;
        Pedometer.win_count = 0;
    }

    // --- Rising-edge detection against the adaptive threshold ---
    if (mag >= Pedometer.threshold)
    {
        if (!Pedometer.above_threshold)
        {
            uint32 dt = s_elapsed_total_ms - Pedometer.last_step_ms;
            if (dt >= STEP_MIN_INTERVAL_MS)
            {
                if (Pedometer.step_count < STEP_MAX_COUNT)
                    Pedometer.step_count++;
                Pedometer.last_step_ms = s_elapsed_total_ms;
            }
            Pedometer.above_threshold = 1;
        }
    }
    else
    {
        Pedometer.above_threshold = 0;
    }
}

/*
 * Pedometer_Reset
 * ---------------
 * Resets step count, timing and the adaptive envelope.
 */
void Pedometer_Reset(void)
{
    Pedometer.step_count      = 0;
    Pedometer.last_step_ms    = s_elapsed_total_ms; // Avoid false step on resume
    Pedometer.above_threshold = 0;
    Pedometer.dyn_min         = 0xFFFFFFFFUL;
    Pedometer.dyn_max         = 0;
    Pedometer.threshold       = STEP_THRESHOLD_INIT;
    Pedometer.win_count       = 0;
}

/*
 * Render_Pedometer
 * ----------------
 * Draws the pedometer UI on the 128x64 OLED.
 * Only redraws when step count or distance changes to avoid flicker.
 *
 * Layout (pages 0-7, each page = 8 pixel rows):
 *   Page 0      : "STEPS" label centered
 *   Pages 1-3   : step count in 12x24 large font, centered
 *   Page 4      : separator dashes
 *   Page 5      : distance string "DIST: XXX.Xm"
 *   Page 7      : "KEY0:RESET" hint
 */
void Render_Pedometer(void)
{
    static uint32 last_rendered_count = 0xFFFFFFFF; // Force first draw

    // Only redraw when step count changes — eliminates flicker
    if (Pedometer.step_count == last_rendered_count)
        return;
    last_rendered_count = Pedometer.step_count;

    // Clear screen once per update (not every main loop iteration)
    Oled_I2C_Clean();

    // --- "STEPS" label at page 0, centered (5*6=30px, start x=49) ---
    Oled_I2C_Put_Str_6x8(49, 0, "STEPS");

    // --- Step count in 12x24 font at pages 1-3, centered ---
    char steps_str[8];
    snprintf(steps_str, sizeof(steps_str), "%lu", (unsigned long)Pedometer.step_count);

    int steps_len = 0;
    const char *p = steps_str;
    while (*p++) steps_len++;
    /* Keep as int; clamp to [0, 127] to guard against any overflow edge case */
    int steps_x = (128 - steps_len * 12) / 2;
    if (steps_x < 0)   steps_x = 0;
    if (steps_x > 127) steps_x = 0;

    Oled_I2C_Put_Str_12x24((uint8)steps_x, 1, (uint8 *)steps_str);

    // --- Separator at page 4 ---
    Oled_I2C_Put_Str_6x8(0, 4, "----------------");

    // --- Distance at page 5 ---
    uint32 dist_cm = Pedometer.step_count * (uint32)STEP_STRIDE_CM;
    uint32 dist_m  = dist_cm / 100;
    uint32 dist_dm = (dist_cm % 100) / 10;

    char dist_str[20];
    snprintf(dist_str, sizeof(dist_str), "DIST:%lu.%lum", (unsigned long)dist_m, (unsigned long)dist_dm);

    int dist_len = 0;
    p = dist_str;
    while (*p++) dist_len++;
    /* Keep as int; clamp to [0, 127] to guard against overflow */
    int dist_x = (128 - dist_len * 6) / 2;
    if (dist_x < 0)   dist_x = 0;
    if (dist_x > 127) dist_x = 0;

    Oled_I2C_Put_Str_6x8((uint8)dist_x, 5, (uint8 *)dist_str);

    // --- "KEY0:RESET" hint at page 7 ---
    Oled_I2C_Put_Str_6x8(28, 7, "KEY0:RESET");
}
