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
#include "kalman_filter.h"
#include "vectors.h"

#undef  VECTOR_077
#define VECTOR_077      PORTC_PTC19_IRQHandler

#undef  VECTOR_078
#define VECTOR_078      PORTD_PTD15_IRQHandler

#undef  VECTOR_064
#define VECTOR_064      PIT0_IRQHandler

#undef  VECTOR_065
#define VECTOR_065      PIT1_IRQHandler

#undef  VECTOR_062
#define VECTOR_062      RTC_IRQHandler

/*
**variate definition
*/
typedef struct {
  int X;
  int Y;
  int Z;
} ACC;

typedef struct {
  int X;
  int Y;
  int Z;
} GYRO;

typedef struct {
  ACC Acc;
  GYRO Gyro;
} MPU6050_Sensor;

typedef struct {
  float Angle_X; //Angle X
  float Angle_Y; //Angle Y
  float Angle_Z; //Angle Z
} Angle;

typedef struct {
  int Year;
  int Month;
  int Day;
  int Hour;
  int Minute;
  int Second;
} RTC_Time;

/*
**variate declaration
*/
extern int PIT0_Count;//PIT0 counter
extern int PIT1_Count;//PIT1 counter
extern uint16 ADC_Convert_Result[2];//ADC sample
extern MPU6050_Sensor MPU6050;//MPU6050
extern Angle MPU6050_Angle;//MPU6050 angle
extern KalmanFilter KF_X;//Kalman Filter X
extern KalmanFilter KF_Y;//Kalman Filter Y
extern KalmanFilter KF_Z;//Kalman Filter Z
extern uint32 RTC_Count;
extern RTC_Time RTC_Time_Now;

/*
**function declaration
*/
extern void PORTC_PTC19_IRQHandler(void);
extern void PORTD_PTD15_IRQHandler(void);
extern void PIT0_IRQHandler(void);
extern void PIT1_IRQHandler(void);
extern void RTC_IRQHandler(void);



#endif