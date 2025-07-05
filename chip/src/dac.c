/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       dac.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "dac.h"

/*
**DAC base pointer
*/
DAC_MemMapPtr DACN[DAC_DAC_MAX]=
{
  DAC0_BASE_PTR,//DAC0 base pointer
  DAC1_BASE_PTR,//DAC1 base pointer
};

/*
 *  @brief      DAC Init
 *  @param      DAC_DACn        DAC_DACx        DAC module number
 *  @since      v1.0
 *  Sample usage:       DAC_Init(DAC1);//Init DAC1
 */
void DAC_Init(DAC_DACn DAC_DACx)
{
  ASSERT(DAC_DACx <= DAC_DAC1);
  
  switch(DAC_DACx)
  {
    case DAC_DAC0:
                  SIM_SCGC2 |= SIM_SCGC2_DAC0_MASK;//DAC0 clock enablement
                  break;
    case DAC_DAC1:
                  SIM_SCGC2 |= SIM_SCGC2_DAC1_MASK;//DAC1 clock enablement
                  break;
    default:
                  ASSERT(0);
                  break;
  }
  
  /*
  **DAC_C0 register configuration
  */
  DAC_C0_REG(DACN[DAC_DACx]) = (0
                                | DAC_C0_DACTRGSEL_MASK         //DAC software trigger
                                | DAC_C0_DACRFS_MASK            //DAC reference source VDD (3.3V)
                                | DAC_C0_DACEN_MASK             //DAC system enable
                                );
  
  /*
  **DAC_C1 register configuration
  */
  DAC_C1_REG(DACN[DAC_DACx]) = (0);
  
  /*
  **DAC_C2 register configuration
  */
  DAC_C2_REG(DACN[DAC_DACx]) = (0 | DAC_C2_DACBFRP(0));
  
  //Output minimum voltage
  DAC_DATH_REG(DACN[DAC_DACx],0) = 0;
  DAC_DATL_REG(DACN[DAC_DACx],0) = 0;
    
}

/*
 *  @brief      DAC output
 *  @param      DAC_DACn        DAC_DACx        DAC module number
 *  @param      uint16          DAC_Value       Output the digital quantity corresponding to the analog quantity (12 bits)
 *  @since      v1.0
 *  Sample usage:       DAC_Out (DAC1 ,0x100);    //Initialize DAC1 to output analog quantity corresponding to 0x100 digital quantity
 */
void DAC_Out(DAC_DACn DAC_DACx,uint16 DAC_Value)
{
  //Assert that the DAC output value is 12 bits
  ASSERT(DAC_Value < 0x1000);
  
  DAC_DATH_REG(DACN[DAC_DACx],0) = (DAC_Value >> 8);    //DAC output high-order register
  DAC_DATL_REG(DACN[DAC_DACx],0) = (DAC_Value & 0xff);  //DAC output status register
  
}