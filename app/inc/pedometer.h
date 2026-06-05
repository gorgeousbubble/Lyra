/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       pedometer.h
 * @brief      Step counter (pedometer) using MPU6050 accelerometer
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __PEDOMETER_H__
#define __PEDOMETER_H__

#include "common.h"

/*
 * Algorithm: threshold-crossing peak detection on acceleration magnitude.
 *
 * The composite acceleration magnitude is:
 *   mag = sqrt(ax^2 + ay^2 + az^2)
 *
 * A step is counted when:
 *   1. mag crosses STEP_THRESHOLD upward (rising edge)
 *   2. At least STEP_MIN_INTERVAL_MS has elapsed since the last step
 *      (debounce: rejects vibration noise / multiple peaks per stride)
 *
 * Tuning:
 *   STEP_THRESHOLD      – raise to reject weak movements (e.g. arm swing while seated)
 *   STEP_MIN_INTERVAL_MS – lower for fast running, raise for slow walk
 */

// Accelerometer full-scale = ±2g → 16384 LSB/g
// A walking step peak is typically 1.3~2.0g total; we set the threshold at ~1.2g
#define STEP_THRESHOLD        19660   // ~1.2g in 16384 LSB/g units (sqrt threshold applied to sum-of-squares)
#define STEP_THRESHOLD_SQ     (19660UL * 19660UL)  // Pre-squared for integer math
#define STEP_MIN_INTERVAL_MS  300     // Minimum ms between two steps (max ~200 steps/min)

// Average stride length (cm) – tunable
#define STEP_STRIDE_CM        65

// Maximum steps before display overflows (99999)
#define STEP_MAX_COUNT        99999

/*
 * Pedometer state (all fields managed internally by Pedometer_Update)
 */
typedef struct
{
    uint32 step_count;        // Total steps since last reset
    uint32 last_step_ms;      // Timestamp of last detected step (ms, from PIT0_Count accumulator)
    uint8  above_threshold;   // 1 = magnitude is currently above threshold (peak state)
} PedometerState;

extern PedometerState Pedometer;

/*
 * Call from main loop whenever MPU6050_Read_Flag fires (every 10ms).
 * ax, ay, az: raw accelerometer values in LSB (16384 = 1g for ±2g range).
 * elapsed_ms: milliseconds since last call (normally 10).
 */
extern void Pedometer_Update(int16 ax, int16 ay, int16 az, uint32 elapsed_ms);

/* Reset step count and distance to zero */
extern void Pedometer_Reset(void);

/* Render the pedometer screen to OLED */
extern void Render_Pedometer(void);

#endif
