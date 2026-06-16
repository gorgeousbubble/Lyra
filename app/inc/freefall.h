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
#define FF_FREEFALL_THRESHOLD_SQ   24159236UL   /* (0.3 × 16384)² */

/* Free-fall must persist for at least this many ms */
#define FF_FREEFALL_MIN_MS         80

/* Impact: |a|² > (2.5g)² = (2.5 × 16384)² ≈ 1677721600 */
#define FF_IMPACT_THRESHOLD_SQ     1677721600UL  /* (2.5 × 16384)² */

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
    uint32  fall_start_ms;      // When free-fall threshold was first crossed
    uint32  fall_duration_ms;   // Duration of confirmed free-fall
    uint32  impact_window_ms;   // ms since impact window opened
    uint32  impact_peak_sq;     // Peak |a|² seen in current impact window

    FreeFall_Event events[FF_MAX_EVENTS]; // Ring buffer of events
    int    event_head;          // Next write index
    int    event_count;         // Events stored (0..FF_MAX_EVENTS)

    uint32 total_falls;         // Total free-falls ever detected
    uint32 total_impacts;       // Total impacts ever detected
    float  max_impact_g;        // Maximum impact force ever seen (g)

    uint32 elapsed_ms;          // Internal ms counter
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
