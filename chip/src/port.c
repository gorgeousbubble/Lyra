/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       port.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "gpio.h"
#include "port.h"

/*
**PTX port base address pointer
*/
PORT_MemMapPtr PORTX[PORT_PTX_MAX]=
{
  PORTA_BASE_PTR,//PTA base address pointer
  PORTB_BASE_PTR,//PTB base address pointer
  PORTC_BASE_PTR,//PTC base address pointer
  PORTD_BASE_PTR,//PTD base address pointer
  PORTE_BASE_PTR,//PTE base address pointer
};

/*
 *  @brief      PORT Init
 *  @param      PTXn    PTXx    port
 *  @param      uint32  CFG     port attribute configuration, such as trigger options and pull-up/pull-down options
 *  @since      v1.0
 *  @note       Unlike PORT_Snit_SoALT, this function requires configuring the MUX multiplexing function, otherwise MUX=ALT0
 *  Sample usage:       PORT_Init(PTA8, IRQ_RISING | PF | ALT1 | PULLUP );    //Initialize PTA8 pin, trigger interrupt with rising edge, equipped with passive filter, multiplexing function for GPIO, pull-up resistor
 */
void PORT_Init(PTXn PTXx,uint32 CFG)
{
  switch(PORT_PTX(PTXx))//select Enable Clock
  {
    case PORT_PTA:
                  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
                  break;
    case PORT_PTB:
                  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
                  break;
    case PORT_PTC:
                  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
                  break;
    case PORT_PTD:
                  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
                  break;
    case PORT_PTE:
                  SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
                  break;
    default:
                  ASSERT(0);
                  break;
  }
  
  PORT_ISFR_REG(PORTX_BASE(PTXx)) |= (1 << PORT_PTn(PTXx));//clear interrupt flag
  PORT_PCR_REG(PORTX_BASE(PTXx),PORT_PTn(PTXx)) &= ~PORT_PCR_MUX_MASK;//clear the original MUX register value
  PORT_PCR_REG(PORTX_BASE(PTXx),PORT_PTn(PTXx)) |= (0 | CFG);//reuse of MUX pin function
  
}

/*
 *  @brief      PORT initialization
 *  @param      PTXn    PTXx    port
 *  @param      uint32  CFG     port attribute configuration, such as trigger options and pull-up/pull-down options
 *  @since      v1.0
 *  @note       Unlike PORT_Snit, this function does not require configuring the MUX multiplexing function (even if configured, it does not take effect), and the MUX remains at the value configured in the original register
 *  Sample usage:       PORT_Init_NoALT (PTA8, IRQ_RISING | PF | PULLUP );    //initialize PTA8 pin, trigger interrupt with rising edge, equipped with passive filter, retain original multiplexing function, pull-up resistor
 */
void PORT_Init_NoALT(PTXn PTXx,uint32 CFG)
{
  switch(PORT_PTX(PTXx))//Ñ¡ÔñEnableÊ±ÖÓ
  {
    case PORT_PTA:
                  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
                  break;
    case PORT_PTB:
                  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
                  break;
    case PORT_PTC:
                  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
                  break;
    case PORT_PTD:
                  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
                  break;
    case PORT_PTE:
                  SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
                  break;
    default:
                  ASSERT(0);
                  break;
  }
  
  PORT_ISFR_REG(PORTX_BASE(PTXx)) |= (1 << PORT_PTn(PTXx));//clear interrupt flag
  CFG &= ~PORT_PCR_MUX_MASK;//clear configuration MUX
  CFG |= (PORT_PCR_REG(PORTX_BASE(PTXx),PORT_PTn(PTXx)) & PORT_PCR_MUX_MASK);//read the original MUX configuration
  PORT_PCR_REG(PORTX_BASE(PTXx),PORT_PTn(PTXx)) |= (0 | CFG);//reuse of MUX pin function
  
}

/*
 *  @brief      PORT initialization EXIT external interrupt
 *  @param      PTXn    PTXx    port
 *  @param      uint32  CFG     port attribute configuration, such as trigger options and pull-up/pull-down options
 *  @since      v1.0
 *  @note       
 *  Sample usage:       PORT_Init_Exit(PTA13,IRQ_RISING)//PTA13, rising edge triggers external interrupt
 */
void PORT_Init_Exit(PTXn PTXx,uint32 CFG)
{
  switch(PORT_PTX(PTXx))//select enable clock
  {
    case PORT_PTA:
                  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
                  break;
    case PORT_PTB:
                  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
                  break;
    case PORT_PTC:
                  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
                  break;
    case PORT_PTD:
                  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
                  break;
    case PORT_PTE:
                  SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
                  break;
    default:
                  ASSERT(0);
                  break;
  }
  
  PORT_PCR_REG(PORTX_BASE(PTXx),PORT_PTn(PTXx)) &= ~PORT_PCR_MUX_MASK;//clear the original MUX register value
  PORT_PCR_REG(PORTX_BASE(PTXx),PORT_PTn(PTXx)) |= (0 | ALT1 | CFG);//reuse of MUX pin function
  GPIO_PDDR_REG(GPIOX_BASE(PTXx)) &= ~(1 << PORT_PTn(PTXx));//pin set to input mode
  PORT_ISFR_REG(PORTX_BASE(PTXx)) |= (1 << PORT_PTn(PTXx));//clear interrupt flag
  
}