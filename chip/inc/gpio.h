/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       gpio.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "common.h"
#include "port.h"

/*
**define enum GPIO port data direction (input, output)
*/
typedef enum
{
  GPI,          //GPIO Input
  GPO,          //GPIO Output
}GPIO_CFG;

#define HIGH    1u
#define LOW     0u

#define GPIOX_BASE(PTxn)    GPIOX[PORT_PTX(PTxn)]       //GPIO base address pointer

/*
**GPIO port related macro
*/
#define GPIO_SET(PTxn,x)        GPIO_SET_##x(PTxn)      //x should be 0 or 1 in output, 0: low level, 1: high level
#define GPIO_DDRX(PTxn,x)       GPIO_DDRX_##x(PTxn)     //x stand for port data direction, should be 0 or 1,0: input, 1: output
#define GPIO_TURN(PTxn)         GPIO_PTOR_REG(GPIOX_BASE(PTxn)) |= (1 << PORT_PTn(PTxn))
#define GPIO_GET(PTxn)          ((GPIO_PDIR_REG(GPIOX_BASE(PTxn)) >> PORT_PTn(PTxn)) & 0x01)

#define GPIO_SET_1(PTxn)        GPIO_PDOR_REG(GPIOX_BASE(PTxn)) |= (1 << PORT_PTn(PTxn))
#define GPIO_SET_0(PTxn)        GPIO_PDOR_REG(GPIOX_BASE(PTxn)) &= ~(1 << PORT_PTn(PTxn))

#define GPIO_DDRX_1(PTxn)       GPIO_PDDR_REG(GPIOX_BASE(PTxn)) |= (1 << PORT_PTn(PTxn))
#define GPIO_DDRX_0(PTxn)       GPIO_PDDR_REG(GPIOX_BASE(PTxn)) &= ~(1 << PORT_PTn(PTxn))

/*
**variate declaration
*/
extern GPIO_MemMapPtr GPIOX[PORT_PTX_MAX];

/*
**function declaration
*/
extern void GPIO_Init(PTXn PTXx,GPIO_CFG CFG,uint8 DATA);
extern void GPIO_DDR(PTXn PTXx,GPIO_CFG CFG);
extern void GPIO_Set(PTXn PTXx,uint8 DATA);
extern void GPIO_Turn(PTXn PTXx);
extern uint8 GPIO_Get(PTXn PTXx);

#endif