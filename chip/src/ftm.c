/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       ftm.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "ftm.h"
#include "port.h"
#include "port_cfg.h"
#include "system_init.h"

/*
**FTM base pointer
*/
FTM_MemMapPtr FTMN[FTM_FTM_MAX]=
{
  FTM0_BASE_PTR,//FTM0 base pointer
  FTM1_BASE_PTR,//FTM1 base pointer
  FTM2_BASE_PTR,//FTM2 base pointer
  FTM3_BASE_PTR,//FTM3 base pointer
};

/*
 *  @brief      Initialize the port of FTM (PORT is reused as FTM)
 *  @param      FTM_FTMn FTM_FTMx module number(FTM0,FTM1,FTM2,FTM3)
 *  @param      FTM_CHn  FTM_CHx  channel number(FTM0&FTM3:CH0~CH7,FTM1&FTM2:CH0~CH1)
 *  @since      v1.0
 *  @note       static declaration
 */
static void FTM_PORT_MUX(FTM_FTMn FTM_FTMx,FTM_CHn FTM_CHx)
{
  switch(FTM_FTMx)//Select FTM module number
  {
    case FTM_FTM0://FTM0
                  SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;//FTM0 clock enable
                  switch(FTM_CHx)//select FTM channel FTM0_CH0~FTM0_CH7
                  {
                    case FTM_CH0:
                                 if(FTM0_CH0_PIN == PTC1)
                                 {
                                   PORT_Init(FTM0_CH0_PIN,ALT4);
                                 }
                                 else if(FTM0_CH0_PIN == PTA3)
                                 {
                                   PORT_Init(FTM0_CH0_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH1:
                                 if(FTM0_CH1_PIN == PTC2)
                                 {
                                   PORT_Init(FTM0_CH1_PIN,ALT4);
                                 }
                                 else if(FTM0_CH1_PIN == PTA4)
                                 {
                                   PORT_Init(FTM0_CH1_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH2:
                                 if(FTM0_CH2_PIN == PTC3)
                                 {
                                   PORT_Init(FTM0_CH2_PIN,ALT4);
                                 }
                                 else if(FTM0_CH2_PIN == PTA5)
                                 {
                                   PORT_Init(FTM0_CH2_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH3:
                                 if(FTM0_CH3_PIN == PTC4)
                                 {
                                   PORT_Init(FTM0_CH3_PIN,ALT4);
                                 }
                                 else if(FTM0_CH3_PIN == PTA6)
                                 {
                                   PORT_Init(FTM0_CH3_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH4:
                                 if(FTM0_CH4_PIN == PTD4)
                                 {
                                   PORT_Init(FTM0_CH4_PIN,ALT4);
                                 }
                                 else if(FTM0_CH4_PIN == PTA7)
                                 {
                                   PORT_Init(FTM0_CH4_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH5:
                                 if(FTM0_CH5_PIN == PTD5)
                                 {
                                   PORT_Init(FTM0_CH5_PIN,ALT4);
                                 }
                                 else if(FTM0_CH5_PIN == PTA0)
                                 {
                                   PORT_Init(FTM0_CH5_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH6:
                                 if(FTM0_CH6_PIN == PTD6)
                                 {
                                   PORT_Init(FTM0_CH6_PIN,ALT4);
                                 }
                                 else if(FTM0_CH6_PIN == PTA1)
                                 {
                                   PORT_Init(FTM0_CH6_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH7:
                                 if(FTM0_CH7_PIN == PTD7)
                                 {
                                   PORT_Init(FTM0_CH7_PIN,ALT4);
                                 }
                                 else if(FTM0_CH7_PIN == PTA2)
                                 {
                                   PORT_Init(FTM0_CH7_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    default:
                                 ASSERT(0);
                                 break;                                 
                  }
                  break;
    case FTM_FTM1://FTM1
                  SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;//FTM1 clock enable
                  switch(FTM_CHx)//select FTM channel FTM1_CH0~FTM1_CH1
                  {
                    case FTM_CH0:
                                 if((FTM1_CH0_PIN == PTA8) || (FTM1_CH0_PIN == PTA12) || (FTM1_CH0_PIN == PTB0))
                                 {
                                   PORT_Init(FTM1_CH0_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH1:
                                 if((FTM1_CH1_PIN == PTA9) || (FTM1_CH1_PIN == PTA13) || (FTM1_CH1_PIN == PTB1))
                                 {
                                   PORT_Init(FTM1_CH1_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    default:
                                 ASSERT(0);
                                 break;
                  }
                  break;
    case FTM_FTM2://FTM2
                  SIM_SCGC6 |= SIM_SCGC6_FTM2_MASK;//FTM2 clock enable
                  SIM_SCGC3 |= SIM_SCGC3_FTM2_MASK;//FTM2 clock enable
                  switch(FTM_CHx)//select FTM channel FTM2_CH0~FTM2_CH1
                  {
                    case FTM_CH0:
                                 if((FTM2_CH0_PIN == PTA10) || (FTM2_CH0_PIN == PTB18))
                                 {
                                   PORT_Init(FTM2_CH0_PIN,ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH1:
                                 if((FTM2_CH1_PIN == PTA11) || (FTM2_CH1_PIN == PTB19))
                                 {
                                   PORT_Init(FTM2_CH1_PIN, ALT3);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    default:
                                 ASSERT(0);
                                 break;
                  }
                  break;
    case FTM_FTM3://FTM3
                  SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK;//FTM3 clock enable
                  switch(FTM_CHx)//select FTM channel FTM3_CH0~FTM3_CH7
                  {
                    case FTM_CH0:
                                 if(FTM3_CH0_PIN == PTD0)
                                 {
                                   PORT_Init(FTM3_CH0_PIN,ALT4);
                                 }
                                 else if(FTM3_CH0_PIN == PTE5)
                                 {
                                   PORT_Init(FTM3_CH0_PIN,ALT6);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH1:
                                 if(FTM3_CH1_PIN == PTD1)
                                 {
                                   PORT_Init(FTM3_CH1_PIN,ALT4);
                                 }
                                 else if(FTM3_CH1_PIN == PTE6)
                                 {
                                   PORT_Init(FTM3_CH1_PIN,ALT6);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH2:
                                 if(FTM3_CH2_PIN == PTD2)
                                 {
                                   PORT_Init(FTM3_CH2_PIN,ALT4);
                                 }
                                 else if(FTM3_CH2_PIN == PTE7)
                                 {
                                   PORT_Init(FTM3_CH2_PIN,ALT6);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH3:
                                 if(FTM3_CH3_PIN == PTD3)
                                 {
                                   PORT_Init(FTM3_CH3_PIN,ALT4);
                                 }
                                 else if(FTM3_CH3_PIN == PTE8)
                                 {
                                   PORT_Init(FTM3_CH3_PIN,ALT6);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH4:
                                 if(FTM3_CH4_PIN == PTC8)
                                 {
                                   PORT_Init(FTM3_CH4_PIN,ALT3);
                                 }
                                 else if(FTM3_CH4_PIN == PTE9)
                                 {
                                   PORT_Init(FTM3_CH4_PIN,ALT6);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH5:
                                 if(FTM3_CH5_PIN == PTC9)
                                 {
                                   PORT_Init(FTM3_CH5_PIN,ALT3);
                                 }
                                 else if(FTM3_CH5_PIN == PTE10)
                                 {
                                   PORT_Init(FTM3_CH5_PIN,ALT6);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH6:
                                 if(FTM3_CH6_PIN == PTC10)
                                 {
                                   PORT_Init(FTM3_CH6_PIN,ALT3);
                                 }
                                 else if(FTM3_CH6_PIN == PTE11)
                                 {
                                   PORT_Init(FTM3_CH6_PIN,ALT6);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    case FTM_CH7:
                                 if(FTM3_CH7_PIN == PTC11)
                                 {
                                   PORT_Init(FTM3_CH7_PIN,ALT3);
                                 }
                                 else if(FTM3_CH7_PIN == PTE12)
                                 {
                                   PORT_Init(FTM3_CH7_PIN,ALT6);
                                 }
                                 else
                                 {
                                   ASSERT(0);
                                 }
                                 break;
                    default:
                                 ASSERT(0);
                                 break;                                 
                  }
                  break;
    default:
                  ASSERT(0);
                  break;                  
  }
  
}

/*
 *  @brief      Initialize the PWM function of FTM
 *  @param      FTM_FTMn FTM_FTMx module number(FTM0,FTM1,FTM2,FTM3)
 *  @param      FTM_CHn  FTM_CHx  channel number(FTM0&FTM3:CH0~CH7,FTM1&FTM2:CH0~CH1)
 *  @param      uint32   FTM_Freq frequency(Hz)
 *  @param      uint32   FTM_Duty Duty cycle molecule, duty cycle=FTM-Duty/FTMn-PRECISON
 *  @since      v1.0
 *  @note       The same FTM must have the same PWM frequency, but the duty cycle can be different There are a total of 4 FTMs, which can output 4 different frequency PWM. It is recommended to use FTM0 and FTM3 for motor control
 *  Sample usage:       FTM_PWM_Init(FTM_FTM0, FTM_CH6,200, 10);    //Initialize FTM0_C6 as a PWM with a frequency of 200Hz and a duty cycle of 10/FTM0-PRECISON
 */
void FTM_PWM_Init(FTM_FTMn FTM_FTMx,FTM_CHn FTM_CHx,uint32 FTM_Freq,uint32 FTM_Duty)
{
  uint8  FTM_Ps=0;      //FTM division factor 2 ^ FTM-Ps
  uint16 FTM_Mod=0;     //FTM modulus value, FTM pulse period
  uint16 FTM_Cv=0;      //FTM pulse cycle high level duration
  uint32 FTM_Clk_Hz=0;  //FTM clock frequency, select the bus clock, which is Bus_Clock
  
  //FTM asserts that FTM1 and FTM2 only have CH0 and CH1; FTM0 and FTM3 have CH0-CH7
  ASSERT(((FTM_FTMx == FTM_FTM0 || FTM_FTMx == FTM_FTM3) && (FTM_CHx <= FTM_CH7))
         || ((FTM_FTMx == FTM_FTM1 || FTM_FTMx == FTM_FTM2) && (FTM_CHx <= FTM_CH1)));
  
  FTM_PORT_MUX(FTM_FTMx,FTM_CHx);//FTM clock port reuse
  
  //The formula for calculating PWM frequency is: PWM_Sreq=(MK66_Sus_Clock)/(2 ^ FTM-PS * (MOD - CNTIN+1))
  //The initial count value CNTIN is set to 0, and the frequency calculation formula can be simplified as: PWM_Sreq=(MK66_Sus_Clock)/(2 ^ FTM-PS * (MOD+1))
  FTM_Clk_Hz = MK64_Bus_KHz * 1000;//FTM clock frequency (Hz)
  FTM_Mod = (FTM_Clk_Hz >> 16) / FTM_Freq;//FTM-Mod temporary cache, (MOD+1) first set to the maximum value (0xffff+0x1=0x10000, i.e. 2 ^ 16, shifted 16 bits to the right, calculate the value of FTM-PS)
  FTM_Ps=0;
  
  while((FTM_Mod >> FTM_Ps) >= 1)//Find the minimum value of FTM-Ps that satisfies the frequency division condition
  {
    FTM_Ps++;
  }
  
  ASSERT(FTM_Ps <= 0x07);//Assumption: FTM-Ps (0~7), the maximum value of FTM-Ps is 0x07. If it exceeds 0x07, the PWM frequency is set too low or the Bus frequency is set too high
  
  FTM_Mod = (FTM_Clk_Hz >> FTM_Ps) / FTM_Freq;//Calculate the value of FTM-Mod and FTM cycle value
  
  //EPWM edge alignment modePWM,FTM_Cv / FTM_PRECISON = FTM_Duty / (FTM_Mod + 1)
  
  switch(FTM_FTMx)//Select FTM module and calculate duty cycle
  {
    case FTM_FTM0:
                  FTM_Cv = (FTM_Duty * (FTM_Mod + 1)) / FTM0_PRECISON;
                  break;
    case FTM_FTM1:
                  FTM_Cv = (FTM_Duty * (FTM_Mod + 1)) / FTM1_PRECISON;
                  break;
    case FTM_FTM2:
                  FTM_Cv = (FTM_Duty * (FTM_Mod + 1)) / FTM2_PRECISON;
                  break;
    case FTM_FTM3:
                  FTM_Cv = (FTM_Duty * (FTM_Mod + 1)) / FTM3_PRECISON;
                  break;
    default:
                  ASSERT(0);
                  break;
  }
  
  //Channel state control, selecting edges or levels based on the mode
  FTM_CnSC_REG(FTMN[FTM_FTMx],FTM_CHx) &= ~FTM_CnSC_ELSA_MASK;
  FTM_CnSC_REG(FTMN[FTM_FTMx],FTM_CHx) |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
  
  FTM_SC_REG(FTMN[FTM_FTMx]) = (0 | FTM_SC_PS(FTM_Ps) | FTM_SC_CLKS(1));//Select bus clock, clock divided by 2 ^ FTM-Ps
  FTM_MOD_REG(FTMN[FTM_FTMx]) = FTM_Mod;//FTM modulus value, FTM pulse period
  FTM_CNTIN_REG(FTMN[FTM_FTMx]) = 0;//FTM timer initial value, set to 0
  FTM_CnV_REG(FTMN[FTM_FTMx],FTM_CHx) = FTM_Cv;//FTM counter, FTM high level continuous period
  FTM_CNT_REG(FTMN[FTM_FTMx]) = 0;//Reset the counter value to zero
  
}

/*
 *  @brief      Set the PWM channel duty cycle of FTM
 *  @param      FTM_FTMn FTM_FTMx module number(FTM0,FTM1,FTM2,FTM3)
 *  @param      FTM_CHn  FTM_CHx  channel number(FTM0&FTM3:CH0~CH7,FTM1&FTM2:CH0~CH1)
 *  @param      uint32   FTM_Duty duty cycle molecule, duty cycle= FTM_Duty / FTMn_PRECISON
 *  @since      v1.0
 *  @note       The same FTM must have the same PWM frequency, but the duty cycle can be different. There are a total of 4 FTMs, which can output 4 different frequency PWM
 *  Sample usage:       FTM_PWM_Duty(FTM_FTM0, FTM_CH6, 10);    //Set the duty cycle of FTM0_CH6 to 10/FTM0-PRECISON
 */
void FTM_PWM_Duty(FTM_FTMn FTM_FTMx,FTM_CHn FTM_CHx,uint32 FTM_Duty)
{
  uint32 FTM_Cv=0;
  uint32 FTM_Mod=0;
  
  //FTM asserts that FTM1 and FTM2 only have CH0 and CH1; FTM0 and FTM3 have CH0-CH7
  ASSERT(((FTM_FTMx == FTM_FTM0 || FTM_FTMx == FTM_FTM3) && (FTM_CHx <= FTM_CH7))
         || ((FTM_FTMx == FTM_FTM1 || FTM_FTMx == FTM_FTM2) && (FTM_CHx <= FTM_CH1)));
  
  switch(FTM_FTMx)//Assert whether the monitoring FTM duty cycle meets the accuracy requirements
  {
    case FTM_FTM0:
                  ASSERT(FTM_Duty <= FTM0_PRECISON);
                  break;
    case FTM_FTM1:
                  ASSERT(FTM_Duty <= FTM1_PRECISON);
                  break;
    case FTM_FTM2:
                  ASSERT(FTM_Duty <= FTM2_PRECISON);
                  break;
    case FTM_FTM3:
                  ASSERT(FTM_Duty <= FTM3_PRECISON);
                  break;
    default:
                  ASSERT(0);
                  break;
  }
  
  do
  {
    FTM_Mod = FTM_MOD_REG(FTMN[FTM_FTMx]);//Read MOD value
  }while(FTM_Mod == 0);//Read the MOD value multiple times until it reaches non-zero
  
  //EPWM edge alignment mode PWM,FTM_Cv / FTM_PRECISON = FTM_Duty / (FTM_Mod + 1)
  
  switch(FTM_FTMx)//PWM duty calculate
  {
    case FTM_FTM0:
                  FTM_Cv = (FTM_Duty * (FTM_Mod + 1)) / FTM0_PRECISON;
                  break;
    case FTM_FTM1:
                  FTM_Cv = (FTM_Duty * (FTM_Mod + 1)) / FTM1_PRECISON;
                  break;
    case FTM_FTM2:
                  FTM_Cv = (FTM_Duty * (FTM_Mod + 1)) / FTM2_PRECISON;
                  break;
    case FTM_FTM3:
                  FTM_Cv = (FTM_Duty * (FTM_Mod + 1)) / FTM3_PRECISON;
                  break;
    default:
                  ASSERT(0);
                  break;
  }
  
  FTM_CnV_REG(FTMN[FTM_FTMx],FTM_CHx) = FTM_Cv;//Set FTM high level count value
  
}

/*
 *  @brief      Set the frequency of FTM
 *  @param      FTM_FTMn FTM_FTMx module number(FTM0,FTM1,FTM2,FTM3)
 *  @param      FTM_CHn  FTM_CHx  channel number(FTM0&FTM3:CH0~CH7,FTM1&FTM2:CH0~CH1)
 *  @since      v1.0
 *  @note       After modifying the PWM frequency, it is necessary to call FTM-PWM_Suty to reconfigure the duty cycle. The same module must have the same PWM frequency
 *  Sample usage:       FTM_PWM_Freq(FTM_FTM0,200);    //Set the frequency of FTM0 to 200Hz
 */
void FTM_PWM_Freq(FTM_FTMn FTM_FTMx,uint32 FTM_Freq)
{
  uint8  FTM_Ps=0;      //FTM division factor 2 ^ FTM-Ps
  uint32 FTM_Mod=0;     //FTM modulus value, FTM pulse period
  uint32 FTM_Clk_Hz=0;  //FTM clock frequency, select the bus clock, which is Bus_Clock
  
  FTM_Clk_Hz = MK64_Bus_KHz * 1000;//Bus frequency
  FTM_Mod = (FTM_Clk_Hz >> 16) / FTM_Freq;//FTM-Mod Temporary Cache
  FTM_Ps=0;
  
  while((FTM_Mod >> FTM_Ps) >= 1)//Find the minimum value of FTM-Ps
  {
    FTM_Ps++;
  }
  
  ASSERT(FTM_Ps <= 0x07);//The assertion is that the maximum value of FTM-Ps is 0x07. If it exceeds 0x07, the PWM frequency is set too low or the Bus frequency is set too high
  
  FTM_Mod = (FTM_Clk_Hz >> FTM_Ps) / FTM_Freq;//Calculate the value of FTM-Mod
  
  //FTM frequency clock configuration
  FTM_SC_REG(FTMN[FTM_FTMx]) = (0 | FTM_SC_PS(FTM_Ps) | FTM_SC_CLKS(1));//Select bus clock, clock divided by 2 ^ FTM-Ps
  FTM_MOD_REG(FTMN[FTM_FTMx]) = FTM_Mod;//FTM cycle FTM-MOD+1
  FTM_CNTIN_REG(FTMN[FTM_FTMx]) = 0;//FTM count initial value 0
  FTM_CNT_REG(FTMN[FTM_FTMx]) = 0;//FTM counter reset to zero
  
}

/*
 *  @brief      Input capture initialization function
 *  @param      FTM_FTMn        FTM_FTMx module number(FTM0,FTM1,FTM2,FTM3)
 *  @param      FTM_CHn         FTM_CHx  channel number(FTM0&FTM3:CH0~CH7,FTM1&FTM2:CH0~CH1)
 *  @param      FTM_INPUT_CFG   FTM_CFG  Input capture trigger condition configuration
 *  @param      FTM_PS_n        FTM_PS_x Frequency division factor
 *  @since      v1.0
 *  @note       The same FTM can only perform one task, and after being used for input capture, it should not be used for other functions such as PWM.
 *  Sample usage:       FTM_Input_Init(FTM_FTM0, FTM_CH0, FTM_Rising,FTM_PS_2);    //Set FTM0_C0 rising edge trigger input capture, binary division
 */
void FTM_Input_Init(FTM_FTMn FTM_FTMx,FTM_CHn FTM_CHx,FTM_INPUT_CFG FTM_CFG,FTM_PS_n FTM_PS_x)
{
  //FTM asserts that FTM1 and FTM2 only have CH0 and CH1; FTM0 and FTM3 have CH0-CH7
  ASSERT(((FTM_FTMx == FTM_FTM0 || FTM_FTMx == FTM_FTM3) && (FTM_CHx <= FTM_CH7))
         || ((FTM_FTMx == FTM_FTM1 || FTM_FTMx == FTM_FTM2) && (FTM_CHx <= FTM_CH1)));
  
  FTM_PORT_MUX(FTM_FTMx,FTM_CHx);//FTM clock port reuse
  
  switch(FTM_CFG)//Select input capture trigger mode
  {
    case FTM_Rising://Rising edge capture
                               FTM_CnSC_REG(FTMN[FTM_FTMx],FTM_CHx) |= (FTM_CnSC_ELSA_MASK | FTM_CnSC_CHIE_MASK);//ELSnB:ELSnA=01,Rising edge capture
                               FTM_CnSC_REG(FTMN[FTM_FTMx],FTM_CHx) &= ~(FTM_CnSC_ELSB_MASK  | FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK);
                               break;
    case FTM_Falling://Falling edge capture
                               FTM_CnSC_REG(FTMN[FTM_FTMx],FTM_CHx) |= (FTM_CnSC_ELSB_MASK  | FTM_CnSC_CHIE_MASK);//ELSnB:ELSnA=10,Falling edge capture
                               FTM_CnSC_REG(FTMN[FTM_FTMx],FTM_CHx) &= ~(FTM_CnSC_ELSA_MASK | FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK);
                               break;
    case FTM_Rising_or_Falling://Jump edge capture
                               FTM_CnSC_REG(FTMN[FTM_FTMx],FTM_CHx) |= (FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK  | FTM_CnSC_CHIE_MASK);
                               FTM_CnSC_REG(FTMN[FTM_FTMx],FTM_CHx) &= ~(FTM_CnSC_MSB_MASK  | FTM_CnSC_MSA_MASK);//ELSnB:ELSnA=11,Jump edge capture
                               break;
    default:
                               ASSERT(0);
                               break;
  }
  
  FTM_SC_REG(FTMN[FTM_FTMx]) = (0 | FTM_SC_CLKS(1) | FTM_SC_PS(FTM_PS_x));//Select Bus clock, division factor
  FTM_MODE_REG(FTMN[FTM_FTMx]) |= FTM_MODE_WPDIS_MASK;//Prohibit writing protection
  FTM_COMBINE_REG(FTMN[FTM_FTMx]) = 0;//Channel settings are not cascaded
  FTM_MODE_REG(FTMN[FTM_FTMx]) &= ~FTM_MODE_FTMEN_MASK;//FTM enable (enable input capture function and QUAD orthogonal decoder function)
  FTM_CNTIN_REG(FTMN[FTM_FTMx]) = 0;//The initial count value is 0
  FTM_STATUS_REG(FTMN[FTM_FTMx]) = 0;//Clear interrupt flag
  
}

/*
 *  @brief      Input capture to obtain pulse function
 *  @param      FTM_FTMn        FTM_FTMx module number(FTM0,FTM1,FTM2,FTM3)
 *  @param      FTM_CHn         FTM_CHx  channel number(FTM0&FTM3:CH0~CH7,FTM1&FTM2:CH0~CH1)
 *  @return     obtain the counter value when the capture event occurs (the cycle frequency can be determined based on the difference between two captures)
 *  @since      v1.0
 *  Sample usage:       uint16 data = FTM_Input_Get(FTM_FTM0, FTM_CH0) ;   // Get the count value when FTM0_C0 input captures the event trigger
 */
uint16 FTM_Input_Get(FTM_FTMn FTM_FTMx,FTM_CHn FTM_CHx)
{
  uint16 FTM_Get_Count=0;
  
  //FTM asserts that FTM1 and FTM2 only have CH0 and CH1; FTM0 and FTM3 have CH0-CH7
  ASSERT(((FTM_FTMx == FTM_FTM0 || FTM_FTMx == FTM_FTM3) && (FTM_CHx <= FTM_CH7))
         || ((FTM_FTMx == FTM_FTM1 || FTM_FTMx == FTM_FTM2) && (FTM_CHx <= FTM_CH1)));
  
  FTM_Get_Count = (uint16)FTM_CnV_REG(FTMN[FTM_FTMx],FTM_CHx);//Captured FTM count value
  
  return FTM_Get_Count;
  
}

/*
 *  @brief      Input Capture Clear Counter Value functions
 *  @param      FTM_FTMn        FTM_FTMx module number(FTM0,FTM1,FTM2,FTM3)
 *  @return     Clear counter count value
 *  @since      v1.0
 *  Sample usage:       FTM_Input_Clean(FTM_FTM0)
 */
void FTM_Input_Clean(FTM_FTMn FTM_FTMx)
{
  //Assert FTM module number
  ASSERT(FTM_FTMx <= FTM_FTM3);
  
  FTM_CNT_REG(FTMN[FTM_FTMx]) = 0;//FTM count value reset to zero
}

/*
 *  @brief      Initialize the orthogonal decoding function of FTM (only FTM1 and FTM2 have orthogonal decoder mode)
 *  @param      FTM_FTMn        FTM_FTMx module number(FTM1,FTM2)
 *  @since      v1.0
 *  Sample usage:       FTM_QUAD_Init(FTM1);    //Initialize FTM1 to orthogonal decoding mode
 */
void FTM_QUAD_Init(FTM_FTMn FTM_FTMx)
{
  //Assert that QUAD orthogonal decoding only has FTM1 and FTM2
  ASSERT((FTM_FTMx == FTM_FTM1) || (FTM_FTMx == FTM_FTM2));
  
  switch(FTM_FTMx)//Select FTM module to initialize QUAD
  {
    case FTM_FTM1:
                  SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;//FTM1 clock enable
                  //FTM1_CH0
                  if((FTM1_QDPHA_PIN == PTA8) || (FTM1_QDPHA_PIN == PTB0))
                  {
                    PORT_Init(FTM1_QDPHA_PIN,ALT6);
                  }
                  else if(FTM1_QDPHA_PIN == PTA12)
                  {
                    PORT_Init(FTM1_QDPHA_PIN,ALT7);
                  }
                  else
                  {
                    ASSERT(0);
                  }
                  //FTM1_CH1
                  if((FTM1_QDPHB_PIN == PTA9) || (FTM1_QDPHB_PIN == PTB1))
                  {
                    PORT_Init(FTM1_QDPHB_PIN,ALT6);
                  }
                  else if(FTM1_QDPHB_PIN == PTA13)
                  {
                    PORT_Init(FTM1_QDPHB_PIN,ALT7);
                  }
                  else
                  {
                    ASSERT(0);
                  }
                  break;
    case FTM_FTM2:
                  SIM_SCGC6 |= SIM_SCGC6_FTM2_MASK;//FTM2 clock enable
                  SIM_SCGC3 |= SIM_SCGC3_FTM2_MASK;
                  //FTM2_CH0
                  if((FTM2_QDPHA_PIN == PTA10) || (FTM2_QDPHA_PIN == PTB18))
                  {
                    PORT_Init(FTM2_QDPHA_PIN,ALT6);
                  }
                  else
                  {
                    ASSERT(0);
                  }
                  //FTM2_CH1
                  if((FTM2_QDPHB_PIN == PTA11) || (FTM2_QDPHB_PIN == PTB19))
                  {
                    PORT_Init(FTM2_QDPHB_PIN,ALT6);
                  }
                  else
                  {
                    ASSERT(0);
                  }
                  break;
    default:
                  ASSERT(0);
                  break;
  }
  
  FTM_MODE_REG(FTMN[FTM_FTMx]) |= (0 | FTM_MODE_WPDIS_MASK);//Prohibit writing protection
  
  //FTM-QUAD orthogonal decoding mode, direct counting mode
  FTM_QDCTRL_REG(FTMN[FTM_FTMx]) |= (0 | FTM_QDCTRL_QUADMODE_MASK);
  //FTM count initial value set to 0
  FTM_CNTIN_REG(FTMN[FTM_FTMx]) = 0;
  //Set the final value of FTM counting cycle to 0xffff
  FTM_MOD_REG(FTMN[FTM_FTMx]) |= FTM_MOD_MOD_MASK;
  //FTM-QUAD orthogonal decoding mode enabled
  FTM_QDCTRL_REG(FTMN[FTM_FTMx]) |= (0 | FTM_QDCTRL_QUADEN_MASK);
  //FTM enable
  FTM_MODE_REG(FTMN[FTM_FTMx]) |= FTM_MODE_FTMEN_MASK;
  //FTM counter clean
  FTM_CNT_REG(FTMN[FTM_FTMx]) = 0;
  
}

/*
 *  @brief      Obtain the number of pulses for FTM orthogonal decoding
 *  @param      FTM_FTMn        FTM_FTMx module number(FTM1,FTM2)
 *  @since      v1.0
 *  Sample usage:       int16 FTM_Count = FTM_QUAD_Get(FTM_FTM1);    //Obtain the number of pulses for FTM1 decoding
 */
int16 FTM_QUAD_Get(FTM_FTMn FTM_FTMx)
{
  int16 FTM_QUAD_Count=0;
  
  //Assert that QUAD orthogonal decoding only has FTM1 and FTM2
  ASSERT((FTM_FTMx == FTM_FTM1) || (FTM_FTMx == FTM_FTM2));
  
  FTM_QUAD_Count = FTM_CNT_REG(FTMN[FTM_FTMx]);//QUAD orthogonal decoding obtains counter values
  
  return FTM_QUAD_Count;
}

/*
 *  @brief      Clear the number of pulses for FTM orthogonal decoding
 *  @param      FTM_FTMn        FTM-FTMx module number (FTM1, FTM2)
 *  @since      v1.0
 *  Sample usage:       FTM_QUAD_Clean(FTM_FTM1);    //Clear the number of pulses for FTM orthogonal decoding
 */
void FTM_QUAD_Clean(FTM_FTMn FTM_FTMx)
{
  //Assert that QUAD orthogonal decoding only has FTM1 and FTM2
  ASSERT((FTM_FTMx == FTM_FTM1) || (FTM_FTMx == FTM_FTM2));
  
  FTM_CNT_REG(FTMN[FTM_FTMx]) = 0;//QUAD orthogonal decoding counter value reset to zero
}