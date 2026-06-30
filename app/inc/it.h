/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       it.h
 * @brief      ISR declarations and shared global variables
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __IT_H__
#define __IT_H__

#include "common.h"
#include "filter.h"
#include "vectors.h"

// Interrupt vector table bindings
#undef VECTOR_077
#define VECTOR_077 PORTC_PTC19_IRQHandler

#undef VECTOR_078
#define VECTOR_078 PORTD_PTD15_IRQHandler

#undef VECTOR_064
#define VECTOR_064 PIT0_IRQHandler

#undef VECTOR_065
#define VECTOR_065 PIT1_IRQHandler

#undef VECTOR_062
#define VECTOR_062 RTC_IRQHandler

// MPU6050 raw sensor data (written by PIT1 ISR, read by main loop)
typedef struct { int16 X; int16 Y; int16 Z; } ACC;
typedef struct { int16 X; int16 Y; int16 Z; } GYRO;
typedef struct { ACC Acc; GYRO Gyro; } MPU6050_Sensor;

// MPU6050 normalized data (reserved, not currently used)
typedef struct { float X; float Y; float Z; } ACC_Norm;
typedef struct { float X; float Y; float Z; } GYRO_Norm;
typedef struct { ACC_Norm Acc; GYRO_Norm Gyro; } MPU6050_Sensor_Norm;

// Euler angles computed from filter output (reserved)
typedef struct { float Angle_X; float Angle_Y; float Angle_Z; } Angle;

// RTC calendar time (written by PIT0 ISR every 100ms)
typedef struct
{
  int Year;
  int Month;
  int Day;
  int Hour;
  int Minute;
  int Second;
} RTC_Time;

// Stopwatch elapsed time (written by PIT0 ISR every 1ms)
typedef struct
{
  int Minute;
  int Second;
  int Centisecond;
} Stop_Watch_Time;

// --- Shared variables (volatile: written in ISR, read in main loop) ---
extern volatile int            PIT0_Count;          // 1ms tick, resets every 100ms
extern volatile int            PIT1_Count;          // 1ms tick, resets every 100ms
extern          uint16         ADC_Convert_Result[2];
extern          MPU6050_Sensor MPU6050;             // Written in main loop (no longer in ISR)
extern          MPU6050_Sensor_Norm MPU6050_Norm;   // Reserved
extern          Angle          MPU6050_Angle;        // Reserved
extern          FusionFilter   FF;                   // Complementary filter state (written in main loop)
extern volatile uint32         RTC_Count;
extern volatile RTC_Time       RTC_Time_Now;
extern volatile uint32         MAX30102_RED;
extern volatile uint32         MAX30102_IR;
extern volatile Stop_Watch_Time Stop_Watch_Now;
extern volatile int            Stop_Watch_Count;
extern volatile int            Stop_Watch_State;    // 0=stopped, 1=running
extern volatile uint8          UART_Send_Flag;       // Set by PIT1, cleared by main
extern volatile uint8          MPU6050_Read_Flag;    // Set by PIT1 every 10ms, cleared by main
extern volatile uint8          MAX30102_Read_Flag;   // Set by PIT1 every 10ms (5ms offset), cleared by main
extern volatile uint8          RTC_Update_Flag;      // Set by PIT0 every 100ms, cleared by main
/* Monotonically increasing ms counter — incremented in PIT1_IRQHandler every 1 ms.
 * uint32 wraps after ~49.7 days; callers handle this with saturating subtraction. */
extern volatile uint32         Key_Ms_Tick;          // Global 1ms tick for non-blocking debounce

// ISR function declarations
extern void PORTC_PTC19_IRQHandler(void);
extern void PORTD_PTD15_IRQHandler(void);
extern void PIT0_IRQHandler(void);
extern void PIT1_IRQHandler(void);
extern void RTC_IRQHandler(void);

#endif
