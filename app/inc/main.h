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
  MAPS_OLED_Off = 0, //OLED off
  MAPS_OLED_On = 1,  //OLED on
}MAPS_OLED_Status;

/*
**Screen status enumeration
*/
typedef enum
{
  MAPS_Screen_Saver = 0,      //Screen saver
  MAPS_Screen_Menu = 1,       //Menu screen
  MAPS_Screen_Normal = 2,     //Normal screen
} MAPS_Screen_Status;

typedef enum
{
  MAPS_Menu_Clock = 0,        //Menu Clock
  MAPS_Menu_StopWatch = 1,    //Menu Stop Watch
  MAPS_Menu_AlarmClock = 2,   //Menu Alarm Clock
} MAPS_Menu_Selection;

typedef enum
{
  MAPS_Clock_Dial = 0,        //Clock Dial
  MAPS_Clock_Digit = 1,       //Clock Digit
  MAPS_Clock_Style_Max = 2,   //Maximum clock style
} MAPS_Clock_Style;

/*
**variate declaration
*/
extern uint16 LPTMR_Count;

/*
**function declaration
*/
extern void main(void);


#endif