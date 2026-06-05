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
#include "oled.h"
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

    // Compute magnitude squared (avoid sqrt for performance)
    // Cast to int32 first to prevent overflow in multiplication
    int32 ax32 = (int32)ax;
    int32 ay32 = (int32)ay;
    int32 az32 = (int32)az;
    uint32 mag_sq = (uint32)(ax32 * ax32 + ay32 * ay32 + az32 * az32);

    // Rising-edge detection: cross threshold from below
    if (mag_sq >= STEP_THRESHOLD_SQ)
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
 *
 * Layout:
 *   Row 0 (y=0..7)   : "STEPS"  label (centered, 6x8)
 *   Row 1 (y=10..25) : step count, up to 5 digits (12x24 large font, centered)
 *   Row 2 (y=28..35) : horizontal separator line
 *   Row 3 (y=38..45) : "DIST"  label (centered, 6x8)
 *   Row 4 (y=48..55) : distance in meters (6x8), e.g. "123.4 m"
 *   Row 5 (y=57..63) : "KEY0:RESET" hint (6x8, small)
 */
void Render_Pedometer(void)
{
    uint8 screen[64][16] = {0};

    // Helper: draw a single 6x8 character at pixel (px, py)
    #define DRAW_CHAR6x8(px, py, ch) do {                                    \
        uint8 _c = (uint8)(ch) - 32;                                         \
        if (_c < 96) {                                                        \
            for (int _col = 0; _col < 6; _col++) {                           \
                uint8 _fb = Oled_FontLib_6x8[_c][_col];                      \
                for (int _bit = 0; _bit < 8; _bit++) {                       \
                    if (_fb & (1 << _bit)) {                                  \
                        int _x = (px) + _col;                                \
                        int _y = (py) + _bit;                                \
                        if (_x >= 0 && _x < 128 && _y >= 0 && _y < 64)      \
                            screen[_y][_x >> 3] |= 0x01 << (7 - (_x & 7));  \
                    }                                                          \
                }                                                              \
            }                                                                  \
        }                                                                      \
    } while(0)

    // Helper: draw a string at pixel (px, py) with 6x8 font
    #define DRAW_STR6x8(px, py, str) do {                   \
        const char *_s = (str);                             \
        int _x = (px);                                      \
        while (*_s) { DRAW_CHAR6x8(_x, py, *_s); _x += 6; _s++; } \
    } while(0)

    // --- "STEPS" label (centered: 5 chars * 6px = 30px, center at 64) ---
    DRAW_STR6x8(49, 0, "STEPS");

    // --- Step count (large, using 12x24 font, up to 5 digits) ---
    // Format step count as zero-padded 5-digit string for simplicity
    char steps_str[8];
    snprintf(steps_str, sizeof(steps_str), "%5lu", (unsigned long)Pedometer.step_count);
    // Remove leading spaces – find first non-space
    const char *steps_disp = steps_str;
    while (*steps_disp == ' ' && *(steps_disp + 1) != '\0') steps_disp++;

    // Calculate width to center: each char is 12px wide
    int steps_len = 0;
    const char *tmp = steps_disp;
    while (*tmp++) steps_len++;
    int steps_x = (128 - steps_len * 12) / 2;
    if (steps_x < 0) steps_x = 0;

    // Draw using 12x24 font (3 rows of 8 bits each)
    for (int i = 0; steps_disp[i] != '\0' && i < 6; i++)
    {
        uint8 c = steps_disp[i] - 32;
        if (c >= 96) continue;
        int char_x = steps_x + i * 12;
        // Row 0 of 12x24 character (y = 10..17)
        for (int col = 0; col < 12; col++)
        {
            uint8 fb = Oled_FontLib_12x24[c * 36 + col];
            for (int bit = 0; bit < 8; bit++)
            {
                if (fb & (1 << bit))
                {
                    int x = char_x + col;
                    int y = 10 + bit;
                    if (x >= 0 && x < 128 && y >= 0 && y < 64)
                        screen[y][x >> 3] |= 0x01 << (7 - (x & 7));
                }
            }
        }
        // Row 1 (y = 18..25)
        for (int col = 0; col < 12; col++)
        {
            uint8 fb = Oled_FontLib_12x24[c * 36 + 12 + col];
            for (int bit = 0; bit < 8; bit++)
            {
                if (fb & (1 << bit))
                {
                    int x = char_x + col;
                    int y = 18 + bit;
                    if (x >= 0 && x < 128 && y >= 0 && y < 64)
                        screen[y][x >> 3] |= 0x01 << (7 - (x & 7));
                }
            }
        }
        // Row 2 (y = 26..33)
        for (int col = 0; col < 12; col++)
        {
            uint8 fb = Oled_FontLib_12x24[c * 36 + 24 + col];
            for (int bit = 0; bit < 8; bit++)
            {
                if (fb & (1 << bit))
                {
                    int x = char_x + col;
                    int y = 26 + bit;
                    if (x >= 0 && x < 128 && y >= 0 && y < 64)
                        screen[y][x >> 3] |= 0x01 << (7 - (x & 7));
                }
            }
        }
    }

    // --- Separator line at y=36 ---
    for (int x = 4; x < 124; x++)
        screen[36][x >> 3] |= 0x01 << (7 - (x & 7));

    // --- Distance calculation ---
    // distance_cm = step_count * STEP_STRIDE_CM
    uint32 dist_cm = Pedometer.step_count * STEP_STRIDE_CM;
    uint32 dist_m  = dist_cm / 100;
    uint32 dist_dm = (dist_cm % 100) / 10; // one decimal

    char dist_str[16];
    snprintf(dist_str, sizeof(dist_str), "%lu.%lu m", (unsigned long)dist_m, (unsigned long)dist_dm);

    // "DIST" label at y=39
    DRAW_STR6x8(49, 39, "DIST");

    // Distance value centered at y=49
    int dist_len = 0;
    tmp = dist_str;
    while (*tmp++) dist_len++;
    int dist_x = (128 - dist_len * 6) / 2;
    if (dist_x < 0) dist_x = 0;
    DRAW_STR6x8(dist_x, 49, dist_str);

    // --- "KEY0:RST" hint at bottom y=57 ---
    DRAW_STR6x8(28, 57, "KEY0:RESET");

    #undef DRAW_CHAR6x8
    #undef DRAW_STR6x8

    // Output to OLED
    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
