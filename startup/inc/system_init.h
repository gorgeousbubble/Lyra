/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       system_init.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __SYSTEMINIT_H__
#define __SYSTEMINIT_H__

#include "common.h"


/*
**variate declaration
*/
extern uint16  MK64_Core_MHz;
extern uint32  MK64_Core_KHz;
extern uint32  MK64_Bus_KHz;

/*
**function declaration
*/
extern void Common_StartUp(void);
extern void SysInit(void);
extern void Start(void);


#endif