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

#include "func.h"
#include "health.h"
#include "init.h"
#include "it.h"
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
    MAPS_WorldClock_Timezone_Hongkong,
    MAPS_WorldClock_Timezone_Taipei,
    MAPS_WorldClock_Timezone_Singapore,
    MAPS_WorldClock_Timezone_Seoul,
    MAPS_WorldClock_Timezone_Tokyo,
    MAPS_WorldClock_Timezone_Sydney,
    MAPS_WorldClock_Timezone_SanFrancisco,
    MAPS_WorldClock_Timezone_NewYork,
};

/*
** MAPS World Clock Timezone Array
*/
MAPS_WorldClock_Time MAPS_WorldClock_Timezone_Array[MAPS_WorldClock_Timezone_Max] = {
    {MAPS_WorldClock_Timezone_Beijing, 8, 0},
    {MAPS_WorldClock_Timezone_Shanghai, 8, 0},
    {MAPS_WorldClock_Timezone_Hongkong, 8, 0},
    {MAPS_WorldClock_Timezone_Taipei, 8, 0},
    {MAPS_WorldClock_Timezone_Singapore, 8, 0},
    {MAPS_WorldClock_Timezone_Seoul, 9, 0},
    {MAPS_WorldClock_Timezone_Tokyo, 9, 0},
    {MAPS_WorldClock_Timezone_Sydney, 10, 0},
    {MAPS_WorldClock_Timezone_SanFrancisco, -8, 0},
    {MAPS_WorldClock_Timezone_NewYork, -5, 0},
};

/*
** MAPS Configure Adjust Mode
*/
MAPS_ConfigureAdjust_Mode MAPS_ConfigureAdjust_ModeN[MAPS_ConfigureAdjust_Max] = {
    MAPS_ConfigureAdjust_List,
    MAPS_ConfigureAdjust_Clock,
    MAPS_ConfigureAdjust_Date,
    MAPS_ConfigureAdjust_Tense,
};

/*
** MAPS Configure Adjust Tense Format
*/
MAPS_ConfigureAdjust_Tense_Format MAPS_ConfigureAdjust_Tense_FormatN[2] = {
    MAPS_ConfigureAdjust_Tense_24H,
    MAPS_ConfigureAdjust_Tense_12H,
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

        AllInit();  // initialization
        ReadConf(); // read configuration from e2prom

        MAPS_LCDC_BMP_From_SD("0:/Mitsuha.bmp", Site); // load image

        for (;;)
        {
                MAPS_Dock_KEY_Incident();            // OLED key handling and display
                MAPS_Dock_Rocker_Key_LCM_Control();  // LCM rocker key handling and display
                if (UART_Send_Flag)                  // Send filter data to host (set by PIT1 every 10ms)
                {
                    UART_Send_Flag = 0;
                    UART_Send_Parameters();
                }
                Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Calculate(); // SpO2/HR algorithm (runs when buffer full)
        }
}