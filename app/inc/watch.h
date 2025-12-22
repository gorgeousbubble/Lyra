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

extern const int LCM_Clock_Dial_coordinate_length;
extern const int LCM_Clock_Digit_coordinate_length;
extern const int LCM_StopWatch_Dial_coordinate_length;
extern const int LCM_StopWatch_Digit_coordinate_length;
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

extern Coord *LCM_Clock_Dial_Buf;// Clock dial buffer
extern Coord *LCM_Clock_Digit_Buf;// Clock digit buffer
extern Coord *LCM_StopWatch_Dial_Buf;// Stopwatch dial buffer
extern Coord *LCM_StopWatch_Digit_Buf;// Stopwatch digit buffer
extern Coord *LCM_WorldClock_shanghai_Buf;// World clock Shanghai buffer
extern Coord *LCM_WorldClock_beijing_Buf;// World clock Beijing buffer
extern Coord *LCM_WorldClock_hongkong_Buf;// World clock Hong Kong
extern Coord *LCM_WorldClock_taipei_Buf;// World clock Taipei buffer
extern Coord *LCM_WorldClock_seoul_Buf;// World clock Seoul buffer
extern Coord *LCM_WorldClock_tokyo_Buf;// World clock Tokyo buffer
extern Coord *LCM_WorldClock_sydney_Buf;// World clock Sydney buffer
extern Coord *LCM_WorldClock_singapore_Buf;// World clock Singapore buffer
extern Coord *LCM_WorldClock_sanfrancisco_Buf;// World clock San Francisco buffer
extern Coord *LCM_WorldClock_newyork_Buf;// World clock New York buffer

extern int LCM_Clock_Dial_Buf_Size;// Clock dial buffer size
extern int LCM_Clock_Digit_Buf_Size;// Clock digit buffer size
extern int LCM_StopWatch_Dial_Buf_Size;// Stopwatch dial buffer size
extern int LCM_StopWatch_Digit_Buf_Size;// Stopwatch digit buffer size
extern int LCM_WorldClock_shanghai_Buf_Size;// World clock Shanghai buffer size
extern int LCM_WorldClock_beijing_Buf_Size;// World clock Beijing buffer
extern int LCM_WorldClock_hongkong_Buf_Size;// World clock Hong Kong buffer
extern int LCM_WorldClock_taipei_Buf_Size;// World clock Taipei buffer size
extern int LCM_WorldClock_seoul_Buf_Size;// World clock Seoul buffer size
extern int LCM_WorldClock_tokyo_Buf_Size;// World clock Tokyo buffer size
extern int LCM_WorldClock_sydney_Buf_Size;// World clock Sydney buffer size
extern int LCM_WorldClock_singapore_Buf_Size;// World clock Singapore
extern int LCM_WorldClock_sanfrancisco_Buf_Size;// World clock San Francisco buffer size
extern int LCM_WorldClock_newyork_Buf_Size;// World clock New York buffer size

extern uint8 Alarm_Clock_Array[16];
extern Alarm_Clock_Time* Alarm_Clock_List;
extern int Alarm_Clock_Max_Len;

/*
**function declaration
*/
extern void Render_Clock_Current_Time_Dial(const Coord *dial, const int dialLen, Coord** buff, int* buffSize, int hour, int minute, int second);
extern void Render_Clock_Current_Time_Digit(const Coord *digit, const int digitLen, Coord** buff, int* buffSize, int hour, int minute);
extern void Render_Stop_Watch_Current_Time_Dial(const Coord *dial, const int dialLen, Coord** buff, int* buffSize, int minute, int second, int centisecond);
extern void Render_Stop_Watch_Current_Time_Digit(const Coord *digit, const int digitLen, Coord** buff, int* buffSize, int minute, int second, int centisecond);
extern void Render_Alarm_Clock_List(int cursor);
extern void Render_Alarm_Clock_Edit(int hour, int minute, int cursor, int number);
extern void Render_World_Clock_Time(const Coord *city, const int cityLen, Coord** buff, int* buffSize, MAPS_WorldClock_Time time, int hour, int minute);

extern void Add_Alarm_Clock_Time_To_List(int hour, int minute);
extern void Mod_Alarm_Clock_Time_To_List(int index, int hour, int minute);
extern void Del_Alarm_Clock_Time_From_List(int index);
extern void Get_Alarm_Clock_Time_From_List(int index, int* hour, int* minute);
extern int Get_Alarm_Clock_List_Len();
extern void Clean_Alarm_Clock_List();
extern void Write_Alarm_Clock_List_To_E2PROM();
extern void Read_Alarm_Clock_E2PROM_To_List();

#endif