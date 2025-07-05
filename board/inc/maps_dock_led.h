/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_led.h
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MAPS_DOCK_LED_H__
#define __MAPS_DOCK_LED_H__

#include "common.h"

/*
**MAPS_Dock_LED enumeration
*/
typedef enum
{
  MAPS_Dock_LED1,
  MAPS_Dock_LED2,
  MAPS_Dock_LED3,
  MAPS_Dock_LED4,
  MAPS_Dock_LED_MAX,
}MAPS_Dock_LEDn;

/*
**MAPS_Dock_LED status
*/
typedef enum
{
  LED_ON,
  LED_OFF,
}LED_Status;

/*
**MAPS LEDs
*/
//LED1(LD3)
#define MAPS_Dock_LED1_On       GPIO_Init(PTB7,GPO,0)   //LED1 on
#define MAPS_Dock_LED1_Off      GPIO_Init(PTB7,GPO,1)   //LED1 off
#define MAPS_Dock_LED1_Turn     GPIO_Turn(PTB7)         //LED1 turn

//LED2(LD4)
#define MAPS_Dock_LED2_On       GPIO_Init(PTB6,GPO,0)   //LED2 on
#define MAPS_Dock_LED2_Off      GPIO_Init(PTB6,GPO,1)   //LED2 off
#define MAPS_Dock_LED2_Turn     GPIO_Turn(PTB6)         //LED2 turn

//LED3(LD5)
#define MAPS_Dock_LED3_On       GPIO_Init(PTB5,GPO,0)   //LED3 on
#define MAPS_Dock_LED3_Off      GPIO_Init(PTB5,GPO,1)   //LED3 off
#define MAPS_Dock_LED3_Turn     GPIO_Turn(PTB5)         //LED3 turn

//LED4(LD6)
#define MAPS_Dock_LED4_On       GPIO_Init(PTB4,GPO,0)   //LED4 on
#define MAPS_Dock_LED4_Off      GPIO_Init(PTB4,GPO,1)   //LED4 off
#define MAPS_Dock_LED4_Turn     GPIO_Turn(PTB4)         //LED4 turn

/*
**function declaration
*/
extern void MAPS_Dock_LEDx_Init(MAPS_Dock_LEDn MAPS_Dock_LEDx,LED_Status LED_Status_x);
extern void MAPS_Dock_LEDx_Turn(MAPS_Dock_LEDn MAPS_Dock_LEDx);
extern void MAPS_Dock_LED_Init(void);
extern void MAPS_Dock_LED_Turn(void);

#endif