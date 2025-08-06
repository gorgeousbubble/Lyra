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
typedef enum {
  ClockSystem12,//12-hours
  ClockSystem24,//24-hours
}ClockSystem;

/*
**variate declaration
*/
extern const Coord LCM_Clock_Dial_coordinate[];
extern const Coord LCM_Clock_Digit_coordinate[];

extern const int LCM_Clock_Dial_coordinate_length;
extern const int LCM_Clock_Digit_coordinate_length;

/*
**function declaration
*/
extern void Watch_Render_Current_Time_6x8(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second, ClockSystem clock_system);
extern void Watch_Render_Current_Time_Wo_Sec_6x8(uint8 x,uint8 y,uint8 ch[],int hour,int minute,ClockSystem clock_system);
extern void Watch_Render_Current_Time_8x16(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second, ClockSystem clock_system);
extern void Watch_Render_Current_Time_Wo_Sec_8x16(uint8 x,uint8 y,uint8 ch[],int hour,int minute,ClockSystem clock_system);
extern void Watch_Render_Current_Time_12x24(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second, ClockSystem clock_system);
extern void Watch_Render_Current_Time_Wo_Sec_12x24(uint8 x,uint8 y,uint8 ch[],int hour,int minute, ClockSystem clock_system);

extern void Render_Clock_Current_Time_Dial(const Coord *dial, const int dialLen, int hour,int minute,int second);
extern void Render_Clock_Current_Time_Digit(const Coord *digit, const int digitLen, int hour,int minute);


#endif