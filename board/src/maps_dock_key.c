/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_key.c
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "animation.h"
#include "conf.h"
#include "dwt.h"
#include "gpio.h"
#include "it.h"
#include "main.h"
#include "maps_dock_key.h"
#include "oled_i2c.h"
#include "rtc.h"
#include "uart.h"
#include "watch.h"
#include <time.h>

/*
**Independent button port
*/
PTXn MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY_MAX] = {PTB21, PTB22, PTB23, PTB20};

int Lyra_Status = 0;                                                    // Screen status
int Lyra_Menu_Selection = 0;                                            // Menu selection
int Lyra_Clock_Style = 0;                                               // Clock style
int Lyra_StopWatch_Style = 0;                                           // Stop Watch style
int Lyra_AlarmClock_Mode = 0;                                           // Alarm Clock mode
int Lyra_AlarmClock_List_Cursor = 0;                                    // Alarm Clock list cursor
int Lyra_AlarmClock_List_Time_Hour = 0;                                 // Alarm Clock list time hour
int Lyra_AlarmClock_List_Time_Minute = 0;                               // Alarm Clock list time minute
int Lyra_AlarmClock_Edit_Cursor = 0;                                    // Alarm Clock edit cursor
int Lyra_AlarmClock_Edit_Number[4] = {0};                               // Alarm Clock edit number (hh:mm)
int Lyra_WorldClock_Time_Cursor = 0;                                    // World Clock time cursor
int Lyra_ConfigureAdjust_Mode = 0;                                      // Configure Adjust mode
int Lyra_ConfigureAdjust_List_Cursor = 1;                               // Configure Adjust list cursor
int Lyra_ConfigureAdjust_Clock_Cursor = 0;                              // Configure Adjust clock cursor
int Lyra_ConfigureAdjust_Clock_Number[6] = {0};                         // Configure Adjust clock number (hh:mm:ss)
int Lyra_ConfigureAdjust_Date_Cursor = 0;                               // Configure Adjust date cursor
int Lyra_ConfigureAdjust_Date_Number[8] = {0};                          // Configure Adjust date number (yyyy:mm:dd)
int Lyra_ConfigureAdjust_Tense_Format = MAPS_ConfigureAdjust_Tense_24H; // Configure Adjust tense format (64: 24-hour/65: 12-hour)

CoordCache Lyra_Dynamic_Cache[2] = {0}; // Dynamic cache

/*
 *  @brief      MAPs_Dock_KEY initializes all keys
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_KEY_ALL_Init();//MAPs_Dock_KEY initializes all keys
 */
void MAPS_Dock_KEY_ALL_Init(void)
{
  GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY0], GPI, 0);
  GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY1], GPI, 0);
  GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY2], GPI, 0);
  GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY3], GPI, 0);
}

/*
 *  @brief      MAPs_Dock_KEY initializes a single button
 *  @para       MAPS_Dock_KEYn  MAPS_Dock_KEYx  key name
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_KEY_KEYn_Init();//MAPs_Dock_KEY initializes a single button
 */
void MAPS_Dock_KEY_KEYn_Init(MAPS_Dock_KEYn MAPS_Dock_KEYx)
{
  switch (MAPS_Dock_KEYx)
  {
  case MAPS_Dock_KEY0: // KEY0
    GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY0], GPI, 0);
    break;
  case MAPS_Dock_KEY1: // KEY1
    GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY1], GPI, 0);
    break;
  case MAPS_Dock_KEY2: // KEY2
    GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY2], GPI, 0);
    break;
  case MAPS_Dock_KEY3: // KEY3
    GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY3], GPI, 0);
    break;
  case MAPS_Dock_KEY_MAX: // KEYALL
    GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY0], GPI, 0);
    GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY1], GPI, 0);
    GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY2], GPI, 0);
    GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY3], GPI, 0);
    break;
  default:
    ASSERT(0);
    break;
  }
}

/*
 *  @brief      MAPs_Dock_KEY retrieves the status of a single key
 *  @para       MAPS_Dock_KEYn  MAPS_Dock_KEYx  key name
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_KEY_KEYn_Get();//MAPs_Dock_KEY retrieves the status of a single key
 */
uint8 MAPS_Dock_KEY_KEYn_Get(MAPS_Dock_KEYn MAPS_Dock_KEYx)
{
  uint8 KEY_Status = 0;

  switch (MAPS_Dock_KEYx)
  {
  case MAPS_Dock_KEY0:
    KEY_Status = GPIO_GET(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY0]);
    break;
  case MAPS_Dock_KEY1:
    KEY_Status = GPIO_GET(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY1]);
    break;
  case MAPS_Dock_KEY2:
    KEY_Status = GPIO_GET(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY2]);
    break;
  case MAPS_Dock_KEY3:
    KEY_Status = GPIO_GET(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY3]);
    break;
  default:
    ASSERT(0);
    break;
  }

  return KEY_Status;
}

/*
 *  @brief      MAPs_Dock_KEY single button press detection
 *  @para       MAPS_Dock_KEYn  MAPS_Dock_KEYx  key name
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_KEY_KEYn_Check();//MAPs_Dock_KEY single button press detection
 */
MAPS_Dock_KEY_Status MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEYn MAPS_Dock_KEYx)
{
  if (GPIO_GET(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEYx]) == MAPS_Dock_KEY_On)
  {
    MAPS_Dock_KEY_Delay(10); // Delay debounce by 10ms
    if (GPIO_GET(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEYx]) == MAPS_Dock_KEY_On)
    {
      return MAPS_Dock_KEY_On;
    }
  }

  return MAPS_Dock_KEY_Off;
}

/*
 *  @brief      MAPs_Dock_KEY scans all keys
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_KEY_Incident();//MAPs_Dock_KEY scans all keys
 */
