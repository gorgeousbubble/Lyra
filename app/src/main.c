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
MAPS_Screen_Status MAPS_Screen_StatusN[MAPS_Screen_Status_Max]=
{
  MAPS_Screen_Saver,
  MAPS_Screen_Menu,
  MAPS_Screen_Normal,
};

/*
** MAPS Menu Selection
*/
MAPS_Menu_Selection MAPS_Menu_SelectionN[MAPS_Menu_Selection_Max]=
{
  MAPS_Menu_Clock,
  MAPS_Menu_StopWatch,
  MAPS_Menu_AlarmClock,
  //MAPS_Menu_WorldClock,
};

/*
** MAPS Clock Style
*/
MAPS_Clock_Style MAPS_Clock_StyleN[MAPS_Clock_Style_Max]=
{
  MAPS_Clock_Dial,
  MAPS_Clock_Digit,
};

/*
** MAPS Alarm Clock Mode
*/
MAPS_AlarmClock_Mode MAPS_AlarmClock_ModeN[MAPS_AlarmClock_Mode_Max]=
{
  MAPS_AlarmClock_List,
  MAPS_AlarmClock_Edit,
};

/*
 *  @brief              main
 *  @note               main function
 *  @since              v1.0
 *  Sample usage:       void main(void)
 */
void main(void)
{
  Site_t Site={0,0};
  
  AllInit(); //initialization
  
  MAPS_LCDC_BMP_From_SD("0:/Mitsuha.bmp",Site);//load image
  
  for(;;)
  {
    MAPS_Dock_KEY_Incident();//Independent button serial port transmission
    MAPS_Dock_Rocker_Key_LCM_Control();//LCM display
  }
}