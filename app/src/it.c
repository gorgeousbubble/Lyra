/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       it.c
 * @brief      Interrupt service routines and shared global variables
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "adc.h"
#include "ftm.h"
#include "health.h"
#include "it.h"
#include "filter.h"
#include "max30102.h"
#include "maps_dock_led.h"
#include "misc.h"
#include "mpu6050.h"
#include "pit.h"
#include "port.h"
#include "rtc.h"
#include <time.h>

/*
**PIT0: 1ms tick counter, resets every 100ms
*/
volatile int PIT0_Count = 0;
char PIT0_Flag = 0; // 100ms cycle flag (0~3 rolling)

/*
**PIT1: 1ms tick counter, resets every 100ms
**  % 10 == 0 : MPU6050 read + filter (10ms, 100Hz)
**  % 10 == 5 : MAX30102 read + collect (10ms, 100Hz, 5ms offset)
**  >= 100    : 100ms periodic tasks
*/
volatile int PIT1_Count = 0;
char PIT1_Flag = 0; // 100ms cycle flag (0~3 rolling)

/*
**ADC convert
*/
uint16 ADC_Convert_Result[2] = {0};

/*
**MPU6050 sensor
*/
volatile MPU6050_Sensor MPU6050 = {
    .Acc = {0, 0, 0},
    .Gyro = {0, 0, 0}};

MPU6050_Sensor_Norm MPU6050_Norm = {
    .Acc = {0, 0, 0},
    .Gyro = {0, 0, 0}};

/*
**Fusion Filter (Complementary Filter for Pitch & Roll)
*/
FusionFilter FF = {
    .pitch = {.angle = 0.0f, .acc_angle = 0.0f, .alpha = 0.93f, .dt = 0.01f},
    .roll  = {.angle = 0.0f, .acc_angle = 0.0f, .alpha = 0.93f, .dt = 0.01f},
    .yaw   = {.angle = 0.0f, .dt = 0.01f},
    .gyro_bias = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0}};

Angle MPU6050_Angle = {
    .Angle_X = 0.0f, // Angle X
    .Angle_Y = 0.0f, // Angle Y
    .Angle_Z = 0.0f  // Angle Z
};

/*
**RTC counter
*/
volatile uint32 RTC_Count = 0;
volatile RTC_Time RTC_Time_Now = {
    .Year = 0,
    .Month = 0,
    .Day = 0,
    .Hour = 0,
    .Minute = 0,
    .Second = 0};

/*
**MAX30102 counter
*/
volatile uint32 MAX30102_RED = 0;
volatile uint32 MAX30102_IR = 0;

/*
**Stop Watch counter
*/
volatile Stop_Watch_Time Stop_Watch_Now = {
    .Minute = 0,
    .Second = 0,
    .Centisecond = 0};

volatile int Stop_Watch_Count = 0; // Stop Watch count
volatile int Stop_Watch_State = 0; // Stop Watch state (0: stop, 1: start)
volatile uint8 UART_Send_Flag = 0; // UART send flag (set in PIT1, cleared in main)

/*
 *  @brief      PORTC_PTC19_IRQHandler     PTC19 External Interrupt Service Function
 *  @since      v1.0
 */
void PORTC_PTC19_IRQHandler(void)
{
  if (IRQ_MARK(C, 19))
  {
    IRQ_CLEAR(C, 19);
    disable_irq(PORTC_IRQn);
    IRQ_CLEAR(C, 19);
    enable_irq(PORTC_IRQn);
  }
}

/*
 *  @brief      PORTD_PTD15_IRQHandler     PTD15 External Interrupt Service Function
 *  @since      v1.0
 */
