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
// static rest:  az ≈ 16384  → mag_sq ≈ 16384² = 268,435,456
// 1.2g motion:  mag ≈ 19660 → mag_sq ≈ 386,516,0 (above rest, triggered by step)
#define STEP_THRESHOLD        19660UL   // ~1.2g in LSB — initial/fallback threshold
#define STEP_THRESHOLD_SQ     (19660ULL * 19660ULL)  // Pre-squared (legacy)
#define STEP_MIN_INTERVAL_MS  300     // Minimum ms between two steps (max ~200 steps/min)

/* -----------------------------------------------------------------------
 * Adaptive threshold parameters
 *
 * A single fixed threshold cannot fit every user: a wrist-worn device sees
 * very different acceleration amplitudes depending on gait, arm swing and
 * how tightly the strap is worn.  Instead of a constant, the detector tracks
 * the recent min/max envelope of the acceleration magnitude and places the
 * step-detection threshold at the midpoint of that envelope, refreshed once
 * per ADAPT_WINDOW samples.
 *
 * An amplitude gate (STEP_MIN_AMPLITUDE) suppresses counting when the
 * peak-to-peak magnitude is too small to be real walking (rest, typing,
 * small hand tremor) — this replaces the old fixed 1.2g gate.
 * ----------------------------------------------------------------------- */
#define STEP_ADAPT_WINDOW     50      // Samples per envelope window (0.5s @ 100Hz)
#define STEP_MIN_AMPLITUDE    4000    // Min peak-to-peak magnitude (LSB, ~0.24g) to count
#define STEP_THRESHOLD_INIT   17000   // Initial magnitude threshold before first adaptation

// Average stride length (cm).
// STEP_STRIDE_CM is the factory default. The live value is held in the runtime
// variable Pedometer_Stride_Cm, which is user-configurable through the
// Configure-Adjust menu and persisted in Flash (Sector 4 / Page 64).
// Distance = step_count * Pedometer_Stride_Cm.
#define STEP_STRIDE_CM        65      /* factory default (cm) */
#define STRIDE_CM_MIN         30      /* configurable lower bound */
#define STRIDE_CM_MAX         120     /* configurable upper bound */
#define STRIDE_CM_STEP        1       /* KEY2/KEY3 increment (cm) */

extern uint32 Pedometer_Stride_Cm;    /* live, persisted stride length (cm) */

// Maximum steps before display overflows (99999)
#define STEP_MAX_COUNT        99999

/*
 * Pedometer state (all fields managed internally by Pedometer_Update)
 */
typedef struct
{
    uint32 step_count;        // Total steps since last reset
    uint32 last_step_ms;      // Timestamp of last detected step (ms accumulator)
    uint8  above_threshold;   // 1 = magnitude is currently above threshold (peak state)

    /* Adaptive threshold state */
    uint32 dyn_min;           // Running minimum magnitude in current window
    uint32 dyn_max;           // Running maximum magnitude in current window
    uint32 threshold;         // Current step-detection threshold (magnitude units)
    uint16 win_count;         // Samples accumulated in current envelope window
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

/* Force the next Render_Pedometer() to redraw (call on screen entry so the
 * cached display can't show stale content from the previous screen). */
extern void Pedometer_Invalidate_Display(void);

#endif
