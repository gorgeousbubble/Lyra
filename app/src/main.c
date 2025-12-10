/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       main.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "init.h"
#include "main.h"
#include "maps_dock_lcd.h"
#include "maps_dock_key.h"
#include "maps_dock_rocker.h"

/*
** MAPS Screen Status
*/
MAPS_Screen_Status MAPS_Screen_StatusN[MAPS_Screen_Status_Max] = {
        MAPS_Screen_Saver,
        MAPS_Screen_Menu,
        MAPS_Screen_Normal,
};

/*
** MAPS Menu Selection
*/
MAPS_Menu_Selection MAPS_Menu_SelectionN[MAPS_Menu_Selection_Max] = {
        MAPS_Menu_Clock,
        MAPS_Menu_StopWatch,
        MAPS_Menu_AlarmClock,
        MAPS_Menu_WorldClock,
        MAPS_Menu_SpiritLevel,
        MAPS_Menu_Configure_Adjust,
};

/*
** MAPS Clock Style
*/
MAPS_Clock_Style MAPS_Clock_StyleN[MAPS_Clock_Style_Max] = {
        MAPS_Clock_Dial,
        MAPS_Clock_Digit,
};

/*
** MAPS Alarm Clock Mode
*/
MAPS_AlarmClock_Mode MAPS_AlarmClock_ModeN[MAPS_AlarmClock_Mode_Max] = {
        MAPS_AlarmClock_List,
        MAPS_AlarmClock_Edit,
};

/*
** MAPS World Clock Timezone
*/
MAPS_WorldClock_Timezone MAPS_WorldClock_TimezoneN[MAPS_WorldClock_Timezone_Max] = {
  MAPS_WorldClock_Timezone_Beijing,
  MAPS_WorldClock_Timezone_Shanghai,
};

/*
** MAPS World Clock Timezone Array
*/
MAPS_WorldClock_Time MAPS_WorldClock_Timezone_Array[MAPS_WorldClock_Timezone_Max] = {
  {MAPS_WorldClock_Timezone_Beijing, "Beijing", 8, 0},
  {MAPS_WorldClock_Timezone_Shanghai, "Shanghai", 8, 0},
};

/*
 *  @brief              main
 *  @note               main function
 *  @since              v1.0
 *  Sample usage:       void main(void)
 */
void main(void)
{
  Site_t Site = {0, 0};

  AllInit(); // initialization
  ReadConf(); // read configuration from e2prom

  MAPS_LCDC_BMP_From_SD("0:/Mitsuha.bmp", Site); // load image

  for (;;)
  {
    MAPS_Dock_KEY_Incident();           // Independent button serial port transmission
    MAPS_Dock_Rocker_Key_LCM_Control(); // LCM display
  }
}