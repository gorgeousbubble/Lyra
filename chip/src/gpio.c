/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       gpio.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "gpio.h"
#include "conf.h"

/*
**GPIO base address pointer
*/
GPIO_MemMapPtr GPIOX[PORT_PTX_MAX]=
{
  PTA_BASE_PTR,//PTA base address pointer
  PTB_BASE_PTR,//PTB base address pointer
  PTC_BASE_PTR,//PTC base address pointer
  PTD_BASE_PTR,//PTD base address pointer
  PTE_BASE_PTR,//PTE base address pointer
};

/*
 *  @brief      GPIO Init
 *  @param      PTXn            PTXx    port
 *  @param      GPIO_CFG        CFG     port data direction,0=output,1=input
 *  @param      uint8           DATA    output status,0=low level,1=high level(input invalidate)
 *  @since      v1.0
 *  Sample usage:       GPIO_Init(PTA8, GPI,0);    //Init PTA8 port as input
 */
void GPIO_Init(PTXn PTXx,GPIO_CFG CFG,uint8 DATA)
{
  PORT_Init(PTXx,ALT1);//Init GPIO
  
  if(CFG == GPI)//GPIO Input
  {
    GPIO_PDDR_REG(GPIOX_BASE(PTXx)) &= ~(1 << PORT_PTn(PTXx));// GPIO PDDR port number clear to 0, GPIO port as input direction
  }
  else//GPIO Output
  {
    GPIO_PDDR_REG(GPIOX_BASE(PTXx)) |= (1 << PORT_PTn(PTXx));// GPIO PDDR port number set to 1, GPIO port as output direction
    
    if(DATA == 0)//low level
    {
      GPIO_PDOR_REG(GPIOX_BASE(PTXx)) &= ~(1 << PORT_PTn(PTXx));// GPIO PDOR port number clear to 0, GPIO port as low level output
    }
    else//high level
    {
      GPIO_PDOR_REG(GPIOX_BASE(PTXx)) |= (1 << PORT_PTn(PTXx));// GPIO PDOR port number set to 1, GPIO port as high level output
    }
    
  }
  
}

/*
 *  @brief      set GPIO port data direction
 *  @param      PTXn            PTXx    port number
 *  @param      GPIO_CFG        CFG     port direction,0=input,1=output
 *  @since      v1.0
 *  Sample usage:       GPIO_DDR(PTA8, GPI);    //set PTA8 port as input
 */
void GPIO_DDR(PTXn PTXx,GPIO_CFG CFG)
{
  if(CFG == GPI)
  {
    GPIO_PDDR_REG(GPIOX_BASE(PTXx)) &= ~(1 << PORT_PTn(PTXx));// GPIO PDDR port number clear to 0, GPIO port as input direction
  }
  else
  {
    GPIO_PDDR_REG(GPIOX_BASE(PTXx)) |= (1 << PORT_PTn(PTXx));// GPIO PDDR port number set to 1, GPIO port as output direction
  }
}

/*
 *  @brief      set GPIO port status
 *  @param      PTXn     PTXx    port
 *  @param      uint8    DATA    output status, 0=low level,1=high level (input invalidate)
 *  @since      v1.0
 *  @warning    should ensure port data direction is output(assert enable in debug mode)
 *  Sample usage:       GPIO_Set(PTA8, 1);    // PTA8 port output high level
 */
void GPIO_Set(PTXn PTXx,uint8 DATA)
{
  ASSERT(BIT_GET(GPIO_PDDR_REG(GPIOX_BASE(PTXx)),PORT_PTn(PTXx)) == GPO);
  
  if(DATA == 0)//low level
  {
    GPIO_PDOR_REG(GPIOX_BASE(PTXx)) &= ~(1 << PORT_PTn(PTXx));// GPIO PDOR port number clear to 0, GPIO port output low level
  }
  else//high level
  {
    GPIO_PDOR_REG(GPIOX_BASE(PTXx)) |= (1 << PORT_PTn(PTXx));// GPIO PDOR port number set to 1, GPIO port output high level
  }
}

/*
 *  @brief      turn GPIO status
 *  @param      PTXn    PTXx    port
 *  @since      v1.0
 *  @warning    should ensure port data direction is output(assert enable in debug mode)
 *  Sample usage:       GPIO_Turn(PTA8);    // PTA8 port output turn
 */
void GPIO_Turn(PTXn PTXx)
{
  ASSERT(BIT_GET(GPIO_PDDR_REG(GPIOX_BASE(PTXx)),PORT_PTn(PTXx)) == GPO);
  
  GPIO_PTOR_REG(GPIOX_BASE(PTXx)) |= (1 << PORT_PTn(PTXx));//GPIO port turn

}

/*
 *  @brief      get GPIO status
 *  @param      PTXn    PTXx    port
 *  @return     GPIO port status, 1 high level, 0 low level
 *  @since      v1.0
 *  @warning    should ensure port data direction is output(assert enable in debug mode)
 *  Sample usage:       uint8 GPIO_DATA = GPIO_Get(PTA8);    // get PTA8 pport input
 */
uint8 GPIO_Get(PTXn PTXx)
{
  uint8 GPIO_Data=0;
  
  ASSERT(BIT_GET(GPIO_PDDR_REG(GPIOX_BASE(PTXx)),PORT_PTn(PTXx)) == GPI);
  
  GPIO_Data = (GPIO_PDIR_REG(GPIOX_BASE(PTXx)) >> PORT_PTn(PTXx)) & 0x01;
  
  return GPIO_Data;
}