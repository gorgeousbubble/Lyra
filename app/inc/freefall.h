/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       freefall.h
 * @brief      Free-fall and impact detection using MPU6050 accelerometer
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __FREEFALL_H__
#define __FREEFALL_H__

#include "common.h"

/* -----------------------------------------------------------------------
 * Detection thresholds
 * MPU6050 ±2g range: 16384 LSB/g
 * ----------------------------------------------------------------------- */

/* Free-fall: |a|² < (0.3g)² = (0.3 × 16384)² ≈ 24159236 */
#define FF_FREEFALL_THRESHOLD_SQ   24159236UL   /* (0.3 × 16384)² — MID default */

/* Free-fall must persist for at least this many ms */
#define FF_FREEFALL_MIN_MS         80

/* Impact: |a|² > (2.5g)² = (2.5 × 16384)² ≈ 1677721600 */
#define FF_IMPACT_THRESHOLD_SQ     1677721600UL  /* (2.5 × 16384)² — MID default */

/* -----------------------------------------------------------------------
 * Detection sensitivity (user-configurable preset)
 *
 * Raw |a|² thresholds are meaningless to a user, so the setting exposes three
 * presets instead. Each level maps to a (free-fall, impact) threshold pair:
 *
 *   LOW  : 0.25g / 3.0g  — hardest to trigger (fewest false alarms)
 *   MID  : 0.30g / 2.5g  — factory default
 *   HIGH : 0.40g / 2.0g  — easiest to trigger (most sensitive)
 *
 * A higher free-fall threshold and a lower impact threshold both make the
 * detector fire more readily, so the two move in opposite directions.
 * The live thresholds are held in FF_Freefall_Thresh_Sq / FF_Impact_Thresh_Sq;
 * the level itself is persisted in Flash (Sector 11 / Page 176).
 * ----------------------------------------------------------------------- */
#define FF_SENS_LOW      0
#define FF_SENS_MID      1
#define FF_SENS_HIGH     2
#define FF_SENS_DEFAULT  FF_SENS_MID
#define FF_SENS_MAX      FF_SENS_HIGH

extern uint8  FF_Sensitivity;         /* live level: FF_SENS_LOW/MID/HIGH   */
extern uint32 FF_Freefall_Thresh_Sq;  /* live free-fall threshold (|a|²)    */
extern uint32 FF_Impact_Thresh_Sq;    /* live impact threshold (|a|²)       */

/* Set FF_Sensitivity and derive the two live thresholds from it.
 * Out-of-range levels fall back to FF_SENS_DEFAULT. */
extern void FreeFall_Apply_Sensitivity(uint8 level);

/* Beep duration after confirmed free-fall (ms) */
#define FF_BEEP_DURATION_MS        300

/* Maximum events stored */
#define FF_MAX_EVENTS              8

/* -----------------------------------------------------------------------
 * Event types
 * ----------------------------------------------------------------------- */
typedef enum
{
    FF_EVENT_FREEFALL = 0,   // Free-fall (no impact recorded yet)
    FF_EVENT_IMPACT   = 1,   // Impact after free-fall
} FreeFall_EventType;

/* One logged event */
typedef struct
{
    FreeFall_EventType type;      // Free-fall or impact
    uint32 timestamp_s;           // RTC seconds at event time
    uint32 peak_mag_sq;           // Peak |a|² during impact (LSB² × 1e-6 for display)
    float  peak_g;                // Peak acceleration in g (computed from peak_mag_sq)
    uint32 freefall_duration_ms;  // Duration of free-fall phase (ms)
} FreeFall_Event;

/* Detector state machine */
typedef enum
{
    FF_STATE_IDLE     = 0,   // No event in progress
    FF_STATE_FALLING  = 1,   // Below free-fall threshold
    FF_STATE_IMPACT   = 2,   // Impact window open (waiting for peak)
} FreeFall_State;

typedef struct
{
    FreeFall_State state;

    /* ---- Detection timing ----
     *
     * OLD design (buggy):
     *   uint32 elapsed_ms  — absolute ms counter since power-on, incremented
     *                        +10 each call.  fall_start_ms was snapshotted from
     *                        it, and duration computed as elapsed_ms - fall_start_ms.
     *   BUG: uint32 wraps after ~49.7 days.  At that moment
     *        elapsed_ms - fall_start_ms produces a huge positive value and
     *        the detector fires a phantom free-fall from the IDLE state.
     *
     * NEW design (fixed):
     *   fall_duration_ms  — counts how many consecutive ms we have been below
     *                       the free-fall threshold in the current FALLING phase.
     *                       Reset to 0 whenever we enter FALLING.
     *                       Incremented by the call interval (10 ms) each sample
     *                       while the threshold is still exceeded.
     *                       Never exceeds FF_FREEFALL_MIN_MS + one call interval,
     *                       so no wrap is possible for any realistic threshold.
     *   impact_window_ms  — same approach for the IMPACT phase timeout.
     *
     * Both counters are local relative durations, never absolute timestamps.
     * They cannot wrap because they are reset at the start of each phase and
     * only accumulate for the duration of that phase (tens to hundreds of ms).
     */
    uint32  fall_duration_ms;   // ms spent continuously below FF threshold (FALLING phase)
    uint32  impact_window_ms;   // ms spent in IMPACT phase (reset when entering IMPACT)
    uint32  impact_peak_sq;     // Peak |a|² seen in current impact window

    FreeFall_Event events[FF_MAX_EVENTS]; // Ring buffer of logged events
    int    event_head;          // Next write index
    int    event_count;         // Events stored (0..FF_MAX_EVENTS)

    uint32 total_falls;         // Total confirmed free-falls ever detected
    uint32 total_impacts;       // Total impacts ever detected
    float  max_impact_g;        // Maximum impact force ever seen (g)

    uint32 beep_ms;             // Non-blocking beep countdown (ms remaining)
} FreeFallDetector;

extern FreeFallDetector FFDet;

/*
 * Call every 10ms from main loop.
 * ax, ay, az: raw accelerometer values (±2g, 16384 LSB/g)
 * rtc_seconds: current RTC timestamp (from RTC_Count)
 */
extern void FreeFall_Update(int16 ax, int16 ay, int16 az, uint32 rtc_seconds);

/* Clear all recorded events (KEY0) */
extern void FreeFall_Clear(void);

/* Render the free-fall detector screen to OLED */
extern void Render_FreeFall(void);

#endif
