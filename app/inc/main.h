/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       main.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#include "common.h"

/*
**OLED state enumeration
*/
typedef enum
{
  MAPS_OLED_Off = 0, // OLED off
  MAPS_OLED_On = 1,  // OLED on
} MAPS_OLED_Status;

/*
**Screen status enumeration
*/
typedef enum
{
  MAPS_Screen_Saver = 0,      // Screen saver
  MAPS_Screen_Menu = 1,       // Menu screen
  MAPS_Screen_Normal = 2,     // Normal screen
  MAPS_Screen_Status_Max = 3, // Maximum screen status
} MAPS_Screen_Status;

typedef enum
{
  MAPS_Menu_Clock = 0,            // Menu Clock
  MAPS_Menu_StopWatch = 1,        // Menu Stop Watch
  MAPS_Menu_AlarmClock = 2,       // Menu Alarm Clock
  MAPS_Menu_WorldClock = 3,       // Menu World Clock
  MAPS_Menu_SpiritLevel = 4,      // Menu Spirit Level
  MAPS_Menu_Configure_Adjust = 5, // Menu Configure Adjust
  MAPS_Menu_Selection_Max = 6,    // Maximum menu selection
} MAPS_Menu_Selection;

typedef enum
{
  MAPS_Clock_Dial = 0,      // Clock Dial
  MAPS_Clock_Digit = 1,     // Clock Digit
  MAPS_Clock_Style_Max = 2, // Maximum clock style
} MAPS_Clock_Style;

typedef enum
{
  MAPS_StopWatch_Dial = 0,      // Stop Watch Dial
  MAPS_StopWatch_Digit = 1,     // Stop Watch Digital
  MAPS_StopWatch_Style_Max = 2, // Maximum Stop Watch style
} MAPS_StopWatch_Style;

typedef enum
{
  MAPS_AlarmClock_List = 0,     // Alarm Clock List
  MAPS_AlarmClock_Edit = 1,     // Alarm Clock Edit
  MAPS_AlarmClock_Mode_Max = 2, // Maximum Alarm Clock mode
} MAPS_AlarmClock_Mode;

typedef enum
{
  MAPS_AlarmClock_Timer_0 = 0,   // Alarm Clock Timer 0
  MAPS_AlarmClock_Timer_1 = 1,   // Alarm Clock Timer 1
  MAPS_AlarmClock_Timer_2 = 2,   // Alarm Clock Timer 2
  MAPS_AlarmClock_Timer_3 = 3,   // Alarm Clock Timer 3
  MAPS_AlarmClock_Timer_4 = 4,   // Alarm Clock timer 4
  MAPS_AlarmClock_Timer_5 = 5,   // Alarm Clock Timer 5
  MAPS_AlarmClock_Timer_6 = 6,   // Alarm Clock Timer 6
  MAPS_AlarmClock_Timer_7 = 7,   // Alarm Clock timer 7
  MAPS_AlarmClock_Timer_Max = 8, // Maximum Alarm Clock timer
} MAPS_AlarmClock_Timer;

typedef enum
{
  MAPS_WorldClock_Timezone_Beijing = 0,      // Beijing Timezone (+8)
  MAPS_WorldClock_Timezone_Shanghai = 1,     // Shanghai Timezone (+8)
  MAPS_WorldClock_Timezone_Hongkong = 2,     // Hong Kong Timezone (+8)
  MAPS_WorldClock_Timezone_Taipei = 3,       // Taipei Timezone (+8)
  MAPS_WorldClock_Timezone_Singapore = 4,    // Singapore Timezone (+8)
  MAPS_WorldClock_Timezone_Seoul = 5,        // Seoul Timezone (+9)
  MAPS_WorldClock_Timezone_Tokyo = 6,        // Tokyo Timezone (+9)
  MAPS_WorldClock_Timezone_Sydney = 7,       // Sydney Timezone (+10)
  MAPS_WorldClock_Timezone_SanFrancisco = 8, // San Francisco Timezone (-8)
  MAPS_WorldClock_Timezone_NewYork = 9,      // New York Timezone (-5)
  MAPS_WorldClock_Timezone_Max = 10,         // Maximum World Clock timezone
} MAPS_WorldClock_Timezone;

typedef struct MAPS_WorldClock_Time
{
  MAPS_WorldClock_Timezone timezone; // World Clock timezone
  int hour_offset;                   // Hour offset from UTC
  int minute_offset;                 // Minute offset from UTC
} MAPS_WorldClock_Time;

typedef enum
{
  MAPS_ConfigureAdjust_List = 0,  // Configure Adjust List
  MAPS_ConfigureAdjust_Clock = 1, // Configure Adjust RTC Clock
  MAPS_ConfigureAdjust_Date = 2,  // Configure Adjust RTC Date
  MAPS_ConfigureAdjust_Tense = 3, // Configure Adjust Time Tense
  MAPS_ConfigureAdjust_Max = 4,   // Maximum Configure Adjust mode
} MAPS_ConfigureAdjust_Mode;

typedef enum
{
  MAPS_ConfigureAdjust_Tense_24H = 64, // 24-Hour Format
  MAPS_ConfigureAdjust_Tense_12H = 65, // 12-Hour Format
} MAPS_ConfigureAdjust_Tense_Format;

/*
**variate declaration
*/
extern uint16 LPTMR_Count;
extern MAPS_Screen_Status MAPS_Screen_StatusN[MAPS_Screen_Status_Max];
extern MAPS_Menu_Selection MAPS_Menu_SelectionN[MAPS_Menu_Selection_Max];
extern MAPS_Clock_Style MAPS_Clock_StyleN[MAPS_Clock_Style_Max];
extern MAPS_AlarmClock_Mode MAPS_AlarmClock_ModeN[MAPS_AlarmClock_Mode_Max];
extern MAPS_WorldClock_Timezone MAPS_WorldClock_TimezoneN[MAPS_WorldClock_Timezone_Max];
extern MAPS_WorldClock_Time MAPS_WorldClock_Timezone_Array[MAPS_WorldClock_Timezone_Max];
extern MAPS_ConfigureAdjust_Mode MAPS_ConfigureAdjust_ModeN[MAPS_ConfigureAdjust_Max];

/*
**function declaration
*/
extern int main(void);

#endif