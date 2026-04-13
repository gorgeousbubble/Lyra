/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       it.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __IT_H__
#define __IT_H__

#include "common.h"
#include "filter.h"
#include "vectors.h"

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

/*
**variate definition
*/
typedef struct
{
  int16 X;
  int16 Y;
  int16 Z;
} ACC;

typedef struct
{
  int16 X;
  int16 Y;
  int16 Z;
} GYRO;

typedef struct
{
  ACC Acc;
  GYRO Gyro;
} MPU6050_Sensor;

typedef struct
{
  float X;
  float Y;
  float Z;
} ACC_Norm;

typedef struct
{
  float X;
  float Y;
  float Z;
} GYRO_Norm;

typedef struct
{
  ACC_Norm Acc;
  GYRO_Norm Gyro;
} MPU6050_Sensor_Norm;

typedef struct
{
  float Angle_X; // Angle X
  float Angle_Y; // Angle Y
  float Angle_Z; // Angle Z
} Angle;

typedef struct
{
  int Year;
  int Month;
  int Day;
  int Hour;
  int Minute;
  int Second;
} RTC_Time;

// stop watch time
typedef struct
{
  int Minute;
  int Second;
  int Centisecond;
} Stop_Watch_Time;

/*
**variate declaration
*/
extern volatile int PIT0_Count;                   // PIT0 counter
extern volatile int PIT1_Count;                   // PIT1 counter
extern uint16 ADC_Convert_Result[2];              // ADC sample
extern volatile MPU6050_Sensor MPU6050;           // MPU6050
extern MPU6050_Sensor_Norm MPU6050_Norm;          // MPU6050 normalized data
extern Angle MPU6050_Angle;                       // MPU6050 angle
extern FusionFilter FF;                           // Fusion Filter (Pitch & Roll)
extern volatile uint32 RTC_Count;
extern volatile RTC_Time RTC_Time_Now;
extern volatile uint32 MAX30102_RED;
extern volatile uint32 MAX30102_IR;
extern volatile Stop_Watch_Time Stop_Watch_Now;
extern volatile int Stop_Watch_Count;
extern volatile int Stop_Watch_State;
extern volatile uint8 UART_Send_Flag;

/*
**function declaration
*/
extern void PORTC_PTC19_IRQHandler(void);
extern void PORTD_PTD15_IRQHandler(void);
extern void PIT0_IRQHandler(void);
extern void PIT1_IRQHandler(void);
extern void RTC_IRQHandler(void);

#endif