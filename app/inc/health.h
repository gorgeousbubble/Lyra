/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       health.h
 * @brief      MAX30102 heart rate and SpO2 measurement interface
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __HEALTH_H__
#define __HEALTH_H__

#include "common.h"

// Must match BUFFER_SIZE in max30102_algo.h (FS=100Hz, 5 seconds of data)
#define HEALTH_BUFFER_SIZE 500

// MAX30102 GPIO port macros
// INT  : interrupt output from MAX30102 (input to MCU)
// IRD  : IR LED drive enable (output from MCU)
// RD   : Red LED PWM drive via FTM (output from MCU)
#ifndef MAX30102_PORT_INIT_INT
#define MAX30102_PORT_INIT_INT GPIO_Init(PTA10, GPI, 0)
#endif
#ifndef MAX30102_PORT_INIT_IRD
#define MAX30102_PORT_INIT_IRD GPIO_Init(PTA7, GPO, 0)
#endif
#ifndef MAX30102_PORT_INIT_RD
#define MAX30102_PORT_INIT_RD  FTM_PWM_Init(FTM_FTM0, FTM_CH4, 50, 0);
#endif
#ifndef MAX30102_SET_IRD_H
#define MAX30102_SET_IRD_H GPIO_SET(PTA7, 1)
#endif
#ifndef MAX30102_SET_IRD_L
#define MAX30102_SET_IRD_L GPIO_SET(PTA7, 0)
#endif
#ifndef MAX30102_GET_INT_IN
#define MAX30102_GET_INT_IN GPIO_GET(PTA10)
#endif

// Power-of-two ring buffer for sensor samples (no dynamic allocation)
typedef struct
{
    uint32 data[HEALTH_BUFFER_SIZE];
    int head;  // Next write position
    int count; // Number of valid samples (0 ~ HEALTH_BUFFER_SIZE)
} RingBuffer;

extern int32  SPO2;       // Latest valid SpO2 result (%)
extern int32  Heart_Rate; // Latest valid heart rate (bpm)
extern uint32 RD_Duty;    // Red LED PWM duty cycle (0~10000)

// Initialize MAX30102 sensor and all health-related GPIO ports
extern void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Init(void);

// Collect one RED/IR sample — call from PIT1 ISR at 100Hz
extern void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Collect(uint32 red, uint32 ir);

// Run SpO2/HR algorithm — call from main loop, NOT from ISR
extern void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Calculate(void);

#endif
