/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       adc.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __ADC_H__
#define __ADC_H__

#include "common.h"

/*
**ADC port enumeration (ADC0, ADC1)
*/
typedef enum
{
  /*
  **ADC0
  */
  ADC0_DP0,
  ADC0_DP1,
  PGA0_DP,        //ADC not supported, ADC PGA register needs to be configured to set amplification gain
  ADC0_DP3,
  
  ADC0_SE4b,      // PTC2     not support software ADC
  ADC0_SE5b,      // PTD1     not support software ADC
  ADC0_SE6b,      // PTD5     not support software ADC
  ADC0_SE7b,      // PTD6     not support software ADC
  
  ADC0_SE8,       // PTB0
  ADC0_SE9,       // PTB1
  ADC0_SE10,      // PTA7
  ADC0_SE11,      // PTA8
  ADC0_SE12,      // PTB2
  ADC0_SE13,      // PTB3
  ADC0_SE14,      // PTC0
  ADC0_SE15,      // PTC1
  ADC0_SE16,      // ADC0_SE16
  ADC0_SE17,      // PTE24
  ADC0_SE18,      // PTE25
  ADC0_DM0,       // ADC0_DM0
  ADC0_DM1,       // ADC0_DM1
  ADC0_SE21,      // reserved
  ADC0_SE22,      // reserved
  DAC0_OUT,       // DAC0 output does not support ADC
  RES2,           // reserved
  RES3,           // reserved
  Temp0_Sensor,   // Temperature Sensor, Internal temperature measurement, using ADC function
  Bandgap0,       // Temperature compensation structure bandgap reference source does not support ADC
  RES4,           // reserved
  VREFH0,         // Referring to high voltage, ADC function can be used, and the result is always 2 ^ n-1
  VREFL0,         // Referring to low voltage, ADC function can be used, and the result is always 0
  Module0_Dis,    // Does not support ADC
  
  /*
  **ADC1
  */
  ADC1_DP0,
  ADC1_DP1,
  PGA1_DP,        // not support ADC
  ADC1_DP3,
  ADC1_SE4a,      // PTE0
  ADC1_SE5a,      // PTE1
  ADC1_SE6a,      // PTE2
  ADC1_SE7a,      // PTE3
  
  ADC1_SE4b = ADC1_SE4a,  // PTC8    Channel B does not support software ADC. Passing ADC1_SE4b to the software ADC function will be treated as ADC1_SE4a.
  ADC1_SE5b = ADC1_SE5a,  // PTC9    B channel does not support software ADC
  ADC1_SE6b = ADC1_SE6a,  // PTC10   B channel does not support software ADC
  ADC1_SE7b = ADC1_SE7a,  // PTC11   B channel does not support software ADC
  
  ADC1_SE8,       // PTB0
  ADC1_SE9,       // PTB1
  ADC1_SE10,      // PTB4
  ADC1_SE11,      // PTB5
  ADC1_SE12,      // PTB6
  ADC1_SE13,      // PTB7
  ADC1_SE14,      // PTB10
  ADC1_SE15,      // PTB11
  ADC1_SE16,      // ADC1_SE16
  ADC1_SE17,      // PTA17
  VREF_OUTPUT,    // VREF Output
  ADC1_DM0,       // ADC1_DM0
  ADC1_DM1,       // ADC1_DM1
  RES5,           // reserved
  RES6,           // reserved
  DAC1_OUT,       // DAC1 output does not support ADC
  RES7,           // reserved
  RES8,           // reserved
  Temp1_Sensor,   // Temperature Sensor, Internal temperature measurement, using ADC function
  Bandgap1,       // Temperature compensation structure bandgap reference source does not support ADC
  RES9,           // reserved
  VREFH1,         // Referring to high voltage, ADC function can be used, and the result is always 2 ^ n-1
  VREFL1,         // Referring to low voltage, ADC function can be used, and the result is always 0
  Module1_Dis,    // Does not support ADC
}ADCn_CHn;

/*
**ADC module enumeration
*/
typedef enum
{
  ADC_ADC0,
  ADC_ADC1,
  ADC_ADC_MAX,
}ADC_ADCn;

/*
**ADC precision bits
*/
typedef enum
{
  ADC_8Bit=0x00,
  ADC_10Bit=0x02,
  ADC_12Bit=0x01,
  ADC_16Bit=0x03,
}ADC_nBit;

/*
**ADC channel enumeration (A channel, B channel)
*/
typedef enum
{
  ADC_Channel_A,
  ADC_Channel_B,
}ADC_Channel_n;

/*
**ADC module number macro
*/
#define ADCn(ADCn_CHx)          ((ADCn_CHx)>>5)  //ADCn=ADC_CHx/32
#define ADC_CHn(ADCn_CHx)       ((ADCn_CHx)&0x1f)//ADC_CHn=ADC_CHx%31

/*
**variate declaration
*/
extern ADC_MemMapPtr ADCN[ADC_ADC_MAX];

/*
**function declaration
*/
extern void ADC_Init(ADCn_CHn ADCn_CHx);
//extern void ADC_Start(ADCn_CHn ADCn_CHx,ADC_nBit ADC_xBit);
extern void ADC_Stop(ADC_ADCn ADC_ADCx);
extern uint16 ADC_Once(ADCn_CHn ADCn_CHx,ADC_nBit ADC_xBit);
extern uint16 ADC_Average(ADCn_CHn ADCn_CHx,ADC_nBit ADC_xBit,uint8 ADC_Samp_Num);
extern void ADC_Samp_Array(ADCn_CHn ADCn_CHx,ADC_nBit ADC_xBit,uint16 *ADC_Array);


#endif