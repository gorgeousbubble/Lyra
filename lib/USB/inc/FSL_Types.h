/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       fsl_types.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __FSL_TYPES_H__
#define __FSL_TYPES_H__

#include "common.h"

#define FLAG_SET(BitNumber, Register)   (Register |=(1<<BitNumber))
#define FLAG_CLR(BitNumber, Register)   (Register &=~(1<<BitNumber))
#define FLAG_CHK(BitNumber, Register)   (Register & (1<<BitNumber))

#define _OUT            1
#define _IN             0
#define _ON             0
#define _OFF            1
#define _HIGH           1
#define _LOW            0

#define _BGND   asm(HALT)
#define _NOP    asm(NOP)

#endif