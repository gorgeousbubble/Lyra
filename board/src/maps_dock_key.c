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
#include "uart.h"
#include "watch.h"

/*
**Independent button port
*/
PTXn MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY_MAX] = {PTB21, PTB22, PTB23, PTB20};

int Lyra_Status = 0;                      // Screen status
int Lyra_Menu_Selection = 0;              // Menu selection
int Lyra_Clock_Style = 0;                 // Clock style
int Lyra_StopWatch_Style = 0;             // Stop Watch style
int Lyra_AlarmClock_Mode = 0;             // Alarm Clock mode
int Lyra_AlarmClock_List_Cursor = 0;      // Alarm Clock list cursor
int Lyra_AlarmClock_List_Time_Hour = 0;   // Alarm Clock list time hour
int Lyra_AlarmClock_List_Time_Minute = 0; // Alarm Clock list time minute
int Lyra_AlarmClock_Edit_Cursor = 0;      // Alarm Clock edit cursor
int Lyra_AlarmClock_Edit_Number[4] = {0}; // Alarm Clock edit number (hh:mm)
int Lyra_WorldClock_Time_Cursor = 0;  // World Clock time cursor

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
          Lyra_AlarmClock_Edit_Number[0] = hour / 10; // Hour tens
          Lyra_AlarmClock_Edit_Number[1] = hour % 10; // Hour
          Lyra_AlarmClock_Edit_Number[2] = minute / 10; // Minute tens
          Lyra_AlarmClock_Edit_Number[3] = minute % 10; // Minute
          Lyra_AlarmClock_Edit_Cursor = 0; // Reset edit cursor  

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
      // check current menu selection is world clock
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
        Render_Clock_Current_Time_Digit(LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, RTC_Time_Now.Hour, RTC_Time_Now.Minute);
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
        Render_Alarm_Clock_List_Mode_Time_Digit(LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, Lyra_AlarmClock_List_Time_Hour, Lyra_AlarmClock_List_Time_Minute, Lyra_AlarmClock_List_Cursor + 1);
      }
      else if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_Edit)
      {
        Render_Alarm_Clock_Edit_Mode_Time_Digit(LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, Lyra_AlarmClock_Edit_Number[0] * 10 + Lyra_AlarmClock_Edit_Number[1], Lyra_AlarmClock_Edit_Number[2] * 10 + Lyra_AlarmClock_Edit_Number[3], Lyra_AlarmClock_List_Cursor + 1, Lyra_AlarmClock_Edit_Cursor);
      }
      break;
    case MAPS_Menu_WorldClock:
      if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Beijing)
      {
        Render_World_Clock_Time(LCM_WorldClock_beijing_coordinate, LCM_WorldClock_beijing_coordinate_length, MAPS_WorldClock_Timezone_Array[0], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Shanghai)
      {
        Render_World_Clock_Time(LCM_WorldClock_shanghai_coordinate, LCM_WorldClock_shanghai_coordinate_length, MAPS_WorldClock_Timezone_Array[1], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Hongkong)
      {
        Render_World_Clock_Time(LCM_WorldClock_hongkong_coordinate, LCM_WorldClock_hongkong_coordinate_length, MAPS_WorldClock_Timezone_Array[2], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Taipei)
      {
        Render_World_Clock_Time(LCM_WorldClock_taipei_coordinate, LCM_WorldClock_taipei_coordinate_length, MAPS_WorldClock_Timezone_Array[3], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Singapore)
      {
        Render_World_Clock_Time(LCM_WorldClock_singapore_coordinate, LCM_WorldClock_singapore_coordinate_length, MAPS_WorldClock_Timezone_Array[4], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Seoul)
      {
        Render_World_Clock_Time(LCM_WorldClock_seoul_coordinate, LCM_WorldClock_seoul_coordinate_length, MAPS_WorldClock_Timezone_Array[5], RTC_Time_Now.Hour + 1, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Tokyo)
      {
        Render_World_Clock_Time(LCM_WorldClock_tokyo_coordinate, LCM_WorldClock_tokyo_coordinate_length, MAPS_WorldClock_Timezone_Array[6], RTC_Time_Now.Hour + 1, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Sydney)
      {
        Render_World_Clock_Time(LCM_WorldClock_sydney_coordinate, LCM_WorldClock_sydney_coordinate_length, MAPS_WorldClock_Timezone_Array[7], RTC_Time_Now.Hour + 2, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_SanFrancisco)
      {
        Render_World_Clock_Time(LCM_WorldClock_sanfrancisco_coordinate, LCM_WorldClock_sanfrancisco_coordinate_length, MAPS_WorldClock_Timezone_Array[8], RTC_Time_Now.Hour - 16, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_NewYork)
      {
        Render_World_Clock_Time(LCM_WorldClock_newyork_coordinate, LCM_WorldClock_newyork_coordinate_length, MAPS_WorldClock_Timezone_Array[9], RTC_Time_Now.Hour - 13, RTC_Time_Now.Minute);
      }
      break;
    default:
      break;
    }
  }
}

/*
 *  @brief      Calc_Dynamic_Animation_Cache_Array
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @since      v1.0
 *  Sample usage:       Calc_Dynamic_Animation_Cache_Array(10,15,90);
*/
void Calc_Dynamic_Animation_Cache_Array(Coord** buff, int* buffSize, const uint8 array[64][16])
{
  CoordNode* head = NULL;
  CoordNode* current = NULL;
  int counter = 0;
  // if buff is not NULL free the memory
  if (*buff != NULL)
  {
    free(*buff);
    *buff = NULL;
    *buffSize = 0;
  }
  // conver array to coordinate array
  for (int i = 0; i < 64; i++)
  {
    for (int j = 0; j < 16; j++)
    {
      for (int k = 0; k < 8; k++)
      {
        if (array[i][j] & (0x01 << (7 - k)))
        {
          // if the pixel is set, add to coordinate array
          uint8 x = j * 8 + k;
          uint8 y = i;
          // create a new coordinate node for the pixel
          CoordNode* node = (CoordNode*)malloc(sizeof(CoordNode));
          node->x = x;
          node->y = y;
          node->next = NULL;
          // link the coordinate node to the list
          if (head == NULL) {
              head = node; // if head is NULL, set head to the coordinate node
              current = head; // move current to the coordinate node
          } else {
              current->next = node; // link the coordinate node to the list
              current = node; // move current to the coordinate node
          }
          // increment counter
          counter++;
        }
      }
    }
  }
  // return the coordinate array
  *buffSize = counter;
  *buff = (Coord*)malloc(sizeof(Coord) * counter);
  int index = 0;
  for (current = head; current != NULL; current = current->next) 
  {
    (*buff)[index].x = current->x;
    (*buff)[index].y = current->y;
    index++;
  }
  // free the linked list
  while (head != NULL) 
  {
    CoordNode* temp = head;
    head = head->next;
    free(temp);
  }
}

/*
 *  @brief      Release_Dynamic_Animation_Cache
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @since      v1.0
 *  Sample usage:       Release_Dynamic_Animation_Cache(10,15,90);
*/
void Release_Dynamic_Animation_Cache(CoordCache* array, int len)
{
  for (int i = 0; i < len; i++)
  {
    // free dynamic animation cache
    if (array[i].coord != NULL)
    {
      free(array[i].coord);
      array[i].coord = NULL;
      array[i].length = 0;
    }
  }
}

/*
 *  @brief      Refresh_Dynamic_Animation_Cache
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @since      v1.0
 *  Sample usage:       Refresh_Dynamic_Animation_Cache(10,15,90);
*/
void Refresh_Dynamic_Animation_Cache(CoordCache* array, int len, int menu, int index[2])
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
          Calc_Clock_Current_Time_Dial((uint8*)cache, LCM_Clock_Dial_coordinate, LCM_Clock_Dial_coordinate_length, RTC_Time_Now.Hour, RTC_Time_Now.Minute, RTC_Time_Now.Second);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_Clock_Digit:
          Calc_Clock_Current_Time_Digit((uint8*)cache, LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, RTC_Time_Now.Hour, RTC_Time_Now.Minute);
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
          Calc_Stop_Watch_Current_Time_Dial((uint8*)cache, LCM_StopWatch_Dial_coordinate, LCM_StopWatch_Dial_coordinate_length, Stop_Watch_Now.Minute, Stop_Watch_Now.Second, Stop_Watch_Now.Centisecond);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_StopWatch_Digit:
          Calc_Stop_Watch_Current_Time_Digit((uint8*)cache, LCM_StopWatch_Digit_coordinate, LCM_StopWatch_Digit_coordinate_length, Stop_Watch_Now.Minute, Stop_Watch_Now.Second, Stop_Watch_Now.Centisecond);
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
      Calc_Alarm_Clock_List_Mode_Time_Digit((uint8*)cache, LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, hour, minute, index[i] + 1);
      Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
    }
    break;
  case MAPS_Menu_WorldClock:
    for (int i = 0; i < len; i++)
    {
      switch (index[i])
      {
        case MAPS_WorldClock_Timezone_Beijing:
          Calc_World_Clock_Time((uint8*)cache, LCM_WorldClock_beijing_coordinate, LCM_WorldClock_beijing_coordinate_length, MAPS_WorldClock_Timezone_Array[0], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_WorldClock_Timezone_Shanghai:
          Calc_World_Clock_Time((uint8*)cache, LCM_WorldClock_shanghai_coordinate, LCM_WorldClock_shanghai_coordinate_length, MAPS_WorldClock_Timezone_Array[1], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_WorldClock_Timezone_Hongkong:
          Calc_World_Clock_Time((uint8*)cache, LCM_WorldClock_hongkong_coordinate, LCM_WorldClock_hongkong_coordinate_length, MAPS_WorldClock_Timezone_Array[2], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_WorldClock_Timezone_Taipei:
          Calc_World_Clock_Time((uint8*)cache, LCM_WorldClock_taipei_coordinate, LCM_WorldClock_taipei_coordinate_length, MAPS_WorldClock_Timezone_Array[3], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_WorldClock_Timezone_Singapore:
          Calc_World_Clock_Time((uint8*)cache, LCM_WorldClock_singapore_coordinate, LCM_WorldClock_singapore_coordinate_length, MAPS_WorldClock_Timezone_Array[4], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_WorldClock_Timezone_Seoul:
          Calc_World_Clock_Time((uint8*)cache, LCM_WorldClock_seoul_coordinate, LCM_WorldClock_seoul_coordinate_length, MAPS_WorldClock_Timezone_Array[5], RTC_Time_Now.Hour + 1, RTC_Time_Now.Minute);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_WorldClock_Timezone_Tokyo:
          Calc_World_Clock_Time((uint8*)cache, LCM_WorldClock_tokyo_coordinate, LCM_WorldClock_tokyo_coordinate_length, MAPS_WorldClock_Timezone_Array[6], RTC_Time_Now.Hour + 1, RTC_Time_Now.Minute);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_WorldClock_Timezone_Sydney:
          Calc_World_Clock_Time((uint8*)cache, LCM_WorldClock_sydney_coordinate, LCM_WorldClock_sydney_coordinate_length, MAPS_WorldClock_Timezone_Array[7], RTC_Time_Now.Hour + 2, RTC_Time_Now.Minute);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_WorldClock_Timezone_SanFrancisco:
          Calc_World_Clock_Time((uint8*)cache, LCM_WorldClock_sanfrancisco_coordinate, LCM_WorldClock_sanfrancisco_coordinate_length, MAPS_WorldClock_Timezone_Array[8], RTC_Time_Now.Hour - 16, RTC_Time_Now.Minute);
          Calc_Dynamic_Animation_Cache_Array(&(array[i].coord), &(array[i].length), cache);
          break;
        case MAPS_WorldClock_Timezone_NewYork:
          Calc_World_Clock_Time((uint8*)cache, LCM_WorldClock_newyork_coordinate, LCM_WorldClock_newyork_coordinate_length, MAPS_WorldClock_Timezone_Array[9], RTC_Time_Now.Hour - 13, RTC_Time_Now.Minute);
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