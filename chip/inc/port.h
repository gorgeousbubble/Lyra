/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       port.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __PORT_H__
#define __PORT_H__

#include "common.h"

/*
**PTX port enumeration
*/
typedef enum
{
  //PORT_PTA(PTA port)
  PTA0,  PTA1,  PTA2,  PTA3,  PTA4,  PTA5,  PTA6,  PTA7,  PTA8,  PTA9,  PTA10, PTA11, PTA12, PTA13, PTA14, PTA15,
  PTA16, PTA17, PTA18, PTA19, PTA20, PTA21, PTA22, PTA23, PTA24, PTA25, PTA26, PTA27, PTA28, PTA29, PTA30, PTA31,
  
  //PORT_PTB(PTB port)
  PTB0,  PTB1,  PTB2,  PTB3,  PTB4,  PTB5,  PTB6,  PTB7,  PTB8,  PTB9,  PTB10, PTB11, PTB12, PTB13, PTB14, PTB15,
  PTB16, PTB17, PTB18, PTB19, PTB20, PTB21, PTB22, PTB23, PTB24, PTB25, PTB26, PTB27, PTB28, PTB29, PTB30, PTB31,
  
  //PORT_PTC(PTC port)
  PTC0,  PTC1,  PTC2,  PTC3,  PTC4,  PTC5,  PTC6,  PTC7,  PTC8,  PTC9,  PTC10, PTC11, PTC12, PTC13, PTC14, PTC15,
  PTC16, PTC17, PTC18, PTC19, PTC20, PTC21, PTC22, PTC23, PTC24, PTC25, PTC26, PTC27, PTC28, PTC29, PTC30, PTC31,
  
  //PORT_PTD(PTD port)
  PTD0,  PTD1,  PTD2,  PTD3,  PTD4,  PTD5,  PTD6,  PTD7,  PTD8,  PTD9,  PTD10, PTD11, PTD12, PTD13, PTD14, PTD15,
  PTD16, PTD17, PTD18, PTD19, PTD20, PTD21, PTD22, PTD23, PTD24, PTD25, PTD26, PTD27, PTD28, PTD29, PTD30, PTD31,
  
  //PORT_PTE(PTE port)
  PTE0,  PTE1,  PTE2,  PTE3,  PTE4,  PTE5,  PTE6,  PTE7,  PTE8,  PTE9,  PTE10, PTE11, PTE12, PTE13, PTE14, PTE15,
  PTE16, PTE17, PTE18, PTE19, PTE20, PTE21, PTE22, PTE23, PTE24, PTE25, PTE26, PTE27, PTE28, PTE29, PTE30, PTE31,
}PTXn;

/*
**PTX port enumeration (A,B,C,D,E)
*/
typedef enum
{
  PORT_PTA,          //PTA port
  PORT_PTB,          //PTB port
  PORT_PTC,          //PTC port
  PORT_PTD,          //PTD port
  PORT_PTE,          //PTE port
  PORT_PTX_MAX,
}PTX;

/*
**PTX port number enumeration (PT0-PT31)
*/
typedef enum
{
  PT0 , PT1 , PT2 , PT3 , PT4 , PT5 , PT6 , PT7 ,
  PT8 , PT9 , PT10, PT11, PT12, PT13, PT14, PT15,
  PT16, PT17, PT18, PT19, PT20, PT21, PT22, PT23,
  PT24, PT25, PT26, PT27, PT28, PT29, PT30, PT31,
}PTn;

/*
**PORT configure enumeration
*/
typedef enum
{
  //interrupt mode and DMA request mode, only one can be selected (optional)
  //interrupt mode selection
  IRQ_ZERO     = 0x08 << PORT_PCR_IRQC_SHIFT,   //low level trigger
  IRQ_RISING   = 0x09 << PORT_PCR_IRQC_SHIFT,   //rising edge trigger
  IRQ_FALLING  = 0x0A << PORT_PCR_IRQC_SHIFT,   //falling edge trigger
  IRQ_EITHER   = 0x0B << PORT_PCR_IRQC_SHIFT,   //jump edge trigger
  IRQ_ONE      = 0x0C << PORT_PCR_IRQC_SHIFT,   //high level trigger

  //DMA request selection
  DMA_RISING   = 0x01 << PORT_PCR_IRQC_SHIFT,   //rising edge trigger
  DMA_FALLING  = 0x02 << PORT_PCR_IRQC_SHIFT,   //falling edge trigger
  DMA_EITHER   = 0x03 << PORT_PCR_IRQC_SHIFT,   //jump edge trigger

  HDS          = 0x01 << PORT_PCR_DSE_SHIFT,    //output high driving capability
  ODO          = 0x01 << PORT_PCR_ODE_SHIFT,    //drain output
  PF           = 0x01 << PORT_PCR_PFE_SHIFT,    //with passive filter
  SSR          = 0x01 << PORT_PCR_SRE_SHIFT,    //output slow rate of change

  //pull down and up selection
  PULLDOWN     = 0x02 << PORT_PCR_PS_SHIFT,     //pull down
  PULLUP       = 0x03 << PORT_PCR_PS_SHIFT,     //pull up

  //function reuse selection (if there is no need to change the function reuse selection, keep the original function reuse and directly select ALT0)
  ALT0         = 0x00 << PORT_PCR_MUX_SHIFT,
  ALT1         = 0x01 << PORT_PCR_MUX_SHIFT,    //GPIO
  ALT2         = 0x02 << PORT_PCR_MUX_SHIFT,
  ALT3         = 0x03 << PORT_PCR_MUX_SHIFT,
  ALT4         = 0x04 << PORT_PCR_MUX_SHIFT,
  ALT5         = 0x05 << PORT_PCR_MUX_SHIFT,
  ALT6         = 0x06 << PORT_PCR_MUX_SHIFT,
  ALT7         = 0x07 << PORT_PCR_MUX_SHIFT,
}PORT_CFG;



/*
**PORT port related macros
*/
#define PORT_PTX(PTxn)          ((PTxn)>>5)             //PTx = PTxn / 32
#define PORT_PTn(PTxn)          ((PTxn)&0x1f)           //PTn = PTxn & 31
#define PORTX_BASE(PTxn)        PORTX[PORT_PTX(PTxn)]   //PORT base address pointer

/*
**external interrupt related macros
*/
#define IRQ_MARK(X,n)   (PORT##X##_ISFR & (1 << n))     //determine the interrupt flag bit
#define IRQ_CLEAR(X,n)  (PORT##X##_ISFR |= (1 << n))    //clear interrupt flag

/*
**variate declaration
*/
extern PORT_MemMapPtr PORTX[PORT_PTX_MAX];

/*
**function declaration
*/
extern void PORT_Init(PTXn PTXx,uint32 CFG);
extern void PORT_Init_NoALT(PTXn PTXx,uint32 CFG);
extern void PORT_Init_Exit(PTXn PTXx,uint32 CFG);

#endif