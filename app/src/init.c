/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       init.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "adc.h"
#include "i2c.h"
#include "init.h"
#include "it.h"
#include "maps_dock_key.h"
#include "maps_dock_lcd.h"
#include "maps_dock_lcm.h"
#include "maps_dock_led.h"
#include "maps_dock_rocker.h"
#include "maps_dock_w25q80.h"
#include "misc.h"
#include "mpu6050.h"
#include "oled.h"
#include "oled_i2c.h"
#include "pit.h"
#include "system_init.h"
#include "uart.h"

/*
 *  @brief      Interrupt_Init     Interrupt initialization
 *  @since      v1.0
 */
void Interrupt_Init(void)
{
  /*PORT_Init_Exit(PTC19,IRQ_EITHER);//PTC19 external interrupt, jump edge trigger
  set_irq_priority(PORTC_IRQn,0);//Interrupt priority 0
  enable_irq(PORTC_IRQn);//Interrupts enable
  
  PORT_Init_Exit(PTD15,IRQ_EITHER);//PTD15 external interrupt, jump edge trigger
  set_irq_priority(PORTD_IRQn,1);//Interrupt priority 1
  enable_irq(PORTD_IRQn);//Interrupts enable*/
  
  PIT_Init_ms(PIT0,1);//PIT0 timed interrupt, with a timing cycle of 1ms
  set_irq_priority(PIT0_IRQn,2);//Interrupt priority 2
  Set_Vector_Handler(PIT0_VECTORn,PIT0_IRQHandler);
  enable_irq(PIT0_IRQn);//Interrupts enable
  
  PIT_Init_ms(PIT1,1);//PIT1 timed interrupt, with a timing cycle of 1ms
  set_irq_priority(PIT1_IRQn,3);//Interrupt priority 3
  Set_Vector_Handler(PIT1_VECTORn,PIT1_IRQHandler);
  enable_irq(PIT1_IRQn);//Interrupts enable
  
}

/*
 *  @brief      AllInit     Application initialization
 *  @since      v1.0
 */
void AllInit(void)
{
  DisableInterrupts;// Close interrupts
  /*
  **MAPS_Dock
  */
  MAPS_Dock_LED_Init();//LED initialization
  MAPS_Dock_LCM_Init();//LCM initialization
  MAPS_Dock_KEY_ALL_Init();//KEY independent button initialization
  MAPS_Dock_Rocker_Key_Init();//Rocker joystick button initialization
  MAPS_Dock_W25Q80_Init();//W25Q80 initialization
  
  /*
  **MAPS
  */
  MAPS_LCD_Init();//LCD initialization
  
  /*
  **MK64
  */
  ADC_Init(ADC0_DP0);//ADC0_DP0 channel initialization
  ADC_Init(ADC0_DM0);//ADC0_DM0 channel initialization
  I2C_Init(I2C_I2C0,400000);//I2C0 initialization, baud rate 400Kbps
  UART_Init(UART_UART4,115200);//UART4 initialization, baud rate 115200Bps
  //DAC_Init(DAC_DAC1);//DAC_DAC1 initialization
  I2C_GPIO_Init();//MPU6050 initialization
  Oled_I2C_Init();//Oled initialization
  Oled_I2C_Draw_Picture_128x64((uint8 const*)Oled_Picture_128x64_JBD_Logo);
  
  Interrupt_Init();//Interrupt initialization
  
  EnableInterrupts;
}