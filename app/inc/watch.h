/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       watch.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __WATCH_H__
#define __WATCH_H__

#include "common.h"

/*
**Watch clock system enumeration (12-hours, 24-hours)
*/
typedef enum
{
  ClockSystem12, // 12-hours
  ClockSystem24, // 24-hours
} ClockSystem;

/*
**variate declaration
*/
extern const Coord LCM_Clock_Dial_coordinate[];
extern const Coord LCM_Clock_Digit_coordinate[];
extern const Coord LCM_StopWatch_Dial_coordinate[];
extern const Coord LCM_StopWatch_Digit_coordinate[];

extern const int LCM_Clock_Dial_coordinate_length;
extern const int LCM_Clock_Digit_coordinate_length;
extern const int LCM_StopWatch_Dial_coordinate_length;
extern const int LCM_StopWatch_Digit_coordinate_length;

/*
**function declaration
*/
extern void Render_Clock_Current_Time_Dial(const Coord *dial, const int dialLen, int hour, int minute, int second);
extern void Render_Clock_Current_Time_Digit(const Coord *digit, const int digitLen, int hour, int minute);
extern void Render_Stop_Watch_Current_Time_Dial(const Coord *dial, const int dialLen, int minute, int second, int centisecond);
extern void Render_Stop_Watch_Current_Time_Digit(const Coord *digit, const int digitLen, int minute, int second, int centisecond);
extern void Render_Alarm_Clock_List();
extern void Render_Alarm_Clock_Edit(int hour, int minute, int cursor);

#endif