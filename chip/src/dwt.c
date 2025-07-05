/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       dwt.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "dwt.h"
#include "system_init.h"

/*
 *  @brief      DWT delay(us)
 *  @param      uint32  DWT_us      delay time
 *  @since      v1.0
 */
void DWT_Delay_us(uint32 DWT_us)
{
  uint32 DWT_St=0;
  uint32 DWT_Et=0;
  uint32 DWT_Ts=0;
  
  ((CoreDebug_Type*)CoreDebug_BASE)->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;//Debug clock enable
  ((DWT_Type*)DWT_BASE)->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  
  DWT_St = ((DWT_Type*)DWT_BASE)->CYCCNT;
  DWT_Ts = DWT_us * (DWT_CLK_KHZ / 1000);
  DWT_Et = DWT_St + DWT_Ts;
  
  if(DWT_Et < DWT_St)
  {
    while((((DWT_Type*)DWT_BASE)->CYCCNT) > DWT_Et);
  }
  
  while((((DWT_Type*)DWT_BASE)->CYCCNT) < DWT_Et);
  
}

/*
 *  @brief      DWT delay(ms)
 *  @param      uint32  DWT_ms      delay time
 *  @since      v1.0
 */
void DWT_Delay_ms(uint32 DWT_ms)
{
  while(DWT_ms--)
  {
    DWT_Delay_us(1000);
  }
}