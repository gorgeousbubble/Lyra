/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       tilt_alarm.h
 * @brief      Tilt alarm: buzzer alert when pitch or roll exceeds threshold
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __TILT_ALARM_H__
#define __TILT_ALARM_H__

#include "common.h"

/* Default tilt threshold (degrees). Adjustable via KEY2/KEY3. */
#define TILT_THRESHOLD_DEFAULT  30.0f
#define TILT_THRESHOLD_MIN       5.0f
#define TILT_THRESHOLD_MAX      85.0f
#define TILT_THRESHOLD_STEP      5.0f

/*
 * TiltAlarm state
 */
typedef struct
{
    float  threshold;       // Current alert threshold in degrees
    uint8  enabled;         // 1 = alarm active (monitoring), 0 = paused
    uint8  triggered;       // 1 = currently over threshold
    float  max_pitch;       // Peak |pitch| since last reset
    float  max_roll;        // Peak |roll| since last reset
    uint32 beep_counter;    // ms counter for non-blocking beep pattern
} TiltAlarmState;

extern TiltAlarmState TiltAlarm;

/*
 * Call from main loop every 10ms (after MPU6050 update).
 * Checks pitch/roll against threshold; drives buzzer non-blocking.
 */
extern void TiltAlarm_Update(float pitch, float roll);

/* Toggle monitoring on/off (KEY0) */
extern void TiltAlarm_Toggle(void);

/* Increase threshold by TILT_THRESHOLD_STEP (KEY3) */
extern void TiltAlarm_Threshold_Inc(void);

/* Decrease threshold by TILT_THRESHOLD_STEP (KEY2) */
extern void TiltAlarm_Threshold_Dec(void);

/* Reset peak values */
extern void TiltAlarm_Reset_Peak(void);

/* Render the tilt alarm screen to OLED */
extern void Render_TiltAlarm(float pitch, float roll);

#endif
