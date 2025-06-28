/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       mcg.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MCG_H__
#define __MCG_H__

#include "common.h"

/*
**PLL phase-locked loop (PLL frequency)
*/
typedef enum
{
  PLL48,
  PLL50,
  PLL52,
  PLL54,
  PLL56,
  PLL58,
  PLL60,
  PLL62,
  PLL64,
  PLL66,
  PLL68,
  PLL70,
  PLL72,
  PLL74,
  PLL76,
  PLL78,
  PLL80,
  PLL82,
  PLL84,
  PLL86,
  PLL88,
  PLL90,
  PLL92,
  PLL94,
  PLL96,
  PLL98,
  PLL100,
  PLL102,
  PLL104,
  PLL106,
  PLL108,
  PLL110,
  PLL115,
  PLL120,
  PLL125,
  PLL130,
  PLL135,
  PLL140,
  PLL150,
  PLL160,
  PLL170,
  PLL180,
  PLL200,
  PLLMAX,
}PLLn;

/*
**PLL frequency division factor structure
*/
typedef struct
{
  uint16 PLL_CLK;
  uint8  PLL_PRDIV;
  uint8  PLL_VDIV;
}MCG_PLL_DIV;

/*
**SIM clock division factor structure
*/
typedef struct
{
  uint8 SIM_CORE_DIV;
  uint8 SIM_BUS_DIV;
  uint8 SIM_FLEX_DIV;
  uint8 SIM_FLASH_DIV;
}MCG_SIM_DIV;

/*
**variate declaration
*/
extern MCG_PLL_DIV MCG_PLL[PLLMAX];
extern MCG_SIM_DIV MCG_DIV;

/*
**function declaration
*/
extern void MCG_DIV_Count(PLLn PLLx);
extern uint8 PLL_Init(PLLn PLLx);


#endif