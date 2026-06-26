/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       sleep_monitor.h
 * @brief      Sleep monitoring using MPU6050 accelerometer activity analysis
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __SLEEP_MONITOR_H__
#define __SLEEP_MONITOR_H__

#include "common.h"

/* -----------------------------------------------------------------------
 * Algorithm parameters
 * MPU6050 ±2g range, 16384 LSB/g
 *
 * Sleep state is determined by a 30-second sliding window RMS of
 * |a| − 1g  (removing gravity to get body-motion acceleration).
 * |a| = sqrt(ax²+ay²+az²); subtract 1g = 16384 LSB.
 * ----------------------------------------------------------------------- */

/* Thresholds for |a|-1g RMS (in LSB units) */
#define SLEEP_AWAKE_THRESHOLD    1800   /* RMS > this → AWAKE           */
#define SLEEP_LIGHT_THRESHOLD     400   /* RMS > this → LIGHT SLEEP     */
/* RMS ≤ SLEEP_LIGHT_THRESHOLD → DEEP SLEEP                             */

/* Window size for RMS calculation: 30 s × 100 Hz = 3000 samples,
   but we accumulate sum-of-squares at 100 Hz and evaluate every 30 s */
#define SLEEP_WINDOW_SEC         30
#define SLEEP_WINDOW_SAMPLES     3000   /* 30 s × 100 Hz */

/* History slots: record one slot every 30 seconds.
   8 hours × 120 slots/hour = 960 slots.
   We store 128 slots (about 1 hour) for OLED bar display. */
#define SLEEP_HIST_SLOTS         128

/* Sleep states */
typedef enum
{
    SLEEP_STATE_AWAKE = 0,
    SLEEP_STATE_LIGHT = 1,
    SLEEP_STATE_DEEP  = 2,
} SleepState;

/* One 30-second slot */
typedef struct
{
    SleepState state;
    uint32     timestamp_s; /* RTC seconds at the end of this window */
} SleepSlot;

typedef struct
{
    /* RMS accumulator */
    uint64 sum_sq;           /* Running sum of (|a|-1g)² for current window */
    uint32 sample_count;     /* Samples accumulated in current window        */

    /* Current state */
    SleepState current_state;

    /* Sleep session statistics */
    uint32 session_start_s;  /* RTC seconds when monitoring started / last woke */
    uint32 total_sleep_s;    /* Total seconds in LIGHT+DEEP states this session */
    uint32 total_deep_s;     /* Total seconds in DEEP state                     */
    uint32 turn_count;       /* Number of AWAKE interruptions during sleep      */
    uint8  was_sleeping;     /* 1 if previous state was LIGHT or DEEP           */

    /* History ring buffer (newest at head-1) */
    SleepSlot history[SLEEP_HIST_SLOTS];
    int       hist_head;     /* Next write index                               */
    int       hist_count;    /* Entries filled (0..SLEEP_HIST_SLOTS)           */

    /* Control */
    uint8  monitoring;       /* 1 = monitoring active (KEY0 toggles)           */
} SleepMonitorState;

extern SleepMonitorState SleepMon;

/*
 * Call every 10ms from main loop (same MPU6050 block).
 * ax, ay, az: raw accelerometer values (±2g, 16384 LSB/g).
 * rtc_seconds: current RTC timestamp.
 */
extern void SleepMonitor_Update(int16 ax, int16 ay, int16 az, uint32 rtc_seconds);

/* Toggle monitoring on/off (KEY0).
 * rtc_now: current RTC_Count — recorded as session_start_s when turning ON. */
extern void SleepMonitor_Toggle(uint32 rtc_now);

/* Clear session statistics (KEY2).
 * rtc_now: current RTC_Count — recorded as new session_start_s. */
extern void SleepMonitor_Clear(uint32 rtc_now);

/* Render the sleep monitor screen to OLED */
extern void Render_SleepMonitor(void);

#endif