void PORTD_PTD15_IRQHandler(void)
{
  if (IRQ_MARK(D, 15))
  {
    IRQ_CLEAR(D, 15);
    disable_irq(PORTD_IRQn);
    IRQ_CLEAR(D, 15);
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

  // Stopwatch centisecond tick (called every 1ms, increments every 10ms)
  if (Stop_Watch_State)
  {
    Stop_Watch_Count++;
    if (Stop_Watch_Count >= 10)
    {
      Stop_Watch_Count = 0;
      Stop_Watch_Now.Centisecond++;
      if (Stop_Watch_Now.Centisecond >= 100)
      {
        Stop_Watch_Now.Centisecond = 0;
        Stop_Watch_Now.Second++;
        if (Stop_Watch_Now.Second >= 60)
        {
          Stop_Watch_Now.Second = 0;
          Stop_Watch_Now.Minute++;
          if (Stop_Watch_Now.Minute >= 60)
          {
            Stop_Watch_Now.Minute = 0;
          }
        }
      }
    }
  }

  PIT0_Count++;

  // 100ms periodic tasks: LED blink, ADC, RTC update
  if (PIT0_Count >= 100)
  {
    PIT0_Count = 0;

    PIT0_Flag++;

    if (PIT0_Flag > 3)
    {
      PIT0_Flag = 0;
    }

    MAPS_Dock_LED_Turn();                                  // LED turnover
    ADC_Convert_Result[0] = ADC_Once(ADC0_DP0, ADC_12Bit); // ADC convert
    ADC_Convert_Result[1] = ADC_Once(ADC0_DM0, ADC_12Bit); // ADC convert
    RTC_Count = RTC_Get_Time();

    // Use gmtime instead of localtime: no timezone conversion,
    // safe in single-threaded embedded ISR context
    time_t rawtime = (time_t)RTC_Count;
    struct tm *timeinfo = gmtime(&rawtime);
    RTC_Time_Now.Year   = timeinfo->tm_year + 1900;
    RTC_Time_Now.Month  = timeinfo->tm_mon + 1;
    RTC_Time_Now.Day    = timeinfo->tm_mday;
    RTC_Time_Now.Hour   = timeinfo->tm_hour;
    RTC_Time_Now.Minute = timeinfo->tm_min;
    RTC_Time_Now.Second = timeinfo->tm_sec;
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

  PIT1_Count++;

  // Sample period of 10ms: MPU6050 read + filter
  if (PIT1_Count % 10 == 0)
  {
    // MPU6050 sensor data read
    MPU6050.Acc.X = MPU_Get_Acc_X();
    MPU6050.Acc.Y = MPU_Get_Acc_Y();
    MPU6050.Acc.Z = MPU_Get_Acc_Z();
    MPU6050.Gyro.X = MPU_Get_Gyro_X();
    MPU6050.Gyro.Y = MPU_Get_Gyro_Y();
    MPU6050.Gyro.Z = MPU_Get_Gyro_Z();

    // Gyro calibration (first 2 seconds after power-on, sensor must be stationary)
    if (!FF.gyro_bias.calibrated)
    {
      int16 gx = MPU6050.Gyro.X;
      int16 gy = MPU6050.Gyro.Y;
      int16 gz = MPU6050.Gyro.Z;
      Fusion_Calibrate(&FF, gx, gy, gz);
    }
    else
    {
      int16 ax = MPU6050.Acc.X;
      int16 ay = MPU6050.Acc.Y;
      int16 az = MPU6050.Acc.Z;
      int16 gx = MPU6050.Gyro.X;
      int16 gy = MPU6050.Gyro.Y;
      int16 gz = MPU6050.Gyro.Z;
      Fusion_Filter(&FF, ax, ay, az, gx, gy, gz);
    }
    UART_Send_Flag = 1; // Signal main loop to send data
  }

  // MAX30102 read on odd 10ms ticks (5ms offset from MPU6050)
  if (PIT1_Count % 10 == 5)
  {
    uint32 red = 0, ir = 0;
    MAX30102_ReadFIFO(&red, &ir);
    MAX30102_RED = red;
    MAX30102_IR  = ir;
    Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Collect(red, ir);
  }

  // Sample period of 100ms
  if (PIT1_Count >= 100)
  {
    PIT1_Count = 0;

    PIT1_Flag++;
    if (PIT1_Flag > 3)
    {
      PIT1_Flag = 0;
    }
  }

  PIT_Flag_Clear(PIT1);
  enable_irq(PIT1_IRQn);
}

/*
 *  @brief      RTC_IRQHandler     RTC timed interrupt service function
 *  @since      v1.0
 */
void RTC_IRQHandler(void)
{
  disable_irq(RTC_IRQn);
  if (RTC_SR & RTC_SR_TIF_MASK) // Time invalid
  {
    RTC_SR  = 0;
    RTC_TSR = 0;
  }
  if (RTC_SR & RTC_SR_TOF_MASK) // Counter overflow
  {
    RTC_SR  = 0;
    RTC_TSR = 0;
  }
  if (RTC_SR & RTC_SR_TAF_MASK) // Alarm triggered
  {
    RTC_TAR = 0;
  }
  enable_irq(RTC_IRQn);
}
