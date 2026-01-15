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
**structure definition
*/
// alarm clock time
struct Alarm_Clock_Time
{
  int hour;
  int minute;
  struct Alarm_Clock_Time* next;
};

typedef struct Alarm_Clock_Time Alarm_Clock_Time;

/*
**variate declaration
*/
extern const Coord LCM_Clock_Dial_coordinate[];
extern const Coord LCM_Clock_Digit_coordinate[];
extern const Coord LCM_StopWatch_Dial_coordinate[];
extern const Coord LCM_StopWatch_Digit_coordinate[];
extern const Coord LCM_AlarmClock_Digit_coordinate[];
extern const Coord LCM_WorldClock_shanghai_coordinate[];
extern const Coord LCM_WorldClock_beijing_coordinate[];
extern const Coord LCM_WorldClock_hongkong_coordinate[];
extern const Coord LCM_WorldClock_taipei_coordinate[];
extern const Coord LCM_WorldClock_seoul_coordinate[];
extern const Coord LCM_WorldClock_tokyo_coordinate[];
extern const Coord LCM_WorldClock_sydney_coordinate[];
extern const Coord LCM_WorldClock_singapore_coordinate[];
extern const Coord LCM_WorldClock_sanfrancisco_coordinate[];
extern const Coord LCM_WorldClock_newyork_coordinate[];
extern const Coord LCM_ConfigureAdjust_Clock_icon_coordinate[];
extern const Coord LCM_ConfigureAdjust_Clock_Digit_coordinate[];
extern const Coord LCM_ConfigureAdjust_Date_icon_coordinate[];
extern const Coord LCM_ConfigureAdjust_Date_Digit_coordinate[];

extern const int LCM_Clock_Dial_coordinate_length;
extern const int LCM_Clock_Digit_coordinate_length;
extern const int LCM_StopWatch_Dial_coordinate_length;
extern const int LCM_StopWatch_Digit_coordinate_length;
extern const int LCM_AlarmClock_Digit_coordinate_length;
extern const int LCM_WorldClock_shanghai_coordinate_length;
extern const int LCM_WorldClock_beijing_coordinate_length;
extern const int LCM_WorldClock_hongkong_coordinate_length;
extern const int LCM_WorldClock_taipei_coordinate_length;
extern const int LCM_WorldClock_seoul_coordinate_length;
extern const int LCM_WorldClock_tokyo_coordinate_length;
extern const int LCM_WorldClock_sydney_coordinate_length;
extern const int LCM_WorldClock_singapore_coordinate_length;
extern const int LCM_WorldClock_sanfrancisco_coordinate_length;
extern const int LCM_WorldClock_newyork_coordinate_length;
extern const int LCM_ConfigureAdjust_Clock_icon_coordinate_length;
extern const int LCM_ConfigureAdjust_Clock_Digit_coordinate_length;
extern const int LCM_ConfigureAdjust_Date_icon_coordinate_length;
extern const int LCM_ConfigureAdjust_Date_Digit_coordinate_length;

extern uint8 Alarm_Clock_Array[16];
extern Alarm_Clock_Time* Alarm_Clock_List;
extern int Alarm_Clock_Max_Len;

/*
**function declaration
*/
extern void Calc_Clock_Current_Time_Dial(uint8* array, const Coord *dial, const int dialLen, int hour,int minute,int second);
extern void Calc_Clock_Current_Time_Digit(uint8* array, const Coord *digit, const int digitLen, int hour, int minute);
extern void Calc_Stop_Watch_Current_Time_Dial(uint8* array, const Coord *dial, const int dialLen,  int minute,int second, int centisecond);
extern void Calc_Stop_Watch_Current_Time_Digit(uint8* array, const Coord *digit, const int digitLen,  int minute,int second, int centisecond);
extern void Calc_Alarm_Clock_List_Mode_Time_Digit(uint8* array, const Coord *digit, const int digitLen, int hour, int minute, int cursor);
extern void Calc_Alarm_Clock_Edit_Mode_Time_Digit(uint8* array, const Coord *digit, const int digitLen, int hour, int minute, int cursor, int number);
extern void Calc_World_Clock_Time(uint8* array, const Coord *city, const int cityLen, MAPS_WorldClock_Time time, int hour, int minute);
extern void Calc_Configure_Adjust_List_Mode_Item(uint8* array, const Coord *item, const int itemLen);
extern void Calc_Configure_Adjust_Clock_Digit(uint8* array, const Coord *digit, const int digitLen, int hour, int minute, int second, int number);
extern void Calc_Configure_Adjust_Date_Digit(uint8* array, const Coord *digit, const int digitLen, int year, int month, int day, int number);
extern void Render_Clock_Current_Time_Dial(const Coord *dial, const int dialLen, int hour, int minute, int second);
extern void Render_Clock_Current_Time_Digit(const Coord *digit, const int digitLen, int hour, int minute);
extern void Render_Stop_Watch_Current_Time_Dial(const Coord *dial, const int dialLen, int minute, int second, int centisecond);
extern void Render_Stop_Watch_Current_Time_Digit(const Coord *digit, const int digitLen, int minute, int second, int centisecond);
extern void Render_Alarm_Clock_List_Mode_Time_Digit(const Coord *digit, const int digitLen, int hour, int minute, int cursor);
extern void Render_Alarm_Clock_Edit_Mode_Time_Digit(const Coord *digit, const int digitLen, int hour, int minute, int cursor, int number);
extern void Render_World_Clock_Time(const Coord *city, const int cityLen, MAPS_WorldClock_Time time, int hour, int minute);
extern void Render_Configure_Adjust_List_Mode_Item(const Coord *item, const int itemLen);
extern void Render_Configure_Adjust_Clock_Digit(const Coord *digit, const int digitLen, int hour, int minute, int second, int number);
extern void Render_Configure_Adjust_Date_Digit(const Coord *digit, const int digitLen, int year, int month, int day, int number);

extern void Add_Alarm_Clock_Time_To_List(int hour, int minute);
extern void Mod_Alarm_Clock_Time_To_List(int index, int hour, int minute);
extern void Del_Alarm_Clock_Time_From_List(int index);
extern void Get_Alarm_Clock_Time_From_List(int index, int* hour, int* minute);
extern int Get_Alarm_Clock_List_Len();
extern void Clean_Alarm_Clock_List();
extern void Write_Alarm_Clock_List_To_E2PROM();
extern void Read_Alarm_Clock_E2PROM_To_List();

#endif