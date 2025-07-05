/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_rocker.h
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MAPS_DOCK_ROCKER_H__
#define __MAPS_DOCK_ROCKER_H__

#include "common.h"
#include "port.h"

/*
**Joystick button structure
*/
//List of joystick buttons
typedef enum
{
  Rocker_Up,    //UP key
  Rocker_Donw,  //DOWN key
  Rocker_Left,  //LEFT key
  Rocker_Right, //RIGHT key
  Rocker_Select,//SELECT key
  Rocker_MAX,
}Rocker_Key_e;

//List of joystick button states
typedef enum
{
  Rocker_Key_On,
  Rocker_Key_Off,
}Rocker_Key_Status_e;

//Joystick button message structure
typedef struct
{
  Rocker_Key_e  Key;//key number            
  Rocker_Key_Status_e   Key_Statu;//key status
}Rocker_Key_Message_s;

//Joystick button macro
#define Rocker_Key_Delay(ms)    DELAY_MS(ms)

/*
**variate declaration
*/
extern PTXn Rocker_Key_PTXn[Rocker_MAX];

/*
**function declaration
*/
extern void MAPS_Dock_Rocker_Key_Init(void);
extern void MAPS_Dock_Rocker_Key_LCM_Control(void);

#endif