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
#include "maps_dock_led.h"
#include "misc.h"
#include "pit.h"
#include "port.h"
#include "rtc.h"

// --- ISR timing constants ---
#define PIT_PERIOD_MS           1     // PIT tick period (ms)
#define SENSOR_SAMPLE_PERIOD   10     // MPU6050/MAX30102 sample interval (ms)
#define SENSOR_PHASE_OFFSET     5     // MAX30102 offset from MPU6050 (ms)
#define PERIODIC_TASK_PERIOD  100     // LED/ADC/RTC update interval (ms)
#define STOPWATCH_TICK_MS      10     // Stopwatch centisecond increment interval (ms)

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
**MPU6050 sensor (no longer volatile: read/write both in main loop)
*/
MPU6050_Sensor MPU6050 = {
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
volatile uint8 MPU6050_Read_Flag = 0;  // Set by PIT1 every 10ms, cleared by main
volatile uint8 MAX30102_Read_Flag = 0; // Set by PIT1 every 10ms (5ms offset), cleared by main
volatile uint8 RTC_Update_Flag = 0;    // Set by PIT0 every 100ms, cleared by main

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

  // Stopwatch centisecond tick (called every 1ms, increments every 10ms)
  if (Stop_Watch_State)
  {
    Stop_Watch_Count++;
    if (Stop_Watch_Count >= STOPWATCH_TICK_MS)
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

  // 100ms periodic tasks: LED blink, set RTC update flag
  if (PIT0_Count >= PERIODIC_TASK_PERIOD)
  {
    PIT0_Count = 0;

    PIT0_Flag++;

    if (PIT0_Flag > 3)
    {
      PIT0_Flag = 0;
    }

    MAPS_Dock_LED_Turn();                                  // LED turnover (fast GPIO, OK in ISR)
    RTC_Update_Flag = 1;                                   // Signal main loop to do ADC + RTC update
  }
}

/*
 *  @brief      PIT1_IRQHandler     PIT1 timed interrupt service function
 *  @since      v1.0
 */
void PIT1_IRQHandler(void)
{
  PIT_Flag_Clear(PIT1);

  PIT1_Count++;

  // Every 10ms: signal main loop to read MPU6050 + run filter
  if (PIT1_Count % SENSOR_SAMPLE_PERIOD == 0)
  {
    MPU6050_Read_Flag = 1;
  }

  // Every 10ms (5ms offset): signal main loop to read MAX30102
  if (PIT1_Count % SENSOR_SAMPLE_PERIOD == SENSOR_PHASE_OFFSET)
  {
    MAX30102_Read_Flag = 1;
  }

  // Sample period of 100ms
  if (PIT1_Count >= PERIODIC_TASK_PERIOD)
  {
    PIT1_Count = 0;

    PIT1_Flag++;
    if (PIT1_Flag > 3)
    {
      PIT1_Flag = 0;
    }
  }
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
