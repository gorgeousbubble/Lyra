/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       alarm.c
 * @brief      Alarm clock trigger detection and ring control
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "alarm.h"
#include "beep.h"
#include "watch.h"   /* Alarm_Clock_List, Alarm_Clock_List_Len */

/* -----------------------------------------------------------------------
 * Global state
 * ----------------------------------------------------------------------- */
AlarmState Alarm = {
    0,   /* ringing            */
    -1,  /* alarm_index        */
    0,   /* ring_elapsed_ms    */
    0,   /* beep_phase_ms      */
    -1   /* last_checked_minute */
};

/* -----------------------------------------------------------------------
 * Internal helpers
 * ----------------------------------------------------------------------- */

/* Start ringing for alarm at list index `idx`. */
static void start_ring(int idx)
{
    Alarm.ringing          = 1;
    Alarm.alarm_index      = idx;
    Alarm.ring_elapsed_ms  = 0;
    Alarm.beep_phase_ms    = 0;
    Beep_On();
}

/* Stop ringing immediately. */
static void stop_ring(void)
{
    Alarm.ringing         = 0;
    Alarm.alarm_index     = -1;
    Alarm.ring_elapsed_ms = 0;
    Alarm.beep_phase_ms   = 0;
    Beep_Off();
}

/* -----------------------------------------------------------------------
 * Alarm_Tick
 *
 * Called every 10 ms.
 *
 * Design notes:
 *
 * 1. MATCH CONDITION
 *    An alarm fires when rtc_hour == alarm.hour AND rtc_minute == alarm.minute.
 *    We only check once per minute (last_checked_minute guard) so the alarm
 *    fires exactly once per matching minute even if Alarm_Tick is called 6000
 *    times during that minute.
 *
 * 2. NON-BLOCKING BEEP PATTERN
 *    beep_phase_ms counts within the current ALARM_RING_PERIOD cycle.
 *    If beep_phase_ms < ALARM_BEEP_ON_MS  → buzzer on
 *    Otherwise                            → buzzer off
 *    The counter wraps at ALARM_RING_PERIOD so the pattern repeats.
 *
 * 3. AUTO-DISMISS
 *    After ALARM_RING_DURATION_MS total ms the alarm silences itself so
 *    the buzzer does not run indefinitely if the user ignores it.
 *
 * 4. MULTIPLE ALARMS
 *    Scans the entire Alarm_Clock_List on each minute boundary.
 *    If two alarms share the same hour:minute they both match; the first
 *    one in the list wins (subsequent ones are skipped while ringing).
 *    This is acceptable for a wristwatch use case.
 * ----------------------------------------------------------------------- */
void Alarm_Tick(int rtc_hour, int rtc_minute)
{
    /* ---- If currently ringing, advance the pattern ---- */
    if (Alarm.ringing)
    {
        Alarm.ring_elapsed_ms += 10;
        Alarm.beep_phase_ms   += 10;

        /* Wrap the phase counter at the end of each on/off period */
        if (Alarm.beep_phase_ms >= ALARM_RING_PERIOD)
            Alarm.beep_phase_ms = 0;

        /* Drive buzzer based on phase */
        if (Alarm.beep_phase_ms < ALARM_BEEP_ON_MS)
            Beep_On();
        else
            Beep_Off();

        /* Auto-dismiss after maximum ring duration */
        if (Alarm.ring_elapsed_ms >= ALARM_RING_DURATION_MS)
            stop_ring();

        return; /* Do not start a second alarm while one is already ringing */
    }

    /* ---- Check for a new alarm match once per minute ---- */
    if (rtc_minute == Alarm.last_checked_minute)
        return; /* Already checked this minute */

    Alarm.last_checked_minute = rtc_minute;

    int i;
    for (i = 0; i < Alarm_Clock_List_Len; i++)
    {
        if (Alarm_Clock_List[i].hour   == rtc_hour &&
            Alarm_Clock_List[i].minute == rtc_minute)
        {
            start_ring(i);
            return; /* First match wins; others in the same minute are ignored */
        }
    }
}

/* -----------------------------------------------------------------------
 * Alarm_Dismiss
 * ----------------------------------------------------------------------- */
void Alarm_Dismiss(void)
{
    if (Alarm.ringing)
        stop_ring();
}

/* -----------------------------------------------------------------------
 * Alarm_Is_Ringing
 * ----------------------------------------------------------------------- */
int Alarm_Is_Ringing(void)
{
    return (int)Alarm.ringing;
}
