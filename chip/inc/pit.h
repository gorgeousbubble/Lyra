/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       pit.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __PIT_H__
#define __PIT_H__

#include "common.h"

/*
**PIT module enumeration
*/
typedef enum
{
  PIT0,
  PIT1,
  PIT2,
  PIT3,
  PIT_MAX,
}PITn;

/*
**PIT macro
*/
#define PIT_Flag_Clear(PITx)    (PIT_TFLG(PITx) |= PIT_TFLG_TIF_MASK)    

/*
**PIT interrupts
*/
#define PIT_Init_ms(PITx,PIT_Count)     PIT_Init(PITx,PIT_Count*MK64_Bus_KHz)
#define PIT_Init_us(PITx,PIT_Count)     PIT_Init(PITx,PIT_Count*MK64_Bus_KHz/1000)
#define PIT_Init_ns(PITx,PIT_Count)     PIT_Init(PITx,PIT_Count*MK64_Bus_KHz/1000000)

/*
**PIT delay (no initialization required)
*/
#define PIT_Delay_ms(PITx,PIT_Count)    PIT_Delay(PITx,PIT_Count*MK64_Bus_KHz)
#define PIT_Delay_us(PITx,PIT_Count)    PIT_Delay(PITx,PIT_Count*MK64_Bus_KHz/1000)
#define PIT_Delay_ns(PITx,PIT_Count)    PIT_Delay(PITx,PIT_Count*MK64_Bus_KHz/1000000)

/*
**PIT timing
*/
#define PIT_Time_Get_ms(PITx)           (PIT_Time_Get(PITx)/MK64_Bus_KHz)
#define PIT_Time_Get_us(PITx)           (PIT_Time_Get(PITx)/(MK64_Bus_KHz/1000))

/*
**PIT interrupt related vectors and IRQ
*/
//PIT0_VECTORn,PIT0 interrupt vector,PIT0_IRQn
//PIT1_VECTORn,PIT0 interrupt vector,PIT1_IRQn
//PIT2_VECTORn,PIT0 interrupt vector,PIT2_IRQn
//PIT3_VECTORn,PIT0 interrupt vector,PIT3_IRQn

/*
**function declaration
*/
extern void PIT_Init(PITn PITx,uint32 PIT_Count);
extern void PIT_Delay(PITn PITx,uint32 PIT_Count);
extern void PIT_Time_Start(PITn PITx);
extern uint32 PIT_Time_Get(PITn PITx);
extern void PIT_Close(PITn PITx);


#endif