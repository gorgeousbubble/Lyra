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
PTXn MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY_MAX]={PTB21,PTB22,PTB23,PTB20};

MAPS_Screen_Status Lyra_Status = MAPS_Screen_Saver; //Screen status
MAPS_Menu_Selection Lyra_Menu_Selection = MAPS_Menu_Clock; //Menu selection

/*
 *  @brief      MAPs_Dock_KEY initializes all keys
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_KEY_ALL_Init();//MAPs_Dock_KEY initializes all keys
 */
void MAPS_Dock_KEY_ALL_Init(void)
{
  GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY0],GPI,0);
  GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY1],GPI,0);
  GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY2],GPI,0);
  GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY3],GPI,0);
}

/*
 *  @brief      MAPs_Dock_KEY initializes a single button
 *  @para       MAPS_Dock_KEYn  MAPS_Dock_KEYx  key name
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_KEY_KEYn_Init();//MAPs_Dock_KEY initializes a single button
 */
void MAPS_Dock_KEY_KEYn_Init(MAPS_Dock_KEYn MAPS_Dock_KEYx)
{
  switch(MAPS_Dock_KEYx)
  {
    case MAPS_Dock_KEY0://KEY0
                        GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY0],GPI,0);
                        break;
    case MAPS_Dock_KEY1://KEY1
                        GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY1],GPI,0);
                        break;
    case MAPS_Dock_KEY2://KEY2
                        GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY2],GPI,0);
                        break;
    case MAPS_Dock_KEY3://KEY3
                        GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY3],GPI,0);
                        break;
    case MAPS_Dock_KEY_MAX://KEYALL
                        GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY0],GPI,0);
                        GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY1],GPI,0);
                        GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY2],GPI,0);
                        GPIO_Init(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY3],GPI,0);
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
  uint8 KEY_Status=0;
  
  switch(MAPS_Dock_KEYx)
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
  if(GPIO_GET(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEYx]) == MAPS_Dock_KEY_On)
  {
    MAPS_Dock_KEY_Delay(10);//Delay debounce by 10ms
    if(GPIO_GET(MAPS_Dock_KEY_PTXn[MAPS_Dock_KEYx]) == MAPS_Dock_KEY_On)
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
  //Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Stop_Watch_icon_coordinate, LCM_Stop_Watch_icon_coordinate_length, LCM_Alarm_Clock_icon_coordinate, LCM_Alarm_Clock_icon_coordinate_length, 0, 1, 2);
  if(Lyra_Status == MAPS_Screen_Saver)
  {
    Oled_I2C_Draw_BMP_128x64(LCM_Freescale_logo, OLED_Invert_Color);
    //Press KEY0
    if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY0) == MAPS_Dock_KEY_On)
    {
      Lyra_Status = MAPS_Screen_Menu; //Switch to normal screen
      MAPS_Dock_KEY_Delay(100);//Button delay 500ms
    }
  }
  else if(Lyra_Status == MAPS_Screen_Menu)
  {
    // Check dock key press status
    //Press KEY0
    if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY0) == MAPS_Dock_KEY_On)
    {
      Lyra_Status = MAPS_Screen_Normal; //Switch to normal screen
      MAPS_Dock_KEY_Delay(100);//Button delay 500ms
    }
    //Press KEY1
    else if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY1) == MAPS_Dock_KEY_On)
    {
      Lyra_Status = MAPS_Screen_Saver; //Return back to screen saver
      MAPS_Dock_KEY_Delay(100);//Button delay 500ms
    }
    //Press KEY2
    else if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY2) == MAPS_Dock_KEY_On)
    {
      switch(Lyra_Menu_Selection)
      {
        case MAPS_Menu_Clock:
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Watch_icon_coordinate, LCM_Watch_icon_coordinate_length, LCM_Alarm_Clock_icon_coordinate, LCM_Alarm_Clock_icon_coordinate_length, 0, 0, 5);
          Lyra_Menu_Selection = MAPS_Menu_AlarmClock; //Switch to Stop Watch
          break;
        case MAPS_Menu_StopWatch:
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Stop_Watch_icon_coordinate, LCM_Stop_Watch_icon_coordinate_length, LCM_Watch_icon_coordinate, LCM_Watch_icon_coordinate_length, 0, 0, 5);
          Lyra_Menu_Selection = MAPS_Menu_Clock; //Switch to Clock
          break;
        case MAPS_Menu_AlarmClock:
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Alarm_Clock_icon_coordinate, LCM_Alarm_Clock_icon_coordinate_length, LCM_Stop_Watch_icon_coordinate, LCM_Stop_Watch_icon_coordinate_length, 0, 0, 5);
          Lyra_Menu_Selection = MAPS_Menu_StopWatch; //Switch to Stop Watch
          break;
        default:
          break;
      }
      MAPS_Dock_KEY_Delay(100);//Button delay 500ms
    }
    //Press KEY3
    else if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY3) == MAPS_Dock_KEY_On)
    {
      switch(Lyra_Menu_Selection)
      {
        case MAPS_Menu_Clock:
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Watch_icon_coordinate, LCM_Watch_icon_coordinate_length, LCM_Stop_Watch_icon_coordinate, LCM_Stop_Watch_icon_coordinate_length, 1, 0, 5);
          Lyra_Menu_Selection = MAPS_Menu_StopWatch; //Switch to Stop Watch
          break;
        case MAPS_Menu_StopWatch:
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Stop_Watch_icon_coordinate, LCM_Stop_Watch_icon_coordinate_length, LCM_Alarm_Clock_icon_coordinate, LCM_Alarm_Clock_icon_coordinate_length, 1, 0, 5);
          Lyra_Menu_Selection = MAPS_Menu_AlarmClock; //Switch to Alarm Clock
          break;
        case MAPS_Menu_AlarmClock:
          Animation_Screen_Switch_Horizontal_Scroll_Array(LCM_Alarm_Clock_icon_coordinate, LCM_Alarm_Clock_icon_coordinate_length, LCM_Watch_icon_coordinate, LCM_Watch_icon_coordinate_length, 1, 0, 5);
          Lyra_Menu_Selection = MAPS_Menu_Clock; //Switch to Clock
          break;
        default:
          break;
      }
      MAPS_Dock_KEY_Delay(100);//Button delay 500ms
    }
    // Display the current menu selection
    switch(Lyra_Menu_Selection)
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
      default:
        break;
    }
  }
  else if(Lyra_Status == MAPS_Screen_Normal)
  {
    //Press KEY0
    if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY0) == MAPS_Dock_KEY_On)
    {
      MAPS_Dock_KEY_Delay(100);//Button delay 500ms
    }
    //Press KEY1
    else if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY1) == MAPS_Dock_KEY_On)
    {
      Lyra_Status = MAPS_Screen_Menu; //Switch to screen saver
      MAPS_Dock_KEY_Delay(100);//Button delay 500ms
    }
    //Press KEY2
    else if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY2) == MAPS_Dock_KEY_On)
    {
      MAPS_Dock_KEY_Delay(100);//Button delay 500ms
    }
    //Press KEY3
    else if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY3) == MAPS_Dock_KEY_On)
    {
      MAPS_Dock_KEY_Delay(100);//Button delay 500ms
    }
    // Display the current menu selection
    switch(Lyra_Menu_Selection)
    {
      case MAPS_Menu_Clock:
        Watch_Render_Current_Time_Clock(RTC_Time_Now.Hour, RTC_Time_Now.Minute, RTC_Time_Now.Second);
        break;
      case MAPS_Menu_StopWatch:
        //Watch_Render_Stop_Watch();
        break;
      case MAPS_Menu_AlarmClock:
        //Watch_Render_Alarm_Clock();
        break;
      default:
        break;
    }
  }
}