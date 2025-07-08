/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       main.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "adc.h"
#include "it.h"
#include "maps_dock_led.h"
#include "misc.h"
#include "mpu6050.h"
#include "pit.h"
#include "port.h"

/*
**PIT counter
*/
int PIT0_Count=0;//PIT0 count
char PIT0_Flag=0;//PIT0 flag

int PIT1_Count=0;//PIT1 count
char PIT1_Flag=0;//PIT1 flag 

/*
**ADC convert
*/
uint16 ADC_Convert_Result[2]={0};

/*
**MPU6050 sensor
*/
MPU6050_Sensor MPU6050 = {
  .Acc = {0, 0, 0},
  .Gyro = {0, 0, 0}
};

/*
 *  @brief      PORTC_PTC19_IRQHandler     PTC19 External Interrupt Service Function
 *  @since      v1.0
 */
void PORTC_PTC19_IRQHandler(void)
{
  if(IRQ_MARK(C,19))
  {
    IRQ_CLEAR(C,19);
    disable_irq(PORTC_IRQn);
    
    //Put Your Code...
    
    IRQ_CLEAR(C,19);
    enable_irq(PORTC_IRQn);
  }
}

/*
 *  @brief      PORTD_PTD15_IRQHandler     PTD15 External Interrupt Service Function
 *  @since      v1.0
 */
void PORTD_PTD15_IRQHandler(void)
{
  if(IRQ_MARK(D,15))
  {
    IRQ_CLEAR(D,15);
    disable_irq(PORTD_IRQn);
    
    //Put Your Code...
    
    IRQ_CLEAR(D,15);
    enable_irq(PORTD_IRQn);
  }
}

/*
 *  @brief      PIT0_IRQHandler     PIT0 timed interrupt service function
 *  @since      v1.0
 */
void PIT0_IRQHandler(void)
{
  PIT_Flag_Clear(PIT0);
  disable_irq(PIT0_IRQn);
  
  //Put Your Code...
  
  PIT0_Count++;
  
  if(PIT0_Count > 100)
  {
    PIT0_Count = 100;
  }
  
  if(PIT0_Count == 100)
  {
    PIT0_Count = 0;
    
    PIT0_Flag++;
    
    if(PIT0_Flag > 3)
    {
      PIT0_Flag = 0;
    }
    
    MAPS_Dock_LED_Turn();//LED turnover
    ADC_Convert_Result[0] = ADC_Once(ADC0_DP0,ADC_12Bit);//ADC convert
    ADC_Convert_Result[1] = ADC_Once(ADC0_DM0,ADC_12Bit);//ADC convert
  }
  
  PIT_Flag_Clear(PIT0);
  enable_irq(PIT0_IRQn);
}

/*
 *  @brief      PIT1_IRQHandler     PIT1 timed interrupt service function
 *  @since      v1.0
 */
void PIT1_IRQHandler(void)
{
  PIT_Flag_Clear(PIT1);
  disable_irq(PIT1_IRQn);
  
  //Put Your Code...
  
  PIT1_Count++;
  
  if(PIT1_Count > 100)
  {
    PIT1_Count = 100;
  }
  
  if(PIT1_Count == 100)
  {
    PIT1_Count = 0;
    
    PIT1_Flag++;
    
    if(PIT1_Flag > 3)
    {
      PIT1_Flag = 0;
    }
    
    // MPU6050 sensor data read
    MPU6050.Acc.X = MPU_Get_Acc_X();
    MPU6050.Acc.Y = MPU_Get_Acc_Y();
    MPU6050.Acc.Z = MPU_Get_Acc_Z();
    MPU6050.Gyro.X = MPU_Get_Gyro_X();
    MPU6050.Gyro.Y = MPU_Get_Gyro_Y();
    MPU6050.Gyro.Z = MPU_Get_Gyro_Z();
  }
  
  PIT_Flag_Clear(PIT1);
  enable_irq(PIT1_IRQn);
}

