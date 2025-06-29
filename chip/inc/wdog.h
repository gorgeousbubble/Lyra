/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       wdog.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */


#ifndef __WDOG_H__
#define __WDOG_H__

#include "common.h"

/*
**function declaration
*/
extern void WDOG_Unlock(void);
extern void WDOG_Feed(void);
extern void WDOG_Disable(void);
extern void WDOG_Enable(void);
extern void WDOG_Init(uint32 WDOG_Cnt);

#endif