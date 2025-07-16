/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       rtc.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "rtc.h"
#include <time.h>

/*
 *  @brief      RTC Initialization
 *  @since      v1.0
 *  Sample usage:       RTC_Init();    //RTC Initialize
 */
void RTC_Init(void)
{
  volatile uint32 RTC_Delay=0;
  
  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;//RTC clock enable
  SIM->SOPT1 |= SIM_SOPT1_OSC32KSEL(0x2);//RTC clock source use LSE
  
  PMC->REGSC |= PMC_REGSC_BGEN_MASK;
  PMC->REGSC |= PMC_REGSC_BGBE_MASK;
  
  RTC_CR = RTC_CR_SWR_MASK;     //RTC register reset
  RTC_CR &= ~RTC_CR_SWR_MASK;   //RTC software reset
  
  RTC_CR =(0
           | RTC_CR_OSCE_MASK   //Enable 32.768KHz crystal oscillator
           | RTC_CR_SC16P_MASK  //Enable 16pF bypass capacitor
           | RTC_CR_CLKO_MASK   //32.768KHz wave does not output to the outside
           | RTC_CR_UM_MASK     //Enable update mode
           );
  
  PMC->REGSC |= PMC_REGSC_ACKISO_MASK;
  
  //Waiting for the crystal oscillator to stabilize
  RTC_Delay = 0x600000;
  while(RTC_Delay--);
  
  //Set time compensation
  RTC_TCR = (0
             | RTC_TCR_CIR(0)   //Crystal oscillator compensation time is 0
             | RTC_TCR_TCR(0)   //32.768KHz crystal oscillator time compensation (uncompensated)
             );
  
  RTC_SR &= ~RTC_SR_TCE_MASK;   //Disable RTC counter 
  
  //Time and alarm settings
  RTC_TSR = 0;                  //The initial value of the second counter is 0
  RTC_TAR = 0;                  //Alarm clock counter value 0
  RTC_IER = 0;                  //Disable interrupt
  
  RTC_SR |= RTC_SR_TCE_MASK;    //Enable RTC counter
}

/*
 *  @brief      Set the current time
 *  @since      v1.0
 */
void RTC_Set_Time(uint32 Seconds)
{
  RTC_SR &= ~RTC_SR_TCE_MASK;   //Disable RTC counter
  RTC_TSR = Seconds;            //Set the initial value time of the counter
  RTC_SR |= RTC_SR_TCE_MASK;    //Enable RTC counter
}

/*
 *  @brief      Set the current time format
 *  @since      v1.0
 */
void RTC_Set_Time_Format(struct tm *timeinfo)
{
  //Convert struct tm to seconds
  time_t rawtime = mktime(timeinfo);
  uint32 Seconds = (uint32)rawtime;

  //Set the RTC time
  RTC_Set_Time(Seconds);
}

/*
 *  @brief      Get the current time
 *  @since      v1.0
 */
uint32 RTC_Get_Time(void)
{
  return RTC_TSR;               //After counting starts, the RTC_TSR register reads the current counting time
}

/*
 *  @brief      Get the current time format
 *  @since      v1.0
 */
struct tm RTC_Get_Time_Format(void)
{
  struct tm *timeinfo;
  time_t rawtime = RTC_Get_Time(); //Get the current time in seconds
  timeinfo = localtime(&rawtime); //Convert to local time structure
  return *timeinfo; //Return the time structure
}

/*
 *  @brief      Set RTC alarm time to enable alarm interruption
 *  @param      uint32  Alarm   alarm time
 *  @return     Set alarm result (0 indicates failure, 1 indicates success)
 *  @since      v1.0
 */
uint8 RTC_Set_Alarm(uint32 Alarm)
{
  //The alarm time cannot be less than the current initial timer time
  if(Alarm < RTC_TSR)
  {
    return 0;
  }
  
  RTC_SR &= ~RTC_SR_TCE_MASK;   //Disable RTC counter
  RTC_TAR = Alarm;              //Set Alarm Time
  RTC_SR |= RTC_SR_TCE_MASK;    //Enable RTC counter
  
  RTC_IER |= RTC_IER_TAIE_MASK; //Alarm clock interrupt enable
  
  return 1;
}

/*
 *  @brief      Set RTC alarm time to enable alarm interruption
 *  @param      uint32  Alarm   alarm time
 *  @return     Set alarm result (0 indicates failure, 1 indicates success)
 *  @since      v1.0
 */
uint8 RTC_Set_Alarm_Format(struct tm *timeinfo)
{
  //Convert struct tm to seconds
  time_t rawtime = mktime(timeinfo);
  uint32 Seconds = (uint32)rawtime;
  //Set the RTC alarm time
  return RTC_Set_Alarm(Seconds);
}

/*
 *  @brief      Turn off alarm clock interrupt
 *  @since      v1.0
 */
void RTC_Close_Alarm(void)
{
  RTC_IER &= ~RTC_IER_TAIE_MASK;//Do not interrupt the alarm clock
}

/*
 *  @brief      RTC Interrupt Service Function
 *  @since      v1.0
 *  @warning    This function needs to be completed by the user according to their own needs, and only a template is provided here
 *  Sample usage:       Set_Vector_Handler(RTC_VECTORn , rtc_test_handler);    //Add the rtc_test_ handler function to the interrupt vector table without manually calling it
 */
void RTC_Test_Handler(void)
{
  if(RTC_SR & RTC_SR_TIF_MASK)//Invalid settings
  {
    RTC_SR = 0;
    RTC_TSR = 0;
    
    //Put Your Code...
    
  }
  
  if(RTC_SR & RTC_SR_TOF_MASK)//Count overflow
  {
    RTC_SR = 0;
    RTC_TSR = 0;
    
    //Put Your Code...
    
  }
  
  if(RTC_SR & RTC_SR_TAF_MASK)//Alarm
  {
    RTC_TAR = 0;
    
    //Put Your Code...
  }
  
}