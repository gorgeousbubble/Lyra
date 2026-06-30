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

#include "animation.h"
#include "common.h"
#include "main.h"
#include "port.h"

/*
**Independent key enumeration
*/
typedef enum
{
  MAPS_Dock_KEY0, // Independent button KEY0
  MAPS_Dock_KEY1, // Independent button KEY1
  MAPS_Dock_KEY2, // Independent button KEY2
  MAPS_Dock_KEY3, // Independent button KEY3
  MAPS_Dock_KEY_MAX,
} MAPS_Dock_KEYn;

/*
**Independent key state enumeration
*/
typedef enum
{
  MAPS_Dock_KEY_On,  // Independent button press down
  MAPS_Dock_KEY_Off, // Independent button pops up
} MAPS_Dock_KEY_Status;

// Non-blocking key debounce / repeat-prevention.
// MAPS_Dock_KEY_Delay(ms) used to be #define MAPS_Dock_KEY_Delay(ms) DELAY_MS(ms)
// — a pure busy-wait that blocked the entire main loop for 100–800 ms on every
// key press, stalling sensor reads, step counting, free-fall detection, etc.
//
// Replacement strategy:
//   Every key action at the end calls KEY_ACTION_DONE(N).  This records a
//   timestamp and cooldown length in file-local statics inside maps_dock_key.c,
//   then returns from MAPS_Dock_KEY_Incident().  On the next call the guard at
//   the top of the function compares the elapsed time (derived from the global
//   ms counter KEY_Ms_Now() exported from it.c) against the cooldown; if the
//   cooldown has not expired it returns immediately without scanning keys.
//
//   Functionally equivalent to busy-waiting for the user (the UI ignores input
//   during the cooldown) but the main loop keeps running at full speed.
//
// KEY_ACTION_DONE(ms) — use in place of every MAPS_Dock_KEY_Delay(N); call.
// It stores the timestamp + cooldown and returns from the current function.
#define KEY_ACTION_DONE(ms) do { key_action_timestamp(ms); return; } while(0)

/*
**variate declaration
*/
extern PTXn MAPS_Dock_KEY_PTXn[MAPS_Dock_KEY_MAX];
extern int Lyra_Status;          // Screen status
extern int Lyra_Menu_Selection;  // Menu selection
extern int Lyra_Clock_Style;     // Clock style
extern int Lyra_StopWatch_Style; // Stop Watch style

/*
**function declaration
*/
extern void MAPS_Dock_KEY_ALL_Init(void);
extern void MAPS_Dock_KEY_KEYn_Init(MAPS_Dock_KEYn MAPS_Dock_KEYx);
extern uint8 MAPS_Dock_KEY_KEYn_Get(MAPS_Dock_KEYn MAPS_Dock_KEYx);
extern MAPS_Dock_KEY_Status MAPS_Dock_KEY_KEYn_Check(MAPS_Dock_KEYn MAPS_Dock_KEYx);
extern void MAPS_Dock_KEY_Incident(void);
/* Internal helper — called by KEY_ACTION_DONE macro only */
extern void key_action_timestamp(uint32 cooldown_ms);

extern void Calc_Dynamic_Animation_Cache_Array(Coord **buff, int *buffSize, const uint8 array[64][16]);
extern void Release_Dynamic_Animation_Cache(CoordCache *array, int len);
extern void Refresh_Dynamic_Animation_Cache(CoordCache *array, int len, int menu, int index[2]);

#endif