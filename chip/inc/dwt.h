/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       dwt.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __DWT_H__
#define __DWT_H__

#include "common.h"

#define DWT_CLK_KHZ             MK64_Core_KHz

/*
**function declaration
*/
extern void DWT_Delay_us(uint32 DWT_us);
extern void DWT_Delay_ms(uint32 DWT_ms);


#endif