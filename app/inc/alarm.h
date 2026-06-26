/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       alarm.h
 * @brief      Alarm clock trigger detection and ring control
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 *
 * Responsibilities of this module:
 *   - Compare current RTC time against every entry in Alarm_Clock_List
 *   - When a match is found, drive the buzzer in a non-blocking pattern
 *   - Allow the user to silence the alarm (snooze/dismiss)
 *
 * NOT handled here (already in watch.c / maps_dock_key.c):
 *   - Editing, adding, deleting alarms (UI layer)
 *   - Persisting alarms to W25Q80 Flash
 */

#ifndef __ALARM_H__
#define __ALARM_H__

#include "common.h"

/* -----------------------------------------------------------------------
 * Ring pattern
 *
 * When an alarm fires the buzzer plays a repeating on/off pattern:
 *   ALARM_BEEP_ON_MS  ms on  → ALARM_BEEP_OFF_MS ms off → repeat
 *
 * The ring continues for up to ALARM_RING_DURATION_MS total milliseconds
 * before automatically silencing (auto-dismiss).  The user can also
 * dismiss early by pressing any key (handled in maps_dock_key.c via
 * Alarm_Dismiss()).
 * ----------------------------------------------------------------------- */
#define ALARM_BEEP_ON_MS        500     /* buzzer on per pulse  (ms) */
#define ALARM_BEEP_OFF_MS       500     /* buzzer off per pulse (ms) */
#define ALARM_RING_PERIOD       (ALARM_BEEP_ON_MS + ALARM_BEEP_OFF_MS)
#define ALARM_RING_DURATION_MS  60000   /* auto-dismiss after 60 s   */

/* -----------------------------------------------------------------------
 * State
 * ----------------------------------------------------------------------- */
typedef struct
{
    uint8  ringing;         /* 1 = alarm is currently sounding           */
    int    alarm_index;     /* which alarm in Alarm_Clock_List fired      */
    uint32 ring_elapsed_ms; /* ms elapsed since alarm started ringing     */
    uint32 beep_phase_ms;   /* ms within the current on/off cycle         */
    int    last_checked_minute; /* last RTC minute we ran the check on    */
} AlarmState;

extern AlarmState Alarm;

/* -----------------------------------------------------------------------
 * API
 * ----------------------------------------------------------------------- */

/*
 * Alarm_Tick
 * ----------
 * Call from the main loop every 10 ms (inside the MPU6050_Read_Flag block
 * or any other 10 ms periodic slot).
 *
 * rtc_hour, rtc_minute: current time from RTC_Time_Now (pass snapshotted
 *   values so this function never reads the volatile struct directly).
 *
 * Behaviour:
 *   - If not already ringing: scan Alarm_Clock_List; if current
 *     hour:minute matches any entry start ringing.
 *   - If ringing: advance the non-blocking beep pattern and auto-dismiss
 *     after ALARM_RING_DURATION_MS.
 */
extern void Alarm_Tick(int rtc_hour, int rtc_minute);

/*
 * Alarm_Dismiss
 * -------------
 * Silence the currently ringing alarm immediately.
 * Call from maps_dock_key.c when any key is pressed while ringing.
 * Safe to call even when no alarm is ringing (no-op).
 */
extern void Alarm_Dismiss(void);

/*
 * Alarm_Is_Ringing
 * ----------------
 * Returns 1 if an alarm is currently sounding, 0 otherwise.
 * Used by the UI to overlay a "ALARM!" banner on the display.
 */
extern int Alarm_Is_Ringing(void);

#endif /* __ALARM_H__ */
