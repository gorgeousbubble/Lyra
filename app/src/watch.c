/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       watch.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "oled_i2c.h"
#include "watch.h"

/*
 *  @brief      Watch_Render_Current_Time_6x8
 *  @param      uint8           x               x scope is 0~127
 *  @param      uint8           y               y scope is 0~7
 *  @param      uint8           ch[]            separator
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @param      int             second          second integer parameter
 *  @param      ClockSystem     clocksystem     clock system (12-hours/24-hours)
 *  @since      v1.0
 *  Sample usage:       Watch_Render_Current_Time_6x8(0,0,":",10,15,30,ClockSystem24);
*/
void Watch_Render_Current_Time_6x8(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second, ClockSystem clock_system) 
{
  int isAM = TRUE; // Assume AM by default
  switch (clock_system) 
  {
    case ClockSystem12:
      if (hour > 12) 
      {
        hour -= 12; // Convert to 12-hour format
        isAM = FALSE; // PM
      }
      else 
      {
        isAM = TRUE; // AM
      }
      Oled_I2C_Put_Time_6x8_12(x, y, ch, hour, minute, second, isAM);
      break;
    case ClockSystem24:
      // No conversion needed for 24-hour format
      Oled_I2C_Put_Time_6x8_24(x, y, ch, hour, minute, second);
      break;
    default:
      ASSERT(0);
      break; // Invalid clock system
  }
}

/*
 *  @brief      Watch_Render_Current_Time_8x16
 *  @param      uint8           x               x scope is 0~127
 *  @param      uint8           y               y scope is 0~7
 *  @param      uint8           ch[]            separator
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @param      int             second          second integer parameter
 *  @param      ClockSystem     clocksystem     clock system (12-hours/24-hours)
 *  @since      v1.0
 *  Sample usage:       Watch_Render_Current_Time_8x16(0,0,":",10,15,30,ClockSystem24);
*/
void Watch_Render_Current_Time_8x16(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second, ClockSystem clock_system) 
{
  int isAM = TRUE; // Assume AM by default
  switch (clock_system) 
  {
    case ClockSystem12:
      if (hour > 12) 
      {
        hour -= 12; // Convert to 12-hour format
        isAM = FALSE; // PM
      }
      else 
      {
        isAM = TRUE; // AM
      }
      Oled_I2C_Put_Time_8x16_12(x, y, ch, hour, minute, second, isAM);
      break;
    case ClockSystem24:
      // No conversion needed for 24-hour format
      Oled_I2C_Put_Time_8x16_24(x, y, ch, hour, minute, second);
      break;
    default:
      ASSERT(0);
      break; // Invalid clock system
  }
}