void MAPS_Dock_KEY_Incident(void)
{
  // render screen saver
  if (Lyra_Status == MAPS_Screen_Saver)
  {
    Oled_I2C_Draw_BMP_128x64(LCM_Freescale_logo, OLED_Invert_Color);
    // Press KEY0
    if (MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY0) == MAPS_Dock_KEY_On)
    {
      Lyra_Status++; // Switch to normal screen
      if (Lyra_Status >= MAPS_Screen_Status_Max)
      {
        Lyra_Status = MAPS_Screen_Status_Max - 1;
      }
      MAPS_Dock_KEY_Delay(100); // Button delay 500ms
    }
  }
  else if (MAPS_Screen_StatusN[Lyra_Status] == MAPS_Screen_Menu)
  {
    // Check dock key press status
    // Press KEY0
    if (MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY0) == MAPS_Dock_KEY_On)
    {
      Lyra_Status++; // Switch to normal screen
      if (Lyra_Status >= MAPS_Screen_Status_Max)
      {
        Lyra_Status = MAPS_Screen_Status_Max - 1;
      }
      MAPS_Dock_KEY_Delay(100); // Button delay 500ms
    }
    // Press KEY1
    else if (MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY1) == MAPS_Dock_KEY_On)
    {
      Lyra_Status--; // Return back to screen saver
      if (Lyra_Status < MAPS_Screen_Saver)
      {
        Lyra_Status = MAPS_Screen_Saver;
      }
      MAPS_Dock_KEY_Delay(100); // Button delay 500ms
    }
    // Press KEY2
    else if (MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY2) == MAPS_Dock_KEY_On)
    {
      switch (MAPS_Menu_SelectionN[Lyra_Menu_Selection])
      {
      case MAPS_Menu_Clock:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Watch_icon_coordinate, LCM_Watch_icon_coordinate_length, LCM_Configure_Adjust_icon_coordinate, LCM_Configure_Adjust_icon_coordinate_length, 0, 0, 5);
        Lyra_Menu_Selection--;
        if (Lyra_Menu_Selection < 0)
        {
          Lyra_Menu_Selection = MAPS_Menu_Selection_Max - 1;
        }
        break;
      case MAPS_Menu_StopWatch:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Stop_Watch_icon_coordinate, LCM_Stop_Watch_icon_coordinate_length, LCM_Watch_icon_coordinate, LCM_Watch_icon_coordinate_length, 0, 0, 5);
        Lyra_Menu_Selection--;
        if (Lyra_Menu_Selection < 0)
        {
          Lyra_Menu_Selection = MAPS_Menu_Selection_Max - 1;
        }
        break;
      case MAPS_Menu_AlarmClock:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Alarm_Clock_icon_coordinate, LCM_Alarm_Clock_icon_coordinate_length, LCM_Stop_Watch_icon_coordinate, LCM_Stop_Watch_icon_coordinate_length, 0, 0, 5);
        Lyra_Menu_Selection--;
        if (Lyra_Menu_Selection < 0)
        {
          Lyra_Menu_Selection = MAPS_Menu_Selection_Max - 1;
        }
        break;
      case MAPS_Menu_WorldClock:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_World_Clock_icon_coordinate, LCM_World_Clock_icon_coordinate_length, LCM_Alarm_Clock_icon_coordinate, LCM_Alarm_Clock_icon_coordinate_length, 0, 0, 5);
        Lyra_Menu_Selection--;
        if (Lyra_Menu_Selection < 0)
        {
          Lyra_Menu_Selection = MAPS_Menu_Selection_Max - 1;
        }
        break;
      case MAPS_Menu_SpiritLevel:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Spirit_Level_icon_coordinate, LCM_Spirit_Level_icon_coordinate_length, LCM_World_Clock_icon_coordinate, LCM_World_Clock_icon_coordinate_length, 0, 0, 5);
        Lyra_Menu_Selection--;
        if (Lyra_Menu_Selection < 0)
        {
          Lyra_Menu_Selection = MAPS_Menu_Selection_Max - 1;
        }
        break;
      case MAPS_Menu_Configure_Adjust:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Configure_Adjust_icon_coordinate, LCM_Configure_Adjust_icon_coordinate_length, LCM_Spirit_Level_icon_coordinate, LCM_Spirit_Level_icon_coordinate_length, 0, 0, 5);
        Lyra_Menu_Selection--;
        if (Lyra_Menu_Selection < 0)
        {
          Lyra_Menu_Selection = MAPS_Menu_Selection_Max - 1;
        }
        break;
      default:
        break;
      }
      MAPS_Dock_KEY_Delay(100); // Button delay 500ms
    }
    // Press KEY3
    else if (MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY3) == MAPS_Dock_KEY_On)
    {
      switch (MAPS_Menu_SelectionN[Lyra_Menu_Selection])
      {
      case MAPS_Menu_Clock:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Watch_icon_coordinate, LCM_Watch_icon_coordinate_length, LCM_Stop_Watch_icon_coordinate, LCM_Stop_Watch_icon_coordinate_length, 1, 0, 5);
        Lyra_Menu_Selection++;
        if (Lyra_Menu_Selection >= MAPS_Menu_Selection_Max)
        {
          Lyra_Menu_Selection = 0;
        }
        break;
      case MAPS_Menu_StopWatch:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Stop_Watch_icon_coordinate, LCM_Stop_Watch_icon_coordinate_length, LCM_Alarm_Clock_icon_coordinate, LCM_Alarm_Clock_icon_coordinate_length, 1, 0, 5);
        Lyra_Menu_Selection++;
        if (Lyra_Menu_Selection >= MAPS_Menu_Selection_Max)
        {
          Lyra_Menu_Selection = 0;
        }
        break;
      case MAPS_Menu_AlarmClock:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Alarm_Clock_icon_coordinate, LCM_Alarm_Clock_icon_coordinate_length, LCM_World_Clock_icon_coordinate, LCM_World_Clock_icon_coordinate_length, 1, 0, 5);
        Lyra_Menu_Selection++;
        if (Lyra_Menu_Selection >= MAPS_Menu_Selection_Max)
        {
          Lyra_Menu_Selection = 0;
        }
        break;
      case MAPS_Menu_WorldClock:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_World_Clock_icon_coordinate, LCM_World_Clock_icon_coordinate_length, LCM_Spirit_Level_icon_coordinate, LCM_Spirit_Level_icon_coordinate_length, 1, 0, 5);
        Lyra_Menu_Selection++;
        if (Lyra_Menu_Selection >= MAPS_Menu_Selection_Max)
        {
          Lyra_Menu_Selection = 0;
        }
        break;
      case MAPS_Menu_SpiritLevel:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Spirit_Level_icon_coordinate, LCM_Spirit_Level_icon_coordinate_length, LCM_Configure_Adjust_icon_coordinate, LCM_Configure_Adjust_icon_coordinate_length, 1, 0, 5);
        Lyra_Menu_Selection++;
        if (Lyra_Menu_Selection >= MAPS_Menu_Selection_Max)
        {
          Lyra_Menu_Selection = 0;
        }
        break;
      case MAPS_Menu_Configure_Adjust:
        Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Configure_Adjust_icon_coordinate, LCM_Configure_Adjust_icon_coordinate_length, LCM_Watch_icon_coordinate, LCM_Watch_icon_coordinate_length, 1, 0, 5);
        Lyra_Menu_Selection++;
        if (Lyra_Menu_Selection >= MAPS_Menu_Selection_Max)
        {
          Lyra_Menu_Selection = 0;
        }
        break;
      default:
        break;
      }
      MAPS_Dock_KEY_Delay(100); // Button delay 500ms
    }
    // Display the current menu selection
    switch (MAPS_Menu_SelectionN[Lyra_Menu_Selection])
    {
    case MAPS_Menu_Clock:
      Oled_I2C_Draw_BMP_128x64(LCM_Watch_icon, OLED_Invert_Color);
      break;
    case MAPS_Menu_StopWatch:
      Oled_I2C_Draw_BMP_128x64(LCM_Stop_Watch_icon, OLED_Invert_Color);
      break;
    case MAPS_Menu_AlarmClock:
      Oled_I2C_Draw_BMP_128x64(LCM_Alarm_Clock_icon, OLED_Invert_Color);
      break;
    case MAPS_Menu_WorldClock:
      Oled_I2C_Draw_BMP_128x64(LCM_World_Clock_icon, OLED_Invert_Color);
      break;
    case MAPS_Menu_SpiritLevel:
      Oled_I2C_Draw_BMP_128x64(LCM_Spirit_Level_icon, OLED_Invert_Color);
      break;
    case MAPS_Menu_Configure_Adjust:
      Oled_I2C_Draw_BMP_128x64(LCM_Configure_Adjust_icon, OLED_Invert_Color);
      break;
    default:
      break;
    }
  }
  else if (Lyra_Status == MAPS_Screen_Normal)
  {
    // Press KEY0
    if (MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY0) == MAPS_Dock_KEY_On)
    {
      // Check current menu selection is stopwatch
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_StopWatch)
      {
        Stop_Watch_State = !Stop_Watch_State; // Toggle stopwatch state
      }
      // Check current menu selection is alarm clock
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_AlarmClock)
      {
        if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_List)
        {
          // get alarm clock time from list
          int hour = 0;
          int minute = 0;

          // get alarm clock time from list
          Get_Alarm_Clock_Time_From_List(Lyra_AlarmClock_List_Cursor, &hour, &minute);
          // set alarm clock edit number
          Lyra_AlarmClock_Edit_Number[0] = hour / 10;   // Hour tens
          Lyra_AlarmClock_Edit_Number[1] = hour % 10;   // Hour
          Lyra_AlarmClock_Edit_Number[2] = minute / 10; // Minute tens
          Lyra_AlarmClock_Edit_Number[3] = minute % 10; // Minute
          Lyra_AlarmClock_Edit_Cursor = 0;              // Reset edit cursor

          Lyra_AlarmClock_Mode++; // Switch to edit mode
          if (Lyra_AlarmClock_Mode >= MAPS_AlarmClock_Mode_Max)
          {
            Lyra_AlarmClock_Mode = MAPS_AlarmClock_Mode_Max - 1;
          }
        }
        else if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_Edit)
        {
          // save alarm clock time to list
          int hour = Lyra_AlarmClock_Edit_Number[0] * 10 + Lyra_AlarmClock_Edit_Number[1];
          int minute = Lyra_AlarmClock_Edit_Number[2] * 10 + Lyra_AlarmClock_Edit_Number[3];
          // display waiting icon
          Oled_I2C_Draw_BMP_128x64(LCM_Wait_icon, OLED_Invert_Color);
          // get alarm clock list length
          if (Get_Alarm_Clock_List_Len() > Lyra_AlarmClock_List_Cursor)
          {
            // modify existing alarm clock time in list
            Mod_Alarm_Clock_Time_To_List(Lyra_AlarmClock_List_Cursor, hour, minute);
          }
          else
          {
            // add new alarm clock time to list
            Add_Alarm_Clock_Time_To_List(hour, minute);
          }
          // write alarm clock list to e2prom
          Write_Alarm_Clock_List_To_E2PROM();
          // clean alarm clock list
          Clean_Alarm_Clock_List();
          // read alarm clock time from E2PROM
          Read_Alarm_Clock_E2PROM_To_List();

          Lyra_AlarmClock_Mode--; // Switch to list mode
          if (Lyra_AlarmClock_Mode < MAPS_AlarmClock_List)
          {
            Lyra_AlarmClock_Mode = MAPS_AlarmClock_List;
          }
        }
      }
      // Check current menu selection is configure adjust
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_Configure_Adjust)
      {
        if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_List)
        {
          struct tm time = RTC_Get_Time_Format();
          switch (Lyra_ConfigureAdjust_List_Cursor)
          {
          case MAPS_ConfigureAdjust_Clock:
            Lyra_ConfigureAdjust_Clock_Number[0] = time.tm_hour / 10; // Hour tens
            Lyra_ConfigureAdjust_Clock_Number[1] = time.tm_hour % 10; // Hour units
            Lyra_ConfigureAdjust_Clock_Number[2] = time.tm_min / 10;  // Minute tens
            Lyra_ConfigureAdjust_Clock_Number[3] = time.tm_min % 10;  // Minute units
            Lyra_ConfigureAdjust_Clock_Number[4] = time.tm_sec / 10;  // Second tens
            Lyra_ConfigureAdjust_Clock_Number[5] = time.tm_sec % 10;  // Second units
            Lyra_ConfigureAdjust_Clock_Cursor = 0;
            Lyra_ConfigureAdjust_Mode = MAPS_ConfigureAdjust_Clock;
            break;
          case MAPS_ConfigureAdjust_Date:
            Lyra_ConfigureAdjust_Date_Number[0] = (time.tm_year + 1900) / 1000;       // Year thousands
            Lyra_ConfigureAdjust_Date_Number[1] = ((time.tm_year + 1900) / 100) % 10; // Year hundreds
            Lyra_ConfigureAdjust_Date_Number[2] = ((time.tm_year + 1900) / 10) % 10;  // Year tens
            Lyra_ConfigureAdjust_Date_Number[3] = (time.tm_year + 1900) % 10;         // Year units
            Lyra_ConfigureAdjust_Date_Number[4] = (time.tm_mon + 1) / 10;             // Month tens
            Lyra_ConfigureAdjust_Date_Number[5] = (time.tm_mon + 1) % 10;             // Month units
            Lyra_ConfigureAdjust_Date_Number[6] = time.tm_mday / 10;                  // Day tens
            Lyra_ConfigureAdjust_Date_Number[7] = time.tm_mday % 10;                  // Day units
            Lyra_ConfigureAdjust_Date_Cursor = 0;
            Lyra_ConfigureAdjust_Mode = MAPS_ConfigureAdjust_Date;
            break;
          case MAPS_ConfigureAdjust_Tense:
            // read configure adjust tense format from e2prom
            Read_Configure_Adjust_Tense_E2PROM_To_Value();
            if (Configure_Adjust_Tense == 0)
            {
              Lyra_ConfigureAdjust_Tense_Format = MAPS_ConfigureAdjust_Tense_24H;
            }
            else if (Configure_Adjust_Tense == 1)
            {
              Lyra_ConfigureAdjust_Tense_Format = MAPS_ConfigureAdjust_Tense_12H;
            }
            else
            {
              Lyra_ConfigureAdjust_Tense_Format = MAPS_ConfigureAdjust_Tense_24H;
            }
            Lyra_ConfigureAdjust_Mode = MAPS_ConfigureAdjust_Tense;
            break;
          default:
            break;
          }
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Clock)
        {
          // save clock time to rtc clock
          int hour = Lyra_ConfigureAdjust_Clock_Number[0] * 10 + Lyra_ConfigureAdjust_Clock_Number[1];
          int minute = Lyra_ConfigureAdjust_Clock_Number[2] * 10 + Lyra_ConfigureAdjust_Clock_Number[3];
          int second = Lyra_ConfigureAdjust_Clock_Number[4] * 10 + Lyra_ConfigureAdjust_Clock_Number[5];
          // display waiting icon
          Oled_I2C_Draw_BMP_128x64(LCM_Wait_icon, OLED_Invert_Color);
          // get rtc clock
          struct tm time = RTC_Get_Time_Format();
          // update clock time
          time.tm_hour = hour;
          time.tm_min = minute;
          time.tm_sec = second;
          RTC_Set_Time_Format(&time); // Set RTC time format*/
          Lyra_ConfigureAdjust_Mode = MAPS_ConfigureAdjust_List;
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Date)
        {
          // save date time to rtc clock
          int year = Lyra_ConfigureAdjust_Date_Number[0] * 1000 + Lyra_ConfigureAdjust_Date_Number[1] * 100 + Lyra_ConfigureAdjust_Date_Number[2] * 10 + Lyra_ConfigureAdjust_Date_Number[3];
          int month = Lyra_ConfigureAdjust_Date_Number[4] * 10 + Lyra_ConfigureAdjust_Date_Number[5];
          int day = Lyra_ConfigureAdjust_Date_Number[6] * 10 + Lyra_ConfigureAdjust_Date_Number[7];
          // display waiting icon
          Oled_I2C_Draw_BMP_128x64(LCM_Wait_icon, OLED_Invert_Color);
          // get rtc clock
          struct tm time = RTC_Get_Time_Format();
          // update clock time
          time.tm_year = year - 1900;
          time.tm_mon = month - 1;
          time.tm_mday = day;
          RTC_Set_Time_Format(&time); // Set RTC time format*/
          Lyra_ConfigureAdjust_Mode = MAPS_ConfigureAdjust_List;
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Tense)
        {
          // should save tense format to memory
          // display waiting icon
          Oled_I2C_Draw_BMP_128x64(LCM_Wait_icon, OLED_Invert_Color);
          // write current tense format to e2prom
          if (Lyra_ConfigureAdjust_Tense_Format == MAPS_ConfigureAdjust_Tense_24H)
          {
            Configure_Adjust_Tense = 0;
          }
          else if (Lyra_ConfigureAdjust_Tense_Format == MAPS_ConfigureAdjust_Tense_12H)
          {
            Configure_Adjust_Tense = 1;
          }
          else
          {
            Configure_Adjust_Tense = 0;
          }
          Write_Configure_Adjust_Tense_Value_To_E2PROM();
          Read_Configure_Adjust_Tense_E2PROM_To_Value();
          Lyra_ConfigureAdjust_Mode = MAPS_ConfigureAdjust_List;
        }
      }
      MAPS_Dock_KEY_Delay(100); // Button delay 500ms
    }
    // Press KEY1
    else if (MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY1) == MAPS_Dock_KEY_On)
    {
      // Check current menu selection is stopwatch
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_StopWatch)
      {
        Stop_Watch_State = 0;           // Stop the stopwatch
        Stop_Watch_Now.Minute = 0;      // Reset minute
        Stop_Watch_Now.Second = 0;      // Reset second
        Stop_Watch_Now.Centisecond = 0; // Reset centisecond
      }
      // Check current menu selection is alarm clock
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_AlarmClock)
      {
        if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_Edit)
        {
          // display waiting icon
          Oled_I2C_Draw_BMP_128x64(LCM_Wait_icon, OLED_Invert_Color);
          // delete alarm clock time from list
          Del_Alarm_Clock_Time_From_List(Lyra_AlarmClock_List_Cursor);
          // write alarm clock list to e2prom
          Write_Alarm_Clock_List_To_E2PROM();
          // clean alarm clock list
          Clean_Alarm_Clock_List();
          // read alarm clock time from E2PROM
          Read_Alarm_Clock_E2PROM_To_List();

          Lyra_AlarmClock_Mode--; // Switch to list mode
          if (Lyra_AlarmClock_Mode < MAPS_AlarmClock_List)
          {
            Lyra_AlarmClock_Mode = MAPS_AlarmClock_List;
          }
        }
      }
      // Check current menu selection is configure adjust
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_Configure_Adjust)
      {
        if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Clock)
        {
          Lyra_ConfigureAdjust_Mode = MAPS_ConfigureAdjust_List;
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Date)
        {
          Lyra_ConfigureAdjust_Mode = MAPS_ConfigureAdjust_List;
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Tense)
        {
          Lyra_ConfigureAdjust_Mode = MAPS_ConfigureAdjust_List;
        }
      }
      Lyra_Status--; // Switch to screen saver
      if (Lyra_Status < MAPS_Screen_Saver)
      {
        Lyra_Status = MAPS_Screen_Saver;
      }
      MAPS_Dock_KEY_Delay(100); // Button delay 500ms
    }
    // Press KEY2
    else if (MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY2) == MAPS_Dock_KEY_On)
    {
      // Check current menu selection is clock
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_Clock)
      {
        int index[2] = {MAPS_Clock_Dial, MAPS_Clock_Digit};
        Lyra_Clock_Style--; // Switch clock style
        if (Lyra_Clock_Style < 0)
        {
          Lyra_Clock_Style = MAPS_Clock_Style_Max - 1; // Reset to maximum clock style
          index[0] = MAPS_Clock_Dial;
          index[1] = MAPS_Clock_Digit;
        }
        else
        {
          index[0] = MAPS_Clock_Digit;
          index[1] = MAPS_Clock_Dial;
        }
        Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
        Animation_Screen_Switch_Horizontal_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 0, 0, 5);
      }
      // Check current menu selection is stopwatch
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_StopWatch)
      {
        int index[2] = {MAPS_StopWatch_Dial, MAPS_StopWatch_Digit};
        Lyra_StopWatch_Style--; // Switch stopwatch style
        if (Lyra_StopWatch_Style < 0)
        {
          Lyra_StopWatch_Style = MAPS_StopWatch_Style_Max - 1; // Reset to maximum stopwatch style
          index[0] = MAPS_StopWatch_Dial;
          index[1] = MAPS_StopWatch_Digit;
        }
        else
        {
          index[0] = MAPS_StopWatch_Digit;
          index[1] = MAPS_StopWatch_Dial;
        }
        Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
        Animation_Screen_Switch_Horizontal_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 0, 0, 5);
      }
      // Check current menu selection is alarm clock
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_AlarmClock)
      {
        if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_List)
        {
          int index[2] = {MAPS_AlarmClock_Timer_0, MAPS_AlarmClock_Timer_1};
          index[0] = Lyra_AlarmClock_List_Cursor;
          index[1] = Lyra_AlarmClock_List_Cursor - 1;
          Lyra_AlarmClock_List_Cursor--; // Move alarm clock list cursor up
          if (Lyra_AlarmClock_List_Cursor < 0)
          {
            Lyra_AlarmClock_List_Cursor = 0; // Reset to minimum alarm clock list cursor position
            index[0] = Lyra_AlarmClock_List_Cursor;
            index[1] = Lyra_AlarmClock_List_Cursor;
          }
          Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
          Animation_Screen_Switch_Vertical_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 0, 0, 5);
        }
        else if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_Edit)
        {
          Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor]++; // Increase alarm clock edit number
          if (Lyra_AlarmClock_Edit_Cursor == 0)                       // Hour tens
          {
            if (Lyra_AlarmClock_Edit_Number[1] >= 5) // If hour units is 5 or more
            {
              if (Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] >= 2)
              {
                Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] = 0; // Reset to 0
              }
            }
            else // If hour units is less than 5
            {
              if (Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] >= 3)
              {
                Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] = 0; // Reset to 0
              }
            }
          }
          else if (Lyra_AlarmClock_Edit_Cursor == 1) // Hour units
          {
            if (Lyra_AlarmClock_Edit_Number[0] == 2) // If hour tens is 2
            {
              if (Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] >= 4)
              {
                Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] = 0; // Reset to 0
              }
            }
            else // If hour tens is 0 or 1
            {
              if (Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] >= 10)
              {
                Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] = 0; // Reset to 0
              }
            }
          }
          else if (Lyra_AlarmClock_Edit_Cursor == 2) // Minute tens
          {
            if (Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] >= 6)
            {
              Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] = 0; // Reset to 0
            }
          }
          else if (Lyra_AlarmClock_Edit_Cursor == 3) // Minute units
          {
            if (Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] >= 10)
            {
              Lyra_AlarmClock_Edit_Number[Lyra_AlarmClock_Edit_Cursor] = 0; // Reset to 0
            }
          }
        }
      }
      // Check current menu selection is world clock
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_WorldClock)
      {
        int index[2] = {MAPS_WorldClock_Timezone_Beijing, MAPS_WorldClock_Timezone_Shanghai};
        Lyra_WorldClock_Time_Cursor--; // Move world clock time cursor up
        if (Lyra_WorldClock_Time_Cursor < 0)
        {
          Lyra_WorldClock_Time_Cursor = MAPS_WorldClock_Timezone_Max - 1; // Reset to minimum world clock time cursor position
          index[0] = MAPS_WorldClock_Timezone_Beijing;
          index[1] = MAPS_WorldClock_Timezone_NewYork;
        }
        else
        {
          if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Beijing)
          {
            index[0] = MAPS_WorldClock_Timezone_Shanghai;
            index[1] = MAPS_WorldClock_Timezone_Beijing;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Shanghai)
          {
            index[0] = MAPS_WorldClock_Timezone_Hongkong;
            index[1] = MAPS_WorldClock_Timezone_Shanghai;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Hongkong)
          {
            index[0] = MAPS_WorldClock_Timezone_Taipei;
            index[1] = MAPS_WorldClock_Timezone_Hongkong;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Taipei)
          {
            index[0] = MAPS_WorldClock_Timezone_Singapore;
            index[1] = MAPS_WorldClock_Timezone_Taipei;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Singapore)
          {
            index[0] = MAPS_WorldClock_Timezone_Seoul;
            index[1] = MAPS_WorldClock_Timezone_Singapore;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Seoul)
          {
            index[0] = MAPS_WorldClock_Timezone_Tokyo;
            index[1] = MAPS_WorldClock_Timezone_Seoul;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Tokyo)
          {
            index[0] = MAPS_WorldClock_Timezone_Sydney;
            index[1] = MAPS_WorldClock_Timezone_Tokyo;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Sydney)
          {
            index[0] = MAPS_WorldClock_Timezone_SanFrancisco;
            index[1] = MAPS_WorldClock_Timezone_Sydney;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_SanFrancisco)
          {
            index[0] = MAPS_WorldClock_Timezone_NewYork;
            index[1] = MAPS_WorldClock_Timezone_SanFrancisco;
          }
        }
        Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
        Animation_Screen_Switch_Horizontal_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 0, 0, 5);
      }
      // Check current menu selection is configure adjust
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_Configure_Adjust)
      {
        if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_List)
        {
          int index[2] = {MAPS_ConfigureAdjust_Clock, MAPS_ConfigureAdjust_Date};
          index[0] = Lyra_ConfigureAdjust_List_Cursor;
          index[1] = Lyra_ConfigureAdjust_List_Cursor - 1;
          Lyra_ConfigureAdjust_List_Cursor--; // Move configure adjust list cursor up
          if (Lyra_ConfigureAdjust_List_Cursor < 1)
          {
            Lyra_ConfigureAdjust_List_Cursor = 1; // Reset to minimum configure adjust list cursor position
            index[0] = Lyra_ConfigureAdjust_List_Cursor;
            index[1] = Lyra_ConfigureAdjust_List_Cursor;
          }
          Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
          Animation_Screen_Switch_Vertical_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 0, 0, 5);
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Clock)
        {
          Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor]++; // Increase configure adjust clock edit number
          if (Lyra_ConfigureAdjust_Clock_Cursor == 0)
          {
            if (Lyra_ConfigureAdjust_Clock_Number[1] >= 5) // If hour units is 5 or more
            {
              if (Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] >= 2)
              {
                Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] = 0; // Reset to 0
              }
            }
            else // If hour units is less than 5
            {
              if (Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] >= 3)
              {
                Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] = 0; // Reset to 0
              }
            }
          }
          else if (Lyra_ConfigureAdjust_Clock_Cursor == 1) // Hour units
          {
            if (Lyra_ConfigureAdjust_Clock_Number[0] == 2) // If hour tens is 2
            {
              if (Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] >= 4)
              {
                Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] = 0; // Reset to 0
              }
            }
            else // If hour tens is 0 or 1
            {
              if (Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] >= 10)
              {
                Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] = 0; // Reset to 0
              }
            }
          }
          else if (Lyra_ConfigureAdjust_Clock_Cursor == 2) // Minute tens
          {
            if (Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] >= 6)
            {
              Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] = 0; // Reset to 0
            }
          }
          else if (Lyra_ConfigureAdjust_Clock_Cursor == 3) // Minute units
          {
            if (Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] >= 10)
            {
              Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] = 0; // Reset to 0
            }
          }
          else if (Lyra_ConfigureAdjust_Clock_Cursor == 4) // Second tens
          {
            if (Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] >= 6)
            {
              Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] = 0; // Reset to 0
            }
          }
          else if (Lyra_ConfigureAdjust_Clock_Cursor == 5) // Second units
          {
            if (Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] >= 10)
            {
              Lyra_ConfigureAdjust_Clock_Number[Lyra_ConfigureAdjust_Clock_Cursor] = 0; // Reset to 0
            }
          }
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Date)
        {
          Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor]++; // Increase configure adjust date edit number
          // Validate the date number based on the cursor position (date format: YYYYMMDD range: 19000101 to 20991231)
          if (Lyra_ConfigureAdjust_Date_Cursor == 0) // Year thousands
          {
            if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 3)
            {
              Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 1; // Reset to 0
            }
          }
          else if (Lyra_ConfigureAdjust_Date_Cursor == 1) // Year hundreds
          {
            if (Lyra_ConfigureAdjust_Date_Number[0] == 1) // If year thousands is 1
            {
              if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 10)
              {
                Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 9; // Reset to 9
              }
            }
            else // If year thousands is 2
            {
              if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 1)
              {
                Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
              }
            }
          }
          else if (Lyra_ConfigureAdjust_Date_Cursor == 2) // Year tens
          {
            if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 10)
            {
              Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
            }
          }
          else if (Lyra_ConfigureAdjust_Date_Cursor == 3) // Year units
          {
            if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 10)
            {
              Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
            }
          }
          else if (Lyra_ConfigureAdjust_Date_Cursor == 4) // Month tens
          {
            if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 2)
            {
              Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
            }
          }
          else if (Lyra_ConfigureAdjust_Date_Cursor == 5) // Month units
          {
            if (Lyra_ConfigureAdjust_Date_Number[4] == 1) // If month tens is 1
            {
              if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 3)
              {
                Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
              }
            }
            else // If month tens is 0
            {
              if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 10)
              {
                Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
              }
            }
          }
          else if (Lyra_ConfigureAdjust_Date_Cursor == 6) // Day tens
          {
            // check month to determine maximum day tens
            int month = Lyra_ConfigureAdjust_Date_Number[4] * 10 + Lyra_ConfigureAdjust_Date_Number[5];
            if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) // Months with 31 days
            {
              if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 4)
              {
                Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
              }
            }
            else if (month == 4 || month == 6 || month == 9 || month == 11) // Months with 30 days
            {
              if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 3)
              {
                Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
              }
            }
            else if (month == 2) // February
            {
              int year = Lyra_ConfigureAdjust_Date_Number[0] * 1000 + Lyra_ConfigureAdjust_Date_Number[1] * 100 + Lyra_ConfigureAdjust_Date_Number[2] * 10 + Lyra_ConfigureAdjust_Date_Number[3];
              // Check for leap year
              if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
              {
                if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 3) // Leap year has 29 days
                {
                  Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
                }
              }
              else
              {
                if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 2) // Non-leap year has 28 days
                {
                  Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
                }
              }
            }
            else // Invalid month, reset day tens
            {
              Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
            }
          }
          else if (Lyra_ConfigureAdjust_Date_Cursor == 7) // Day units
          {
            // check month and day tens to determine maximum day units
            int month = Lyra_ConfigureAdjust_Date_Number[4] * 10 + Lyra_ConfigureAdjust_Date_Number[5];
            int day_tens = Lyra_ConfigureAdjust_Date_Number[6];
            if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) // Months with 31 days
            {
              if (day_tens == 3) // Day tens is 3
              {
                if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 2)
                {
                  Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
                }
              }
              else // Day tens is 0, 1, or 2
              {
                if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 10)
                {
                  Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
                }
              }
            }
            else if (month == 4 || month == 6 || month == 9 || month == 11) // Months with 30 days
            {
              if (day_tens == 3) // Day tens is 3
              {
                if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 1)
                {
                  Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
                }
              }
              else // Day tens is 0, 1, or 2
              {
                if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 10)
                {
                  Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
                }
              }
            }
            else if (month == 2) // February
            {
              int year = Lyra_ConfigureAdjust_Date_Number[0] * 1000 + Lyra_ConfigureAdjust_Date_Number[1] * 100 + Lyra_ConfigureAdjust_Date_Number[2] * 10 + Lyra_ConfigureAdjust_Date_Number[3];
              // Check for leap year
              if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
              {
                if (day_tens == 2) // Day tens is 2
                {
                  if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 9)
                  {
                    Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
                  }
                }
                else // Day tens is 0 or 1
                {
                  if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 10)
                  {
                    Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
                  }
                }
              }
              else
              {
                if (day_tens == 2) // Day tens is 2
                {
                  if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 8)
                  {
                    Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
                  }
                }
                else // Day tens is 0 or 1
                {
                  if (Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] >= 10)
                  {
                    Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
                  }
                }
              }
            }
            else // Invalid month, reset day units
            {
              Lyra_ConfigureAdjust_Date_Number[Lyra_ConfigureAdjust_Date_Cursor] = 0; // Reset to 0
            }
          }
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Tense)
        {
          int index[2] = {MAPS_ConfigureAdjust_Tense_24H, MAPS_ConfigureAdjust_Tense_12H};
          index[0] = Lyra_ConfigureAdjust_Tense_Format;
          index[1] = Lyra_ConfigureAdjust_Tense_Format - 1;
          Lyra_ConfigureAdjust_Tense_Format--; // Switch tense format
          if (Lyra_ConfigureAdjust_Tense_Format < MAPS_ConfigureAdjust_Tense_24H)
          {
            Lyra_ConfigureAdjust_Tense_Format = MAPS_ConfigureAdjust_Tense_24H; // Reset to 0
            index[0] = Lyra_ConfigureAdjust_Tense_Format;
            index[1] = Lyra_ConfigureAdjust_Tense_Format;
          }
          Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
          Animation_Screen_Switch_Vertical_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 0, 0, 5);
        }
      }
      MAPS_Dock_KEY_Delay(100); // Button delay 500ms
    }
    // Press KEY3
    else if (MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY3) == MAPS_Dock_KEY_On)
    {
      // Check current menu selection is clock
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_Clock)
      {
        int index[2] = {MAPS_Clock_Dial, MAPS_Clock_Digit};
        Lyra_Clock_Style++; // Switch clock style
        if (Lyra_Clock_Style >= MAPS_Clock_Style_Max)
        {
          Lyra_Clock_Style = 0; // Reset to clock dial style
          index[0] = MAPS_Clock_Digit;
          index[1] = MAPS_Clock_Dial;
        }
        else
        {
          index[0] = MAPS_Clock_Dial;
          index[1] = MAPS_Clock_Digit;
        }
        Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
        Animation_Screen_Switch_Horizontal_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 1, 0, 5);
      }
      // Check current menu selection is stopwatch
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_StopWatch)
      {
        int index[2] = {MAPS_StopWatch_Dial, MAPS_StopWatch_Digit};
        Lyra_StopWatch_Style++; // Switch stopwatch style
        if (Lyra_StopWatch_Style >= MAPS_StopWatch_Style_Max)
        {
          Lyra_StopWatch_Style = 0; // Reset to stopwatch dial style
          index[0] = MAPS_StopWatch_Digit;
          index[1] = MAPS_StopWatch_Dial;
        }
        else
        {
          index[0] = MAPS_StopWatch_Dial;
          index[1] = MAPS_StopWatch_Digit;
        }
        Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
        Animation_Screen_Switch_Horizontal_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 1, 0, 5);
      }
      // Check current menu selection is alarm clock
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_AlarmClock)
      {
        if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_List)
        {
          int index[2] = {MAPS_AlarmClock_Timer_0, MAPS_AlarmClock_Timer_1};
          index[0] = Lyra_AlarmClock_List_Cursor;
          index[1] = Lyra_AlarmClock_List_Cursor + 1;
          Lyra_AlarmClock_List_Cursor++; // Move alarm clock list cursor down
          if (Lyra_AlarmClock_List_Cursor > Get_Alarm_Clock_List_Len())
          {
            Lyra_AlarmClock_List_Cursor = Get_Alarm_Clock_List_Len(); // Reset to minimum alarm clock list cursor position
            index[0] = Lyra_AlarmClock_List_Cursor;
            index[1] = Lyra_AlarmClock_List_Cursor;
          }
          // if cursor exceeds maximum length
          if (Lyra_AlarmClock_List_Cursor >= Alarm_Clock_Max_Len)
          {
            Lyra_AlarmClock_List_Cursor = Alarm_Clock_Max_Len - 1; // Reset to maximum alarm clock list cursor position
            index[0] = Lyra_AlarmClock_List_Cursor;
            index[1] = Lyra_AlarmClock_List_Cursor;
          }
          Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
          Animation_Screen_Switch_Vertical_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 1, 0, 5);
        }
        else if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_Edit)
        {
          Lyra_AlarmClock_Edit_Cursor++; // Move alarm clock edit cursor down
          if (Lyra_AlarmClock_Edit_Cursor >= 4)
          {
            Lyra_AlarmClock_Edit_Cursor = 0; // Reset to maximum alarm clock edit cursor position
          }
        }
      }
      // check current menu selection is world clock
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_WorldClock)
      {
        int index[2] = {MAPS_WorldClock_Timezone_Beijing, MAPS_WorldClock_Timezone_Shanghai};
        Lyra_WorldClock_Time_Cursor++; // Move world clock time cursor up
        if (Lyra_WorldClock_Time_Cursor >= MAPS_WorldClock_Timezone_Max)
        {
          Lyra_WorldClock_Time_Cursor = 0; // Reset to minimum world clock time cursor position
          index[0] = MAPS_WorldClock_Timezone_NewYork;
          index[1] = MAPS_WorldClock_Timezone_Beijing;
        }
        else
        {
          if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Shanghai)
          {
            index[0] = MAPS_WorldClock_Timezone_Beijing;
            index[1] = MAPS_WorldClock_Timezone_Shanghai;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Hongkong)
          {
            index[0] = MAPS_WorldClock_Timezone_Shanghai;
            index[1] = MAPS_WorldClock_Timezone_Hongkong;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Taipei)
          {
            index[0] = MAPS_WorldClock_Timezone_Hongkong;
            index[1] = MAPS_WorldClock_Timezone_Taipei;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Singapore)
          {
            index[0] = MAPS_WorldClock_Timezone_Taipei;
            index[1] = MAPS_WorldClock_Timezone_Singapore;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Seoul)
          {
            index[0] = MAPS_WorldClock_Timezone_Singapore;
            index[1] = MAPS_WorldClock_Timezone_Seoul;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Tokyo)
          {
            index[0] = MAPS_WorldClock_Timezone_Seoul;
            index[1] = MAPS_WorldClock_Timezone_Tokyo;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Sydney)
          {
            index[0] = MAPS_WorldClock_Timezone_Tokyo;
            index[1] = MAPS_WorldClock_Timezone_Sydney;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_SanFrancisco)
          {
            index[0] = MAPS_WorldClock_Timezone_Sydney;
            index[1] = MAPS_WorldClock_Timezone_SanFrancisco;
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_NewYork)
          {
            index[0] = MAPS_WorldClock_Timezone_SanFrancisco;
            index[1] = MAPS_WorldClock_Timezone_NewYork;
          }
        }
        Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
        Animation_Screen_Switch_Horizontal_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 1, 0, 5);
      }
      // Check current menu selection is configure adjust
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_Configure_Adjust)
      {
        if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_List)
        {
          int index[2] = {MAPS_ConfigureAdjust_Clock, MAPS_ConfigureAdjust_Date};
          index[0] = Lyra_ConfigureAdjust_List_Cursor;
          index[1] = Lyra_ConfigureAdjust_List_Cursor + 1;
          Lyra_ConfigureAdjust_List_Cursor++; // Move configure adjust list cursor down
          if (Lyra_ConfigureAdjust_List_Cursor > MAPS_ConfigureAdjust_Max - 1)
          {
            Lyra_ConfigureAdjust_List_Cursor = MAPS_ConfigureAdjust_Max - 1; // Reset to maximum configure adjust list cursor position
            index[0] = Lyra_ConfigureAdjust_List_Cursor;
            index[1] = Lyra_ConfigureAdjust_List_Cursor;
          }
          Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
          Animation_Screen_Switch_Vertical_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 1, 0, 5);
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Clock)
        {
          Lyra_ConfigureAdjust_Clock_Cursor++;
          if (Lyra_ConfigureAdjust_Clock_Cursor >= 6)
          {
            Lyra_ConfigureAdjust_Clock_Cursor = 0; // Reset to maximum configure adjust clock edit cursor position
          }
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Date)
        {
          Lyra_ConfigureAdjust_Date_Cursor++;
          if (Lyra_ConfigureAdjust_Date_Cursor >= 8)
          {
            Lyra_ConfigureAdjust_Date_Cursor = 0; // Reset to maximum configure adjust date edit cursor position
          }
        }
        else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Tense)
        {
          int index[2] = {MAPS_ConfigureAdjust_Tense_24H, MAPS_ConfigureAdjust_Tense_12H};
          index[0] = Lyra_ConfigureAdjust_Tense_Format;
          index[1] = Lyra_ConfigureAdjust_Tense_Format + 1;
          Lyra_ConfigureAdjust_Tense_Format++; // Switch tense format
          if (Lyra_ConfigureAdjust_Tense_Format > MAPS_ConfigureAdjust_Tense_12H)
          {
            Lyra_ConfigureAdjust_Tense_Format = MAPS_ConfigureAdjust_Tense_12H; // Reset to maximum tense format
            index[0] = Lyra_ConfigureAdjust_Tense_Format;
            index[1] = Lyra_ConfigureAdjust_Tense_Format;
          }
          Refresh_Dynamic_Animation_Cache(Lyra_Dynamic_Cache, ARRAY_LENGTH(Lyra_Dynamic_Cache), MAPS_Menu_SelectionN[Lyra_Menu_Selection], index);
          Animation_Screen_Switch_Vertical_Scroll_Array(Lyra_Dynamic_Cache[0].coord, Lyra_Dynamic_Cache[0].length, Lyra_Dynamic_Cache[1].coord, Lyra_Dynamic_Cache[1].length, 1, 0, 5);
        }
      }
      MAPS_Dock_KEY_Delay(100); // Button delay 500ms
    }
    // Display the current menu selection
    switch (MAPS_Menu_SelectionN[Lyra_Menu_Selection])
    {
    case MAPS_Menu_Clock:
      if (Lyra_Clock_Style == MAPS_Clock_Dial)
      {
        Render_Clock_Current_Time_Dial(LCM_Clock_Dial_coordinate, LCM_Clock_Dial_coordinate_length, RTC_Time_Now.Hour, RTC_Time_Now.Minute, RTC_Time_Now.Second);
      }
      else if (Lyra_Clock_Style == MAPS_Clock_Digit)
      {
        Render_Clock_Current_Time_Digit(LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      break;
    case MAPS_Menu_StopWatch:
      if (Lyra_StopWatch_Style == MAPS_StopWatch_Dial)
      {
        Render_Stop_Watch_Current_Time_Dial(LCM_StopWatch_Dial_coordinate, LCM_StopWatch_Dial_coordinate_length, Stop_Watch_Now.Minute, Stop_Watch_Now.Second, Stop_Watch_Now.Centisecond);
      }
      else if (Lyra_StopWatch_Style == MAPS_StopWatch_Digit)
      {
        Render_Stop_Watch_Current_Time_Digit(LCM_StopWatch_Digit_coordinate, LCM_StopWatch_Digit_coordinate_length, Stop_Watch_Now.Minute, Stop_Watch_Now.Second, Stop_Watch_Now.Centisecond);
      }
      break;
    case MAPS_Menu_AlarmClock:
      if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_List)
      {
        Get_Alarm_Clock_Time_From_List(Lyra_AlarmClock_List_Cursor, &Lyra_AlarmClock_List_Time_Hour, &Lyra_AlarmClock_List_Time_Minute);
        Render_Alarm_Clock_List_Mode_Time_Digit(LCM_AlarmClock_Digit_coordinate, LCM_AlarmClock_Digit_coordinate_length, Lyra_AlarmClock_List_Time_Hour, Lyra_AlarmClock_List_Time_Minute, Lyra_AlarmClock_List_Cursor + 1);
      }
      else if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_Edit)
      {
        Render_Alarm_Clock_Edit_Mode_Time_Digit(LCM_AlarmClock_Digit_coordinate, LCM_AlarmClock_Digit_coordinate_length, Lyra_AlarmClock_Edit_Number[0] * 10 + Lyra_AlarmClock_Edit_Number[1], Lyra_AlarmClock_Edit_Number[2] * 10 + Lyra_AlarmClock_Edit_Number[3], Lyra_AlarmClock_List_Cursor + 1, Lyra_AlarmClock_Edit_Cursor);
      }
      break;
    case MAPS_Menu_WorldClock:
      if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Beijing)
      {
        Render_World_Clock_Time(LCM_WorldClock_beijing_coordinate, LCM_WorldClock_beijing_coordinate_length, MAPS_WorldClock_Timezone_Array[0], RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Shanghai)
      {
        Render_World_Clock_Time(LCM_WorldClock_shanghai_coordinate, LCM_WorldClock_shanghai_coordinate_length, MAPS_WorldClock_Timezone_Array[1], RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Hongkong)
      {
        Render_World_Clock_Time(LCM_WorldClock_hongkong_coordinate, LCM_WorldClock_hongkong_coordinate_length, MAPS_WorldClock_Timezone_Array[2], RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Taipei)
      {
        Render_World_Clock_Time(LCM_WorldClock_taipei_coordinate, LCM_WorldClock_taipei_coordinate_length, MAPS_WorldClock_Timezone_Array[3], RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Singapore)
      {
        Render_World_Clock_Time(LCM_WorldClock_singapore_coordinate, LCM_WorldClock_singapore_coordinate_length, MAPS_WorldClock_Timezone_Array[4], RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Seoul)
      {
        Render_World_Clock_Time(LCM_WorldClock_seoul_coordinate, LCM_WorldClock_seoul_coordinate_length, MAPS_WorldClock_Timezone_Array[5], RTC_Time_Now.Hour + 1, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Tokyo)
      {
        Render_World_Clock_Time(LCM_WorldClock_tokyo_coordinate, LCM_WorldClock_tokyo_coordinate_length, MAPS_WorldClock_Timezone_Array[6], RTC_Time_Now.Hour + 1, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Sydney)
      {
        Render_World_Clock_Time(LCM_WorldClock_sydney_coordinate, LCM_WorldClock_sydney_coordinate_length, MAPS_WorldClock_Timezone_Array[7], RTC_Time_Now.Hour + 2, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_SanFrancisco)
      {
        Render_World_Clock_Time(LCM_WorldClock_sanfrancisco_coordinate, LCM_WorldClock_sanfrancisco_coordinate_length, MAPS_WorldClock_Timezone_Array[8], RTC_Time_Now.Hour - 16, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_NewYork)
      {
        Render_World_Clock_Time(LCM_WorldClock_newyork_coordinate, LCM_WorldClock_newyork_coordinate_length, MAPS_WorldClock_Timezone_Array[9], RTC_Time_Now.Hour - 13, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
      }
      break;
    case MAPS_Menu_Configure_Adjust:
      if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_List)
      {
        if (Lyra_ConfigureAdjust_List_Cursor == MAPS_ConfigureAdjust_Clock)
        {
          Render_Configure_Adjust_List_Mode_Item(LCM_ConfigureAdjust_Clock_icon_coordinate, LCM_ConfigureAdjust_Clock_icon_coordinate_length);
        }
        else if (Lyra_ConfigureAdjust_List_Cursor == MAPS_ConfigureAdjust_Date)
        {
          Render_Configure_Adjust_List_Mode_Item(LCM_ConfigureAdjust_Date_icon_coordinate, LCM_ConfigureAdjust_Date_icon_coordinate_length);
        }
        else if (Lyra_ConfigureAdjust_List_Cursor == MAPS_ConfigureAdjust_Tense)
        {
          Render_Configure_Adjust_List_Mode_Item(LCM_ConfigureAdjust_Tense_icon_coordinate, LCM_ConfigureAdjust_Tense_icon_coordinate_length);
        }
      }
      else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Clock)
      {
        Render_Configure_Adjust_Clock_Digit(LCM_ConfigureAdjust_Clock_Digit_coordinate, LCM_ConfigureAdjust_Clock_Digit_coordinate_length, Lyra_ConfigureAdjust_Clock_Number[0] * 10 + Lyra_ConfigureAdjust_Clock_Number[1], Lyra_ConfigureAdjust_Clock_Number[2] * 10 + Lyra_ConfigureAdjust_Clock_Number[3], Lyra_ConfigureAdjust_Clock_Number[4] * 10 + Lyra_ConfigureAdjust_Clock_Number[5], Lyra_ConfigureAdjust_Clock_Cursor);
      }
      else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Date)
      {
        Render_Configure_Adjust_Date_Digit(LCM_ConfigureAdjust_Date_Digit_coordinate, LCM_ConfigureAdjust_Date_Digit_coordinate_length, Lyra_ConfigureAdjust_Date_Number[0] * 1000 + Lyra_ConfigureAdjust_Date_Number[1] * 100 + Lyra_ConfigureAdjust_Date_Number[2] * 10 + Lyra_ConfigureAdjust_Date_Number[3], Lyra_ConfigureAdjust_Date_Number[4] * 10 + Lyra_ConfigureAdjust_Date_Number[5], Lyra_ConfigureAdjust_Date_Number[6] * 10 + Lyra_ConfigureAdjust_Date_Number[7], Lyra_ConfigureAdjust_Date_Cursor);
      }
      else if (Lyra_ConfigureAdjust_Mode == MAPS_ConfigureAdjust_Tense)
      {
        if (Lyra_ConfigureAdjust_Tense_Format == MAPS_ConfigureAdjust_Tense_24H)
        {
          Render_Configure_Adjust_Tense_Digit(LCM_ConfigureAdjust_Tense_24_icon_coordinate, LCM_ConfigureAdjust_Tense_24_icon_coordinate_length);
        }
        else
        {
          Render_Configure_Adjust_Tense_Digit(LCM_ConfigureAdjust_Tense_12_icon_coordinate, LCM_ConfigureAdjust_Tense_12_icon_coordinate_length);
        }
      }
      break;
    default:
      break;
    }
  }
}

/*
 *  @brief      Calc_Dynamic_Animation_Cache_Array
 *              Convert a 64x16 bitmap to a Coord array stored in static memory.
 *              No malloc/free - uses two static ping-pong buffers.
 */
void Calc_Dynamic_Animation_Cache_Array(Coord **buff, int *buffSize, const uint8 array[64][16])
{
  // Two static ping-pong buffers, each large enough for a full 128x64 frame
  static Coord static_buf[2][COORD_CACHE_MAX_SIZE];
  static int buf_index = 0;

  // Alternate between the two buffers so both CoordCache slots stay valid
  // during the animation (one holds src, the other holds dst)
  buf_index = (buf_index + 1) % 2;
  Coord *buf = static_buf[buf_index];
  int counter = 0;

  // Directly scan bitmap and fill Coord array - no linked list, no malloc
  for (int i = 0; i < 64 && counter < COORD_CACHE_MAX_SIZE; i++)
  {
    for (int j = 0; j < 16 && counter < COORD_CACHE_MAX_SIZE; j++)
    {
      for (int k = 0; k < 8 && counter < COORD_CACHE_MAX_SIZE; k++)
      {
        if (array[i][j] & (0x01 << (7 - k)))
        {
          buf[counter].x = (uint8)(j * 8 + k);
          buf[counter].y = (uint8)i;
          counter++;
        }
      }
    }
  }

  *buff     = buf;
  *buffSize = counter;
}

/*
 *  @brief      Release_Dynamic_Animation_Cache
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @since      v1.0
 *  Sample usage:       Release_Dynamic_Animation_Cache(10,15,90);
 */
void Release_Dynamic_Animation_Cache(CoordCache *array, int len)
{
  // coord points to static storage - do NOT free, just clear the reference
  for (int i = 0; i < len; i++)
  {
    array[i].coord  = NULL;
    array[i].length = 0;
  }
}

/*
 *  @brief      Refresh_Dynamic_Animation_Cache
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @since      v1.0
 *  Sample usage:       Refresh_Dynamic_Animation_Cache(10,15,90);
 */
void Refresh_Dynamic_Animation_Cache(CoordCache *array, int len, int menu, int index[2])
{
  uint8 cache[64][16] = {0x00}; // 64 rows, 128 columns
  // release dynamic animation cache
  Release_Dynamic_Animation_Cache(array, len);
  // index cannot have the same value
  if (index[0] == index[1])
  {
    return;
  }
  // load dynamic animation cache according to menu and source and destination index
  switch (menu)
  {
  case MAPS_Menu_Clock:
    for (int i = 0; i < len; i++)
    {
      switch (index[i])
      {
      case MAPS_Clock_Dial:
        Calc_Clock_Current_Time_Dial((uint8 *)cache, LCM_Clock_Dial_coordinate, LCM_Clock_Dial_coordinate_length, RTC_Time_Now.Hour, RTC_Time_Now.Minute, RTC_Time_Now.Second);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_Clock_Digit:
        Calc_Clock_Current_Time_Digit((uint8 *)cache, LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      default:
        break;
      }
    }
    break;
  case MAPS_Menu_StopWatch:
    for (int i = 0; i < len; i++)
    {
      switch (index[i])
      {
      case MAPS_StopWatch_Dial:
        Calc_Stop_Watch_Current_Time_Dial((uint8 *)cache, LCM_StopWatch_Dial_coordinate, LCM_StopWatch_Dial_coordinate_length, Stop_Watch_Now.Minute, Stop_Watch_Now.Second, Stop_Watch_Now.Centisecond);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_StopWatch_Digit:
        Calc_Stop_Watch_Current_Time_Digit((uint8 *)cache, LCM_StopWatch_Digit_coordinate, LCM_StopWatch_Digit_coordinate_length, Stop_Watch_Now.Minute, Stop_Watch_Now.Second, Stop_Watch_Now.Centisecond);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      default:
        break;
      }
    }
    break;
  case MAPS_Menu_AlarmClock:
    for (int i = 0; i < len; i++)
    {
      int hour = 0;
      int minute = 0;
      Get_Alarm_Clock_Time_From_List(index[i], &hour, &minute);
      Calc_Alarm_Clock_List_Mode_Time_Digit((uint8 *)cache, LCM_AlarmClock_Digit_coordinate, LCM_AlarmClock_Digit_coordinate_length, hour, minute, index[i] + 1);
      Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
    }
    break;
  case MAPS_Menu_WorldClock:
    for (int i = 0; i < len; i++)
    {
      switch (index[i])
      {
      case MAPS_WorldClock_Timezone_Beijing:
        Calc_World_Clock_Time((uint8 *)cache, LCM_WorldClock_beijing_coordinate, LCM_WorldClock_beijing_coordinate_length, MAPS_WorldClock_Timezone_Array[0], RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_WorldClock_Timezone_Shanghai:
        Calc_World_Clock_Time((uint8 *)cache, LCM_WorldClock_shanghai_coordinate, LCM_WorldClock_shanghai_coordinate_length, MAPS_WorldClock_Timezone_Array[1], RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_WorldClock_Timezone_Hongkong:
        Calc_World_Clock_Time((uint8 *)cache, LCM_WorldClock_hongkong_coordinate, LCM_WorldClock_hongkong_coordinate_length, MAPS_WorldClock_Timezone_Array[2], RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_WorldClock_Timezone_Taipei:
        Calc_World_Clock_Time((uint8 *)cache, LCM_WorldClock_taipei_coordinate, LCM_WorldClock_taipei_coordinate_length, MAPS_WorldClock_Timezone_Array[3], RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_WorldClock_Timezone_Singapore:
        Calc_World_Clock_Time((uint8 *)cache, LCM_WorldClock_singapore_coordinate, LCM_WorldClock_singapore_coordinate_length, MAPS_WorldClock_Timezone_Array[4], RTC_Time_Now.Hour, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_WorldClock_Timezone_Seoul:
        Calc_World_Clock_Time((uint8 *)cache, LCM_WorldClock_seoul_coordinate, LCM_WorldClock_seoul_coordinate_length, MAPS_WorldClock_Timezone_Array[5], RTC_Time_Now.Hour + 1, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_WorldClock_Timezone_Tokyo:
        Calc_World_Clock_Time((uint8 *)cache, LCM_WorldClock_tokyo_coordinate, LCM_WorldClock_tokyo_coordinate_length, MAPS_WorldClock_Timezone_Array[6], RTC_Time_Now.Hour + 1, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_WorldClock_Timezone_Sydney:
        Calc_World_Clock_Time((uint8 *)cache, LCM_WorldClock_sydney_coordinate, LCM_WorldClock_sydney_coordinate_length, MAPS_WorldClock_Timezone_Array[7], RTC_Time_Now.Hour + 2, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_WorldClock_Timezone_SanFrancisco:
        Calc_World_Clock_Time((uint8 *)cache, LCM_WorldClock_sanfrancisco_coordinate, LCM_WorldClock_sanfrancisco_coordinate_length, MAPS_WorldClock_Timezone_Array[8], RTC_Time_Now.Hour - 16, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_WorldClock_Timezone_NewYork:
        Calc_World_Clock_Time((uint8 *)cache, LCM_WorldClock_newyork_coordinate, LCM_WorldClock_newyork_coordinate_length, MAPS_WorldClock_Timezone_Array[9], RTC_Time_Now.Hour - 13, RTC_Time_Now.Minute, Lyra_ConfigureAdjust_Tense_Format);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      default:
        break;
      }
    }
    break;
  case MAPS_Menu_Configure_Adjust:
    for (int i = 0; i < len; i++)
    {
      switch (index[i])
      {
      case MAPS_ConfigureAdjust_Clock:
        Calc_Configure_Adjust_List_Mode_Item((uint8 *)cache, LCM_ConfigureAdjust_Clock_icon_coordinate, LCM_ConfigureAdjust_Clock_icon_coordinate_length);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_ConfigureAdjust_Date:
        Calc_Configure_Adjust_List_Mode_Item((uint8 *)cache, LCM_ConfigureAdjust_Date_icon_coordinate, LCM_ConfigureAdjust_Date_icon_coordinate_length);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_ConfigureAdjust_Tense:
        Calc_Configure_Adjust_List_Mode_Item((uint8 *)cache, LCM_ConfigureAdjust_Tense_icon_coordinate, LCM_ConfigureAdjust_Tense_icon_coordinate_length);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_ConfigureAdjust_Tense_24H:
        Calc_Configure_Adjust_List_Mode_Item((uint8 *)cache, LCM_ConfigureAdjust_Tense_24_icon_coordinate, LCM_ConfigureAdjust_Tense_24_icon_coordinate_length);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      case MAPS_ConfigureAdjust_Tense_12H:
        Calc_Configure_Adjust_List_Mode_Item((uint8 *)cache, LCM_ConfigureAdjust_Tense_12_icon_coordinate, LCM_ConfigureAdjust_Tense_12_icon_coordinate_length);
        Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
        break;
      default:
        break;
      }
    }
    break;
  default:
    break;
  }
}