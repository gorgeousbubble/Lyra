/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       sys_tick.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "system_init.h"
#include "sys_tick.h"

/*
 *  @brief      SysTick delay function
 *  @param      uint32  SysTick_Time
 *  @since      v1.0
 *  Sample usage:       SysTick_Delay(32);     //Systick delays by 32 system cycles
 */
void SysTick_Delay(uint32 SysTick_Time)
{
  ASSERT(SysTick_Time > 0 && SysTick_Time <= SysTick_LOAD_RELOAD_Msk);
  
  ((SysTick_Type*)SysTick_BASE)->CTRL = 0;
  ((SysTick_Type*)SysTick_BASE)->LOAD = SysTick_Time;
  ((SysTick_Type*)SysTick_BASE)->VAL = 0;
  
  ((SysTick_Type*)SysTick_BASE)->CTRL = (0 | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk);
  
  while(!(((SysTick_Type*)SysTick_BASE)->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
  
}

/*
 *  @brief      SysTick delay function
 *  @param      uint32  SysTick_Time_ms
 *  @since      v1.0
 *  Sample usage:       SysTick_Delay(32);     // systick delay 32 ms
 */
void SysTick_Delay_ms(uint32 SysTick_Time_ms)
{
  while(SysTick_Time_ms--)
  {
    SysTick_Delay(SYSTICK_CLK_KHZ);
  }
}

/*
 *  @brief      SysTick timer function
 *  @param      uint32  SysTick_Time
 *  @since      v1.0
 *  Sample usage:       SysTick_Timing(32);     // systick delay 32 system cycles
 */
void SysTick_Timing(uint32 SysTick_Time)
{
  ASSERT(SysTick_Time > 0 && SysTick_Time <= SysTick_LOAD_RELOAD_Msk);
  
  ((SysTick_Type*)SysTick_BASE)->LOAD = SysTick_Time;
  
  NVIC_SetPriority(SysTick_IRQn,(1 << __NVIC_PRIO_BITS) - 1);
  
  ((SysTick_Type*)SysTick_BASE)->VAL = 0;
  
  ((SysTick_Type*)SysTick_BASE)->CTRL = (0 | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk);
}