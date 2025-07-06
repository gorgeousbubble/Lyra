/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       lptmr.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __LPTMR_H__
#define __LPTMR_H__

#include "common.h"

/*
**LPTMR enamurate
*/
typedef enum
{
  LPT0_ALT1=1,//PTA19
  LPT0_ALT2=2,//PTC5
}LPT0_ALTn;

/*
**LPTMR pulse counting triggering method
*/
typedef enum
{
  LPT_Rising, //Rising edge trigger
  LPT_Falling,//Falling edge trigger
}LPT_CFG;

/*
**LPTMR macros
*/
#define LPTMR_Flag_Clear        (LPTMR0_CSR |= LPTMR_CSR_TCF_MASK)//Clear LPT comparison bit


/*
**function declaration
*/
extern void LPTMR_Pluse_Init(LPT0_ALTn LPT0_ALTx,LPT_CFG LPT_CFG_x,uint16 LPT_Count);
extern uint16 LPTMR_Pluse_Get(void);
extern void LPTMR_Count_Clean(void);
extern void LPTMR_Delay_ms(uint16 LPTMR_Delay);
extern void LPTMR_Timing_ms(uint16 LPTMR_Delay);
extern void LPTMR_Delay_us(uint16 LPTMR_Delay);
extern void LPTMR_Timing_us(uint16 LPTMR_Delay);

#endif