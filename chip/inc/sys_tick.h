/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       sys_tick.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __SYSTICK_H__
#define __SYSTICK_H__


#include "common.h"

/*
**SysTick clock
*/
#define SYSTICK_CLK_KHZ MK64_Core_KHz

#define SysTick_DELAY_US(us)    SysTick_Delay(us * (SYSTICK_CLK_KHZ / 1000))
#define SysTick_DELAY_NS(ns)    SysTick_Delay(ns * (SYSTICK_CLK_KHZ / 1000000))

#define SysTick_TIMING_MS(ms)   SysTick_Timing(ms * (SYSTICK_CLK_KHZ))
#define SysTick_TIMING_US(us)   SysTick_Timing(us * (SYSTICK_CLK_KHZ / 1000))
#define SysTick_TIMING_NS(ns)   SysTick_Timing(ns * (SYSTICK_CLK_KHZ / 1000000))

/*
**function declaration
*/
extern void SysTick_Delay(uint32 SysTick_Time);
extern void SysTick_Delay_ms(uint32 SysTick_Time_ms);
extern void SysTick_Timing(uint32 SysTick_Time);

#endif