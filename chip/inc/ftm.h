/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       ftm.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __FTM_H__
#define __FTM_H__

#include "common.h"

/*
**FTM module number enumeration
*/
typedef enum
{
  FTM_FTM0,
  FTM_FTM1,
  FTM_FTM2,
  FTM_FTM3,
  FTM_FTM_MAX,
}FTM_FTMn;

/*
**FTM channel number
*/
typedef enum
{
  FTM_CH0,
  FTM_CH1,
  FTM_CH2,
  FTM_CH3,
  FTM_CH4,
  FTM_CH5,
  FTM_CH6,
  FTM_CH7,
}FTM_CHn;

/*
**FTM frequency division coefficient
*/
typedef enum
{
  FTM_PS_1,
  FTM_PS_2,
  FTM_PS_4,
  FTM_PS_8,
  FTM_PS_16,
  FTM_PS_32,
  FTM_PS_64,
  FTM_PS_128,
  FTM_PS_MAX,
}FTM_PS_n;

/*
**FTM input capture
*/
typedef enum
{
  FTM_Rising,                   //Rising edge capture
  FTM_Falling,                  //Falling edge capture
  FTM_Rising_or_Falling,        //Jump edge capture
}FTM_INPUT_CFG;

/*
**FTM module PWM accuracy setting
*/
#define FTM0_PRECISON   10000u     //Define the precision of duty cycle, where 100 represents 1% precision and 1000u represents 0.1% precision. This is used for inputting duty cycle parameters, i.e. duty cycle is duty/FTM-PRECISON
#define FTM1_PRECISON   10000u     //Define the precision of duty cycle, where 100 represents 1% precision and 1000u represents 0.1% precision. This is used for inputting duty cycle parameters, i.e. duty cycle is duty/FTM-PRECISON
#define FTM2_PRECISON   10000u     //Define the precision of duty cycle, where 100 represents 1% precision and 1000u represents 0.1% precision. This is used for inputting duty cycle parameters, i.e. duty cycle is duty/FTM-PRECISON
#define FTM3_PRECISON   10000u     //Define the precision of duty cycle, where 100 represents 1% precision and 1000u represents 0.1% precision. This is used for inputting duty cycle parameters, i.e. duty cycle is duty/FTM-PRECISON


/*
**variate declaration
*/
extern FTM_MemMapPtr FTMN[FTM_FTM_MAX];

/*
**function declaration
*/
extern void FTM_PWM_Init(FTM_FTMn FTM_FTMx,FTM_CHn FTM_CHx,uint32 FTM_Freq,uint32 FTM_Duty);
extern void FTM_PWM_Duty(FTM_FTMn FTM_FTMx,FTM_CHn FTM_CHx,uint32 FTM_Duty);
extern void FTM_PWM_Freq(FTM_FTMn FTM_FTMx,uint32 FTM_Freq);
extern void FTM_Input_Init(FTM_FTMn FTM_FTMx,FTM_CHn FTM_CHx,FTM_INPUT_CFG FTM_CFG,FTM_PS_n FTM_PS_x);
extern uint16 FTM_Input_Get(FTM_FTMn FTM_FTMx,FTM_CHn FTM_CHx);
extern void FTM_Input_Clean(FTM_FTMn FTM_FTMx);
extern void FTM_QUAD_Init(FTM_FTMn FTM_FTMx);
extern int16 FTM_QUAD_Get(FTM_FTMn FTM_FTMx);
extern void FTM_QUAD_Clean(FTM_FTMn FTM_FTMx);

#endif