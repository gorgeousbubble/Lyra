/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       lptmr.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "lptmr.h"
#include "port.h"

/*
 *  @brief      LPTMR pulse count initialization
 *  @param      LPT0_ALTn       LPT0_ALTx       LPTMR pulse counting pin
 *  @param      LPT_CFG         LPT_CFG_x       LPTMR pulse counting method: rising edge counting or falling edge counting
 *  @param      uint16          LPT_Count       LPTMR pulse comparison value
 *  @since      v1.0
 *  Sample usage:       LPTMR_Pluse_Init(LPT0_ALT1,LPT_Rising,0xFFFF);//LPTMR pulse counting, triggering interrupt request after counting 0xFFFF (interrupt service function needs to be activated before execution), capturing rising edge
 */
void LPTMR_Pluse_Init(LPT0_ALTn LPT0_ALTx,LPT_CFG LPT_CFG_x,uint16 LPT_Count)
{
  OSC_CR |= OSC_CR_ERCLKEN_MASK;//Enable external crystal oscillator reference clock
  
  SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;//LPTMR clock enable
  
  switch(LPT0_ALTx)//LPTMR Reuse
  {
    case LPT0_ALT1:
                    PORT_Init(PTA19,ALT6);//PTA19 reuse ALT6
                    break;
    case LPT0_ALT2:
                    PORT_Init(PTC5,ALT4);//PTC5 reuse ALT4
                    break;
    default:
                    ASSERT(0);
                    break;
  }

  LPTMR0_CSR = 0;//Clear LPTMR0_SR register
  
  LPTMR0_PSR = (0 
                | LPTMR_PSR_PCS(3)      //Clock source selection OSC crystal oscillator clock
                | LPTMR_PSR_PRESCALE(4) //Pre division setting 2 ^ (n+1)
                );
  
  LPTMR0_CMR = LPTMR_CMR_COMPARE(LPT_Count);//Set LPTMR count comparison value
  
  LPTMR0_CSR = (0
                | LPTMR_CSR_TPS(LPT0_ALTx)//LPTMR pulse input, ALT1,ALT2
                | LPTMR_CSR_TMS_MASK//Select pulse counting mode
                | (LPT_CFG_x == LPT_Falling ? LPTMR_CSR_TPP_MASK : 0)//Select pulse counting method, rising edge counting, falling edge counting
                | LPTMR_CSR_TEN_MASK//LPTMR Timer Enable
                | LPTMR_CSR_TIE_MASK//LPTMR timer interrupt enable
                );

}

/*
 *  @brief      Obtain LPTMR pulse count value
 *  @return     Pulse count value
 *  @since      v1.0
 *  Sample usage:       uint16 data = LPTMR_Pluse_Get();//Obtain pulse count value
 */
uint16 LPTMR_Pluse_Get(void)
{
  uint16 LPTMR_Data=0;

  if(LPTMR0_CSR & LPTMR_CSR_TCF_MASK)//LPTMR counter overflow
  {
    LPTMR_Data = ~0;
  }
  else
  {
    LPTMR0_CNR = LPTMR_CNR_COUNTER(0);
    LPTMR_Data = LPTMR0_CNR;//Read LPTMR pulse count value
  }
  
  return LPTMR_Data;
}

/*
 *  @brief      Clear LPTMR pulse count
 *  @since      v1.0
 *  Sample usage:       LPTMR_Count_Clean();//Clear LPTMR pulse count
 */
void LPTMR_Count_Clean(void)
{
  LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;//Prohibit LPTMR counter
  LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;//Enable LPTMR counter
}

/*
 *  @brief      LPTMR delay function(ms)
 *  @param      uint16          LPTMR_Delay      LPTMR delay timer(0~65535)
 *  @since      v1.0
 *  Sample usage:       LPTMR_Delay_ms(32);     //LPTMR delay 32ms
 */
void LPTMR_Delay_ms(uint16 LPTMR_Delay)
{
  //Assert that the delay time is greater than 0
  ASSERT(LPTMR_Delay > 0);

  SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;//LPTMR clock enable
  
  LPTMR0_CSR = 0;//Clear LPTMR0_SR register
  LPTMR0_CMR = LPTMR_Delay;//Set comparison time

  LPTMR0_PSR = (0
                | LPTMR_PSR_PCS(1)      //Select LPO (1KHz) clock source
                | LPTMR_PSR_PBYP_MASK   //Bypass prescaler/interference filter
                );
  
  LPTMR0_CSR = (0 | LPTMR_CSR_TEN_MASK);//Enable LPTMR timer
  
  while((LPTMR0_CSR & LPTMR_CSR_TCF_MASK) != LPTMR_CSR_TCF_MASK);
  LPTMR0_CSR &= ~LPTMR_CSR_TCF_MASK;//Clear the comparison flag bit
  
}

