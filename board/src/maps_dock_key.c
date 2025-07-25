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

#include "conf.h"
#include "dwt.h"
#include "gpio.h"
#include "it.h"
#include "maps_dock_key.h"
#include "oled_i2c.h"
#include "uart.h"
#include "watch.h"

/*
**Independent button port
*/
PTXn MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY_MAX]={PTB21,PTB22,PTB23,PTB20};

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
  //Press KEY0
  if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY0) == MAPS_Dock_KEY_On)
  {
    //Put Your Code...
    UART_PutStr(UART_UART0,"KEY0 ");
    MAPS_Dock_KEY_Delay(500);//Button delay 500ms
  }
  else if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY1) == MAPS_Dock_KEY_On)
  {
    //Put Your Code...
    UART_PutStr(UART_UART0,"KEY1 ");
    MAPS_Dock_KEY_Delay(500);//Button delay 500ms
  }
  else if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY2) == MAPS_Dock_KEY_On)
  {
    //Put Your Code...
    UART_PutStr(UART_UART0,"KEY2 ");
    MAPS_Dock_KEY_Delay(500);//Button delay 500ms
  }
  else if(MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEY3) == MAPS_Dock_KEY_On)
  {
    //Put Your Code...
    UART_PutStr(UART_UART0,"KEY3 ");
    MAPS_Dock_KEY_Delay(500);//Button delay 500ms
  }
  
  //Oled_I2C_Put_Str_6x8(28, 0, "Lyra Project");
  //Oled_I2C_Put_Date_6x8(34, 1, "-", RTC_Time_Now.Year, RTC_Time_Now.Month, RTC_Time_Now.Day);
  //Oled_I2C_Put_Time_6x8(40, 2, ":", RTC_Time_Now.Hour, RTC_Time_Now.Minute, RTC_Time_Now.Second);
  //Watch_Render_Current_Time_6x8(40, 1, ":", RTC_Time_Now.Hour, RTC_Time_Now.Minute, RTC_Time_Now.Second, ClockSystem12);
  //Watch_Render_Current_Time_6x8(40, 2, ":", RTC_Time_Now.Hour, RTC_Time_Now.Minute, RTC_Time_Now.Second, ClockSystem24);
  //Watch_Render_Current_Time_Wo_Sec_12x24(34, 3, ":", RTC_Time_Now.Hour, RTC_Time_Now.Minute, ClockSystem24);
  
  //Oled_I2C_Draw_BMP_128x64(LCM_Freescale_logo, OLED_Invert_Color);
  Watch_Render_Current_Time_Clock(RTC_Time_Now.Hour, RTC_Time_Now.Minute, RTC_Time_Now.Second);
}