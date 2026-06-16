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

/* Global pedometer state */
PedometerState Pedometer = {0, 0, 0};

/* Internal elapsed time accumulator (ms) */
static uint32 s_elapsed_total_ms = 0;

/*
 * Pedometer_Update
 * ----------------
 * Called every 10ms from the main loop after MPU6050 is read.
 * Detects steps via threshold-crossing peak detection on |a| squared.
 */
void Pedometer_Update(int16 ax, int16 ay, int16 az, uint32 elapsed_ms)
{
    s_elapsed_total_ms += elapsed_ms;

    // Compute magnitude squared using int64 to prevent overflow.
    // int16 max = 32767; 32767^2 * 3 = ~3.2e9 which exceeds int32 max (2.1e9).
    int64 ax64 = (int64)ax;
    int64 ay64 = (int64)ay;
    int64 az64 = (int64)az;
    uint64 mag_sq = (uint64)(ax64 * ax64 + ay64 * ay64 + az64 * az64);

    // Rising-edge detection: cross threshold from below
    if (mag_sq >= (uint64)STEP_THRESHOLD_SQ)
    {
        if (!Pedometer.above_threshold)
        {
            // Just entered the "above threshold" zone: check debounce interval
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
        // Below threshold: reset edge flag so next crossing is detected
        Pedometer.above_threshold = 0;
    }
}

/*
 * Pedometer_Reset
 * ---------------
 * Resets step count and timing.
 */
void Pedometer_Reset(void)
{
    Pedometer.step_count    = 0;
    Pedometer.last_step_ms  = s_elapsed_total_ms; // Avoid false step on resume
    Pedometer.above_threshold = 0;
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
    uint8 steps_x = (uint8)((128 - steps_len * 12) / 2);

    Oled_I2C_Put_Str_12x24(steps_x, 1, (uint8 *)steps_str);

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
    uint8 dist_x = (uint8)((128 - dist_len * 6) / 2);

    Oled_I2C_Put_Str_6x8(dist_x, 5, (uint8 *)dist_str);

    // --- "KEY0:RESET" hint at page 7 ---
    Oled_I2C_Put_Str_6x8(28, 7, "KEY0:RESET");
}
