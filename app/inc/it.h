/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       it.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __IT_H__
#define __IT_H__

#include "common.h"
#include "vectors.h"

#undef  VECTOR_077
#define VECTOR_077      PORTC_PTC19_IRQHandler

#undef  VECTOR_078
#define VECTOR_078      PORTD_PTD15_IRQHandler

#undef  VECTOR_064
#define VECTOR_064      PIT0_IRQHandler

/*
**variate declaration
*/
extern int PIT_Count;//PIT counter
extern uint16 ADC_Convert_Result[2];//ADC sample

/*
**function declaration
*/
extern void PORTC_PTC19_IRQHandler(void);
extern void PORTD_PTD15_IRQHandler(void);
extern void PIT0_IRQHandler(void);



#endif