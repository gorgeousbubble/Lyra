/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       beep.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __BEEP_H__
#define __BEEP_H__

#include "common.h"

/*
** Beep port macro definition
*/
// Beep port initialization
#ifndef BEEP_PORT_INIT
#define BEEP_PORT_INIT GPIO_Init(PTE7, GPO, 0)
#endif
// Beep port high level
#ifndef BEEP_SET_H
#define BEEP_SET_H GPIO_SET(PTE7, 1)
#endif
// Beep port low level
#ifndef BEEP_SET_L
#define BEEP_SET_L GPIO_SET(PTE7, 0)
#endif

/*
**variate declaration
*/
/* Global buzzer mute flag: 0 = sound enabled, 1 = muted (all beeps suppressed).
 * User-configurable via the Configure-Adjust menu, persisted in Flash
 * (Sector 6 / Page 96). Beep_On() honours this flag, so every buzzer source
 * (alarm, tilt alarm, free-fall) is silenced at once when muted. */
extern uint8 Beep_Muted;

/*
**function declaration
*/
extern void Beep_Init(void);
extern void Beep_On(void);
extern void Beep_Off(void);

#endif