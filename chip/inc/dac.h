/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       dac.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __DAC_H__
#define __DAC_H__

#include "common.h"

/*
**DAC module enumerate
*/
typedef enum
{
  DAC_DAC0,
  DAC_DAC1,
  DAC_DAC_MAX,
}DAC_DACn;

/*
**variate declaration
*/
extern DAC_MemMapPtr DACN[DAC_DAC_MAX];

/*
**function declaration
*/
extern void DAC_Init(DAC_DACn DAC_DACx);
extern void DAC_Out(DAC_DACn DAC_DACx,uint16 DAC_Value);


#endif