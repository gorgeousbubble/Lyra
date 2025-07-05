/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_led.c
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "gpio.h"
#include "maps_dock_led.h"
#include "port.h"

PTXn MAPS_LED[MAPS_Dock_LED_MAX]={PTB7,PTB6,PTB5,PTB4};

/*
 *  @brief      MAPs_Dock_LED Single LED Initialization
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LEDx_Init();//Initialization MAPS_Dock_LED
 */
void MAPS_Dock_LEDx_Init(MAPS_Dock_LEDn MAPS_Dock_LEDx,LED_Status LED_Status_x)
{
  GPIO_Init(MAPS_LED[MAPS_Dock_LEDx],GPO,LED_Status_x);
}

/*
 *  @brief      MAPs_Dock_LED Single LED Reverse
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LEDx_Turn();//Initialization MAPS_Dock_LED
 */
void MAPS_Dock_LEDx_Turn(MAPS_Dock_LEDn MAPS_Dock_LEDx)
{
  GPIO_Turn(MAPS_LED[MAPS_Dock_LEDx]);
}

/*
 *  @brief      MAPS_Dock_LED Initialization
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LED_Init();//Initialization MAPS_Dock_LED
 */
void MAPS_Dock_LED_Init(void)
{
  MAPS_Dock_LED1_Off;    //LED1 off
  MAPS_Dock_LED2_On;     //LED2 on
  MAPS_Dock_LED3_Off;    //LED3 off
  MAPS_Dock_LED4_On;     //LED4 on
}

/*
 *  @brief      MAPS_Dock_LED turnover
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LED_Turn();//Initialization MAPS_Dock_LED
 */
void MAPS_Dock_LED_Turn(void)
{
  MAPS_Dock_LED1_Turn;  //LED1 turnover
  MAPS_Dock_LED2_Turn;  //LED2 turnover
  MAPS_Dock_LED3_Turn;  //LED3 turnover
  MAPS_Dock_LED4_Turn;  //LED4 turnover
}