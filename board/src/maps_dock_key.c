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
int Lyra_AlarmClock_Edit_Cursor = 0;      // Alarm Clock edit cursor
int Lyra_AlarmClock_Edit_Number[4] = {0}; // Alarm Clock edit number (hh:mm)
int Lyra_WorldClock_Time_Cursor = 0;  // World Clock time cursor

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
        Lyra_Clock_Style--; // Switch clock style
        if (Lyra_Clock_Style < 0)
        {
          Lyra_Clock_Style = MAPS_Clock_Style_Max - 1; // Reset to maximum clock style
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Clock_Dial_Buf, LCM_Clock_Dial_Buf_Size, LCM_Clock_Digit_Buf, LCM_Clock_Digit_Buf_Size, 0, 0, 5);
          //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Clock_Dial_coordinate, LCM_Clock_Dial_coordinate_length, LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, 0, 0, 5);
        } else {
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Clock_Digit_Buf, LCM_Clock_Digit_Buf_Size, LCM_Clock_Dial_Buf, LCM_Clock_Dial_Buf_Size, 0, 0, 5);
          //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, LCM_Clock_Dial_coordinate, LCM_Clock_Dial_coordinate_length, 0, 0, 5);
        }
      }
      // Check current menu selection is stopwatch
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_StopWatch)
      {
        Lyra_StopWatch_Style--; // Switch stopwatch style
        if (Lyra_StopWatch_Style < 0)
        {
          Lyra_StopWatch_Style = MAPS_StopWatch_Style_Max - 1; // Reset to maximum stopwatch style
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_StopWatch_Dial_Buf, LCM_StopWatch_Dial_Buf_Size, LCM_StopWatch_Digit_Buf, LCM_StopWatch_Digit_Buf_Size, 0, 0, 5);
          //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_StopWatch_Dial_coordinate, LCM_StopWatch_Dial_coordinate_length, LCM_StopWatch_Digit_coordinate, LCM_StopWatch_Digit_coordinate_length, 0, 0, 5);
        } else {
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_StopWatch_Digit_Buf, LCM_StopWatch_Digit_Buf_Size, LCM_StopWatch_Dial_Buf, LCM_StopWatch_Dial_Buf_Size, 0, 0, 5);
          //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_StopWatch_Digit_coordinate, LCM_StopWatch_Digit_coordinate_length, LCM_StopWatch_Dial_coordinate, LCM_StopWatch_Dial_coordinate_length, 0, 0, 5);
        }
      }
      // Check current menu selection is alarm clock
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_AlarmClock)
      {
        if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_List)
        {
          Lyra_AlarmClock_List_Cursor++; // Move alarm clock list cursor down
          if (Lyra_AlarmClock_List_Cursor >= Get_Alarm_Clock_List_Len())
          {
            Lyra_AlarmClock_List_Cursor = Get_Alarm_Clock_List_Len(); // Reset to minimum alarm clock list cursor position
          }
          if (Lyra_AlarmClock_List_Cursor >= Alarm_Clock_Max_Len)
          {
            Lyra_AlarmClock_List_Cursor = Alarm_Clock_Max_Len - 1; // Reset to maximum alarm clock list cursor position
          }
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
        Lyra_WorldClock_Time_Cursor--; // Move world clock time cursor up
        if (Lyra_WorldClock_Time_Cursor < 0)
        {
          Lyra_WorldClock_Time_Cursor = MAPS_WorldClock_Timezone_Max - 1; // Reset to minimum world clock time cursor position
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_beijing_Buf, LCM_WorldClock_beijing_Buf_Size, LCM_WorldClock_newyork_Buf, LCM_WorldClock_newyork_Buf_Size, 0, 0, 5);
          //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_beijing_coordinate, LCM_WorldClock_beijing_coordinate_length, LCM_WorldClock_newyork_coordinate, LCM_WorldClock_newyork_coordinate_length, 0, 0, 5);
        } else {
          if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Beijing) {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_shanghai_Buf, LCM_WorldClock_shanghai_Buf_Size, LCM_WorldClock_beijing_Buf, LCM_WorldClock_beijing_Buf_Size, 0, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_shanghai_coordinate, LCM_WorldClock_shanghai_coordinate_length, LCM_WorldClock_beijing_coordinate, LCM_WorldClock_beijing_coordinate_length, 0, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Shanghai)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_hongkong_Buf, LCM_WorldClock_hongkong_Buf_Size, LCM_WorldClock_shanghai_Buf, LCM_WorldClock_shanghai_Buf_Size, 0, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_hongkong_coordinate, LCM_WorldClock_hongkong_coordinate_length, LCM_WorldClock_shanghai_coordinate, LCM_WorldClock_shanghai_coordinate_length, 0, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Hongkong)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_taipei_Buf, LCM_WorldClock_taipei_Buf_Size, LCM_WorldClock_hongkong_Buf, LCM_WorldClock_hongkong_Buf_Size, 0, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_taipei_coordinate, LCM_WorldClock_taipei_coordinate_length, LCM_WorldClock_hongkong_coordinate, LCM_WorldClock_hongkong_coordinate_length, 0, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Taipei)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_singapore_Buf, LCM_WorldClock_singapore_Buf_Size, LCM_WorldClock_taipei_Buf, LCM_WorldClock_taipei_Buf_Size, 0, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_singapore_coordinate, LCM_WorldClock_singapore_coordinate_length, LCM_WorldClock_taipei_coordinate, LCM_WorldClock_taipei_coordinate_length, 0, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Singapore)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_seoul_Buf, LCM_WorldClock_seoul_Buf_Size, LCM_WorldClock_singapore_Buf, LCM_WorldClock_singapore_Buf_Size, 0, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_seoul_coordinate, LCM_WorldClock_seoul_coordinate_length, LCM_WorldClock_singapore_coordinate, LCM_WorldClock_singapore_coordinate_length, 0, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Seoul)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_tokyo_Buf, LCM_WorldClock_tokyo_Buf_Size, LCM_WorldClock_seoul_Buf, LCM_WorldClock_seoul_Buf_Size, 0, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_tokyo_coordinate, LCM_WorldClock_tokyo_coordinate_length, LCM_WorldClock_seoul_coordinate, LCM_WorldClock_seoul_coordinate_length, 0, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Tokyo)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_sydney_Buf, LCM_WorldClock_sydney_Buf_Size, LCM_WorldClock_tokyo_Buf, LCM_WorldClock_tokyo_Buf_Size, 0, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_sydney_coordinate, LCM_WorldClock_sydney_coordinate_length, LCM_WorldClock_tokyo_coordinate, LCM_WorldClock_tokyo_coordinate_length, 0, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Sydney)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_sanfrancisco_Buf, LCM_WorldClock_sanfrancisco_Buf_Size, LCM_WorldClock_sydney_Buf, LCM_WorldClock_sydney_Buf_Size, 0, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_sanfrancisco_coordinate, LCM_WorldClock_sanfrancisco_coordinate_length, LCM_WorldClock_sydney_coordinate, LCM_WorldClock_sydney_coordinate_length, 0, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_SanFrancisco)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_newyork_Buf, LCM_WorldClock_newyork_Buf_Size, LCM_WorldClock_sanfrancisco_Buf, LCM_WorldClock_sanfrancisco_Buf_Size, 0, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_newyork_coordinate, LCM_WorldClock_newyork_coordinate_length, LCM_WorldClock_sanfrancisco_coordinate, LCM_WorldClock_sanfrancisco_coordinate_length, 0, 0, 5);
          }
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
        Lyra_Clock_Style++; // Switch clock style
        if (Lyra_Clock_Style >= MAPS_Clock_Style_Max)
        {
          Lyra_Clock_Style = 0; // Reset to clock dial style
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Clock_Digit_Buf, LCM_Clock_Digit_Buf_Size, LCM_Clock_Dial_Buf, LCM_Clock_Dial_Buf_Size, 1, 0, 5);
          //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, LCM_Clock_Dial_coordinate, LCM_Clock_Dial_coordinate_length, 1, 0, 5);
        } else {
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Clock_Dial_Buf, LCM_Clock_Dial_Buf_Size, LCM_Clock_Digit_Buf, LCM_Clock_Digit_Buf_Size, 1, 0, 5);
          //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Clock_Dial_coordinate, LCM_Clock_Dial_coordinate_length, LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, 1, 0, 5);
        }
      }
      // Check current menu selection is stopwatch
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_StopWatch)
      {
        Lyra_StopWatch_Style++; // Switch stopwatch style
        if (Lyra_StopWatch_Style >= MAPS_StopWatch_Style_Max)
        {
          Lyra_StopWatch_Style = 0; // Reset to stopwatch dial style
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_StopWatch_Digit_Buf, LCM_StopWatch_Digit_Buf_Size, LCM_StopWatch_Dial_Buf, LCM_StopWatch_Dial_Buf_Size, 1, 0, 5);
          //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_StopWatch_Digit_coordinate, LCM_StopWatch_Digit_coordinate_length, LCM_StopWatch_Dial_coordinate, LCM_StopWatch_Dial_coordinate_length, 1, 0, 5);
        } else {
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_StopWatch_Dial_Buf, LCM_StopWatch_Dial_Buf_Size, LCM_StopWatch_Digit_Buf, LCM_StopWatch_Digit_Buf_Size, 1, 0, 5);
          //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_StopWatch_Dial_coordinate, LCM_StopWatch_Dial_coordinate_length, LCM_StopWatch_Digit_coordinate, LCM_StopWatch_Digit_coordinate_length, 1, 0, 5);
        }
      }
      // Check current menu selection is alarm clock
      if (MAPS_Menu_SelectionN[Lyra_Menu_Selection] == MAPS_Menu_AlarmClock)
      {
        if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_List)
        {
          Lyra_AlarmClock_List_Cursor--; // Move alarm clock list cursor up
          if (Lyra_AlarmClock_List_Cursor < 0)
          {
            Lyra_AlarmClock_List_Cursor = 0; // Reset to minimum alarm clock list cursor position
          }
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
        Lyra_WorldClock_Time_Cursor++; // Move world clock time cursor up
        if (Lyra_WorldClock_Time_Cursor >= MAPS_WorldClock_Timezone_Max)
        {
          Lyra_WorldClock_Time_Cursor = 0; // Reset to minimum world clock time cursor position
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_newyork_Buf, LCM_WorldClock_newyork_Buf_Size, LCM_WorldClock_beijing_Buf, LCM_WorldClock_beijing_Buf_Size, 1, 0, 5);
          //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_newyork_coordinate, LCM_WorldClock_newyork_coordinate_length, LCM_WorldClock_beijing_coordinate, LCM_WorldClock_beijing_coordinate_length, 1, 0, 5);
        } else {
          if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Shanghai) {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_beijing_Buf, LCM_WorldClock_beijing_Buf_Size, LCM_WorldClock_shanghai_Buf, LCM_WorldClock_shanghai_Buf_Size, 1, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_beijing_coordinate, LCM_WorldClock_beijing_coordinate_length, LCM_WorldClock_shanghai_coordinate, LCM_WorldClock_shanghai_coordinate_length, 1, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Hongkong)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_shanghai_Buf, LCM_WorldClock_shanghai_Buf_Size, LCM_WorldClock_hongkong_Buf, LCM_WorldClock_hongkong_Buf_Size, 1, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_shanghai_coordinate, LCM_WorldClock_shanghai_coordinate_length, LCM_WorldClock_hongkong_coordinate, LCM_WorldClock_hongkong_coordinate_length, 1, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Taipei)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_hongkong_Buf, LCM_WorldClock_hongkong_Buf_Size, LCM_WorldClock_taipei_Buf, LCM_WorldClock_taipei_Buf_Size, 1, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_hongkong_coordinate, LCM_WorldClock_hongkong_coordinate_length, LCM_WorldClock_taipei_coordinate, LCM_WorldClock_taipei_coordinate_length, 1, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Singapore)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_taipei_Buf, LCM_WorldClock_taipei_Buf_Size, LCM_WorldClock_singapore_Buf, LCM_WorldClock_singapore_Buf_Size, 1, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_taipei_coordinate, LCM_WorldClock_taipei_coordinate_length, LCM_WorldClock_singapore_coordinate, LCM_WorldClock_singapore_coordinate_length, 1, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Seoul)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_singapore_Buf, LCM_WorldClock_singapore_Buf_Size, LCM_WorldClock_seoul_Buf, LCM_WorldClock_seoul_Buf_Size, 1, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_singapore_coordinate, LCM_WorldClock_singapore_coordinate_length, LCM_WorldClock_seoul_coordinate, LCM_WorldClock_seoul_coordinate_length, 1, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Tokyo)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_seoul_Buf, LCM_WorldClock_seoul_Buf_Size, LCM_WorldClock_tokyo_Buf, LCM_WorldClock_tokyo_Buf_Size, 1, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_seoul_coordinate, LCM_WorldClock_seoul_coordinate_length, LCM_WorldClock_tokyo_coordinate, LCM_WorldClock_tokyo_coordinate_length, 1, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Sydney)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_tokyo_Buf, LCM_WorldClock_tokyo_Buf_Size, LCM_WorldClock_sydney_Buf, LCM_WorldClock_sydney_Buf_Size, 1, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_tokyo_coordinate, LCM_WorldClock_tokyo_coordinate_length, LCM_WorldClock_sydney_coordinate, LCM_WorldClock_sydney_coordinate_length, 1, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_SanFrancisco)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_sydney_Buf, LCM_WorldClock_sydney_Buf_Size, LCM_WorldClock_sanfrancisco_Buf, LCM_WorldClock_sanfrancisco_Buf_Size, 1, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_sydney_coordinate, LCM_WorldClock_sydney_coordinate_length, LCM_WorldClock_sanfrancisco_coordinate, LCM_WorldClock_sanfrancisco_coordinate_length, 1, 0, 5);
          }
          else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_NewYork)
          {
            Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_sanfrancisco_Buf, LCM_WorldClock_sanfrancisco_Buf_Size, LCM_WorldClock_newyork_Buf, LCM_WorldClock_newyork_Buf_Size, 1, 0, 5);
            //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_WorldClock_sanfrancisco_coordinate, LCM_WorldClock_sanfrancisco_coordinate_length, LCM_WorldClock_newyork_coordinate, LCM_WorldClock_newyork_coordinate_length, 1, 0, 5);
          }
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
        Render_Clock_Current_Time_Dial(LCM_Clock_Dial_coordinate, LCM_Clock_Dial_coordinate_length, &LCM_Clock_Dial_Buf, &LCM_Clock_Dial_Buf_Size, RTC_Time_Now.Hour, RTC_Time_Now.Minute, RTC_Time_Now.Second);
      }
      else if (Lyra_Clock_Style == MAPS_Clock_Digit)
      {
        Render_Clock_Current_Time_Digit(LCM_Clock_Digit_coordinate, LCM_Clock_Digit_coordinate_length, &LCM_Clock_Digit_Buf, &LCM_Clock_Digit_Buf_Size, RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      break;
    case MAPS_Menu_StopWatch:
      if (Lyra_StopWatch_Style == MAPS_StopWatch_Dial)
      {
        Render_Stop_Watch_Current_Time_Dial(LCM_StopWatch_Dial_coordinate, LCM_StopWatch_Dial_coordinate_length, &LCM_StopWatch_Dial_Buf, &LCM_StopWatch_Dial_Buf_Size, Stop_Watch_Now.Minute, Stop_Watch_Now.Second, Stop_Watch_Now.Centisecond);
      }
      else if (Lyra_StopWatch_Style == MAPS_StopWatch_Digital)
      {
        Render_Stop_Watch_Current_Time_Digit(LCM_StopWatch_Digit_coordinate, LCM_StopWatch_Digit_coordinate_length, &LCM_StopWatch_Digit_Buf, &LCM_StopWatch_Digit_Buf_Size, Stop_Watch_Now.Minute, Stop_Watch_Now.Second, Stop_Watch_Now.Centisecond);
      }
      break;
    case MAPS_Menu_AlarmClock:
      if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_List)
      {
        Render_Alarm_Clock_List(Lyra_AlarmClock_List_Cursor);
      }
      else if (Lyra_AlarmClock_Mode == MAPS_AlarmClock_Edit)
      {
        Render_Alarm_Clock_Edit(Lyra_AlarmClock_Edit_Number[0] * 10 + Lyra_AlarmClock_Edit_Number[1], Lyra_AlarmClock_Edit_Number[2] * 10 + Lyra_AlarmClock_Edit_Number[3], Lyra_AlarmClock_Edit_Cursor, Lyra_AlarmClock_List_Cursor);
      }
      break;
    case MAPS_Menu_WorldClock:
      if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Beijing)
      {
        Render_World_Clock_Time(LCM_WorldClock_beijing_coordinate, LCM_WorldClock_beijing_coordinate_length, &LCM_WorldClock_beijing_Buf, &LCM_WorldClock_beijing_Buf_Size, MAPS_WorldClock_Timezone_Array[0], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Shanghai)
      {
        Render_World_Clock_Time(LCM_WorldClock_shanghai_coordinate, LCM_WorldClock_shanghai_coordinate_length, &LCM_WorldClock_shanghai_Buf, &LCM_WorldClock_shanghai_Buf_Size, MAPS_WorldClock_Timezone_Array[1], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Hongkong)
      {
        Render_World_Clock_Time(LCM_WorldClock_hongkong_coordinate, LCM_WorldClock_hongkong_coordinate_length, &LCM_WorldClock_hongkong_Buf, &LCM_WorldClock_hongkong_Buf_Size, MAPS_WorldClock_Timezone_Array[2], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Taipei)
      {
        Render_World_Clock_Time(LCM_WorldClock_taipei_coordinate, LCM_WorldClock_taipei_coordinate_length, &LCM_WorldClock_taipei_Buf, &LCM_WorldClock_taipei_Buf_Size, MAPS_WorldClock_Timezone_Array[3], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Singapore)
      {
        Render_World_Clock_Time(LCM_WorldClock_singapore_coordinate, LCM_WorldClock_singapore_coordinate_length, &LCM_WorldClock_singapore_Buf, &LCM_WorldClock_singapore_Buf_Size, MAPS_WorldClock_Timezone_Array[4], RTC_Time_Now.Hour, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Seoul)
      {
        Render_World_Clock_Time(LCM_WorldClock_seoul_coordinate, LCM_WorldClock_seoul_coordinate_length, &LCM_WorldClock_seoul_Buf, &LCM_WorldClock_seoul_Buf_Size, MAPS_WorldClock_Timezone_Array[5], RTC_Time_Now.Hour + 1, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Tokyo)
      {
        Render_World_Clock_Time(LCM_WorldClock_tokyo_coordinate, LCM_WorldClock_tokyo_coordinate_length, &LCM_WorldClock_tokyo_Buf, &LCM_WorldClock_tokyo_Buf_Size, MAPS_WorldClock_Timezone_Array[6], RTC_Time_Now.Hour + 1, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_Sydney)
      {
        Render_World_Clock_Time(LCM_WorldClock_sydney_coordinate, LCM_WorldClock_sydney_coordinate_length, &LCM_WorldClock_sydney_Buf, &LCM_WorldClock_sydney_Buf_Size, MAPS_WorldClock_Timezone_Array[7], RTC_Time_Now.Hour + 2, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_SanFrancisco)
      {
        Render_World_Clock_Time(LCM_WorldClock_sanfrancisco_coordinate, LCM_WorldClock_sanfrancisco_coordinate_length, &LCM_WorldClock_sanfrancisco_Buf, &LCM_WorldClock_sanfrancisco_Buf_Size, MAPS_WorldClock_Timezone_Array[8], RTC_Time_Now.Hour - 16, RTC_Time_Now.Minute);
      }
      else if (Lyra_WorldClock_Time_Cursor == MAPS_WorldClock_Timezone_NewYork)
      {
        Render_World_Clock_Time(LCM_WorldClock_newyork_coordinate, LCM_WorldClock_newyork_coordinate_length, &LCM_WorldClock_newyork_Buf, &LCM_WorldClock_newyork_Buf_Size, MAPS_WorldClock_Timezone_Array[9], RTC_Time_Now.Hour - 13, RTC_Time_Now.Minute);
      }
      break;
    default:
      break;
    }
  }
}