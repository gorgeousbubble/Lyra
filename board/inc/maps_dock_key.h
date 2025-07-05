/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_key.h
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MAPS_DOCK_KEY_H__
#define __MAPS_DOCK_KEY_H__

#include "common.h"
#include "port.h"

/*
**Independent key enumeration
*/
typedef enum
{
  MAPS_Dock_KEY0,       //Independent button KEY0
  MAPS_Dock_KEY1,       //Independent button KEY1
  MAPS_Dock_KEY2,       //Independent button KEY2
  MAPS_Dock_KEY3,       //Independent button KEY3
  MAPS_Dock_KEY_MAX,
}MAPS_Dock_KEYn;

/*
**Independent key state enumeration
*/
typedef enum
{
  MAPS_Dock_KEY_On,     //Independent button press down
  MAPS_Dock_KEY_Off,    //Independent button pops up
}MAPS_Dock_KEY_Status;

//Independent key macro
#define MAPS_Dock_KEY_Delay(ms) DELAY_MS(ms)    //Button delay in ms

/*
**variate declaration
*/
extern PTXn MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY_MAX];

/*
**function declaration
*/
extern void MAPS_Dock_KEY_ALL_Init(void);
extern void MAPS_Dock_KEY_KEYn_Init(MAPS_Dock_KEYn MAPS_Dock_KEYx);
extern uint8 MAPS_Dock_KEY_KEYn_Get(MAPS_Dock_KEYn MAPS_Dock_KEYx);
extern MAPS_Dock_KEY_Status MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEYn MAPS_Dock_KEYx);
extern void MAPS_Dock_KEY_Incident(void);

#endif