/*
 *  @brief      LPTMR timing function(ms)
 *  @param      uint16          LPTMR_Delay      LPTMR timing function(0~65535)
 *  @since      v1.0
 *  Sample usage:
                    LPTMR_Timing_ms(32);                                // LPTMR timing 32ms
                    Set_Vector_Handler(LPTimer_VECTORn,lptmr_hander);   // Set the interrupt service function to the interrupt vector table
                    enable_irq(LPTimer_IRQn);                           // Enable LPTMR interrupt
 */
void LPTMR_Timing_ms(uint16 LPTMR_Delay)
{
  //Assert that the delay time is greater than 0
  ASSERT(LPTMR_Delay > 0);
  
  SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;//LPTMR clock enable
  
  LPTMR0_CSR = 0;//Clear LPTMR0_SR register
  LPTMR0_CMR = LPTMR_Delay;//Set comparison time
  
  LPTMR0_PSR = (0
                | LPTMR_PSR_PCS(1)      //Select LPO (1KHz) clock source
                | LPTMR_PSR_PBYP_MASK   //Bypass prescaler/interference filter
                );
  
  LPTMR0_CSR = (0 | LPTMR_CSR_TEN_MASK | LPTMR_CSR_TIE_MASK);//Enable LPTMR timer, enable LPTMR timer interrupt
  
}

/*
 *  @brief      LPTMR delay function(us)
 *  @param      uint16          LPTMR_Delay      LPTMR delay time(0~41942)
 *  @since      v1.0
 *  Sample usage:       LPTMR_Delay_us(32);     //LPTMR delay 32us
 */
void LPTMR_Delay_us(uint16 LPTMR_Delay)
{
  //Assert the timing duration to ensure that the register does not overflow
  ASSERT((((EXTAL_IN_MHz * LPTMR_Delay + 16) / 32) < 0xffff) && (LPTMR_Delay > 0));
  
  OSC_CR |= OSC_CR_ERCLKEN_MASK;//Enable external crystal oscillator reference clock
  
  SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;//LPTMR clock enable
  
  LPTMR0_CSR = 0;//Clear LPTMR0_SR register
  
  LPTMR0_CMR = (EXTAL_IN_MHz * LPTMR_Delay + 16) / 32;//Set comparison time
  
  LPTMR0_PSR = (0
                | LPTMR_PSR_PCS(3)      //Clock source selection OSC crystal oscillator clock
                | LPTMR_PSR_PRESCALE(4) //Pre division setting 2 ^ (n+1)
                );
  
  LPTMR0_CSR = (0 | LPTMR_CSR_TEN_MASK);//LPTMR clock enable
  
  while((LPTMR0_CSR & LPTMR_CSR_TCF_MASK) != LPTMR_CSR_TCF_MASK);
  LPTMR0_CSR &= ~LPTMR_CSR_TCF_MASK;//Clear the comparison flag bit
  
}

/*
 *  @brief      LPTMR timing function(us)
 *  @param      uint16          LPTMR_Delay      LPTMR delay time(0~41942)
 *  @since      v1.0
 *  Sample usage:
                    LPTMR_Timing_us(32);                                // LPTMR timing 32us
                    Set_Vector_Handler(LPTimer_VECTORn,lptmr_hander);   // Set the interrupt service function to the interrupt vector table
                    enable_irq(LPTimer_IRQn);                           // Enable LPTMR interrupt
 */
void LPTMR_Timing_us(uint16 LPTMR_Delay)
{
  //Assert the timing duration to ensure that the register does not overflow
  ASSERT((((EXTAL_IN_MHz * LPTMR_Delay + 16) / 32) < 0xffff) && (LPTMR_Delay > 0));
  
  OSC_CR |= OSC_CR_ERCLKEN_MASK;//Enable external crystal oscillator reference clock
  
  SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;//LPTMR clock enable
  
  LPTMR0_CSR = 0;//Clear LPTMR0_SR register
  
  LPTMR0_CMR = (EXTAL_IN_MHz * LPTMR_Delay + 16) / 32;//Set comparison time
  
  LPTMR0_PSR = (0
                | LPTMR_PSR_PCS(3)      //Clock source selection OSC crystal oscillator clock
                | LPTMR_PSR_PRESCALE(4) //Pre division setting 2 ^ (n+1)
                );
  
  LPTMR0_CSR = (0 | LPTMR_CSR_TEN_MASK | LPTMR_CSR_TIE_MASK);//LPTMR clock enable, LPTMR interrupt enable
  
}