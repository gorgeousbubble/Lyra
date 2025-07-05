/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       delay.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */


#include "delay.h"
#include "system_init.h"

/*
 *  @brief      Microsecond level delay (us)
 *  @param      uint32  Delay_Time
 *  @since      v1.0
 *  Sample usage:       DELAY_us(1);//delay 1us
 */
void DELAY_us(uint32 Delay_Time)
{
  uint32 i=0;
  uint32 j=0;
  uint32 DELAY_Clk=MK64_Core_MHz;//Delay reference clock
  
  for(i=0;i<Delay_Time;i++)
  {
    for(j=0;j<DELAY_Clk;j++)
    {
      asm("nop");
    }
  }
  
}

/*
 *  @brief      Millisecond level delay (ms)
 *  @param      uint32  Delay_Time
 *  @since      v1.0
 *  Sample usage:       DELAY_ms(1);//delay 1ms
 */
void DELAY_ms(uint32 Delay_Time)
{
  uint32 i=0;
  uint32 j=0;
  uint32 DELAY_Clk=MK64_Core_KHz;//Delay reference clock
  
  for(i=0;i<Delay_Time;i++)
  {
    for(j=0;j<DELAY_Clk;j++)
    {
      asm("nop");
    }
  }
  
}