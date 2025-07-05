/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       pit.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "pit.h"

/*
 *  @brief      PITn timed interrupt
 *  @param      PITn    PITx    module number(PIT0~PIT3)
 *  @param      uint32  PIT_Count       Timed interrupt time (in bus clock cycles)
 *  @since      v1.0
 *  Sample usage:
                PIT_Init(PIT0,1000);                           //Interrupt after timing 1000 bus clocks
                Set_Vector_Handler(PIT0_VECTORn,pit_hander);   //Set the interrupt service function to the interrupt vector table
                enable_irq(PIT0_IRQn);                         //Enable PIT interrupt
 */
void PIT_Init(PITn PITx,uint32 PIT_Count)
{
  //Assert PIT module number, PIT timing value is greater than 0
  ASSERT((PITx <= PIT3) && (PIT_Count > 0));
  
  SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;//PIT clock enable
  
  PIT_MCR &= ~(PIT_MCR_MDIS_MASK | PIT_MCR_FRZ_MASK);//PIT runs in debugging mode with PIT enabled
  PIT_LDVAL(PITx) = PIT_Count - 1;//Load PIT timer initial value (countdown)
  
  PIT_Flag_Clear(PITx);//Clear interrupt flag
  
  PIT_TCTRL(PITx) = (0 | PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK);//Timer interrupt enable, timer enable, without using counter cascading
  
}

/*
 *  @brief      PITn delay
 *  @param      PITn    PITx    module number(PIT0~PIT3)
 *  @param      uint32  PIT_Count       Delay time (in bus clock cycles)
 *  @since      v1.0
 *  Sample usage:       PIT_Delay(PIT0, 1000);  //delay 1000 bus clock
 */
void PIT_Delay(PITn PITx,uint32 PIT_Count)
{
  //Assert PIT module number, PIT timing value is greater than 0
  ASSERT((PITx <= PIT3) && (PIT_Count > 0));
  
  SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;//PIT clock enable
  
  PIT_MCR &= ~(PIT_MCR_MDIS_MASK | PIT_MCR_FRZ_MASK);//PIT runs in debugging mode with PIT enabled
  PIT_TCTRL(PITx) &= ~PIT_TCTRL_TEN_MASK;//Prohibit PIT interruption
  PIT_LDVAL(PITx) = PIT_Count - 1;//Load PIT timer initial value (countdown)
  
  PIT_Flag_Clear(PITx);//Clear interrupt flag
  
  PIT_TCTRL(PITx) &= ~PIT_TCTRL_TEN_MASK;//Prohibit PIT interruption
  
  
  PIT_TCTRL(PITx) = (0 | PIT_TCTRL_TEN_MASK);//Timer enabled, without cascading counters
  
  while((PIT_TFLG(PITx) & PIT_TFLG_TIF_MASK) != PIT_TFLG_TIF_MASK);
  
  PIT_Flag_Clear(PITx);//Clear interrupt flag
  
}

/*
 *  @brief      PITn timer starts
 *  @param      PITn    PITx    moudule number(PIT0~PIT3)
 *  @since      v1.0
 *  Sample usage:       PIT_Time_Start(PIT0);//PIT0 timer starts
 */
void PIT_Time_Start(PITn PITx)
{
  //Assert PIT module number
  ASSERT(PITx <= PIT3);
  
  SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;//PIT clock enable
  
  PIT_MCR &= ~(PIT_MCR_MDIS_MASK | PIT_MCR_FRZ_MASK);//PIT runs in debugging mode with PIT enabled
  PIT_TCTRL(PITx) &= ~PIT_TCTRL_TEN_MASK;//Prohibit PIT interruption
  PIT_LDVAL(PITx) = ~0;//Load PIT timer initial value (countdown)
  
  PIT_Flag_Clear(PITx);//Clear interrupt flag
  
  PIT_TCTRL(PITx) &= ~PIT_TCTRL_TEN_MASK;//Prohibit PIT interruption
  PIT_TCTRL(PITx) = (0 | PIT_TCTRL_TEN_MASK);//Timer enabled, without cascading counters
  
}

/*
 *  @brief      Get PITn timer time (timer will be turned off when exceeded)
 *  @param      PITn    PITx    module number(PIT0~PIT3)
 *  @since      v1.0
 *  Sample usage:       uint32 time = PIT_Time_Get(PIT0);       //Get PITn timing time
 */
uint32 PIT_Time_Get(PITn PITx)
{
  uint32 PIT_Value=0;
  
  //Assert PIT module number
  ASSERT(PITx <= PIT3);
  
  PIT_Value = ~0 - PIT_CVAL(PITx);//Count time value=initial count value 0~- current count value PIT_CVAL
  
  if(PIT_TFLG(PITx) & PIT_TFLG_TIF_MASK)//Has it exceeded the counting time
  {
    PIT_Flag_Clear(PITx);//Clear overflow flag
    PIT_TCTRL(PITx) &= ~PIT_TCTRL_TEN_MASK;//Prohibit PIT interruption
    return ~0;
  }
  
  if(PIT_Value == ~0)
  {
    PIT_Value = 0xffffffff - 0x1;
  }
  
  return PIT_Value;
  
}

/*
 *  @brief      Close PIT
 *  @param      PITn    PITx    module number(PIT0~PIT3)
 *  @since      v1.0
 *  Sample usage:        PIT_Close(PIT0);       //Close PIT
 */
void PIT_Close(PITn PITx)
{
  //Assert PIT module number
  ASSERT(PITx <= PIT3);
  
  PIT_Flag_Clear(PITx);//Clear overflow flag
  PIT_TCTRL(PITx) &= ~PIT_TCTRL_TEN_MASK;//Prohibit PIT interruption
  
}