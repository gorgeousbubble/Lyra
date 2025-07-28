/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       health.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __HEALTH_H__
#define __HEALTH_H__

#include "common.h"

#define MAX_BRIGHTNESS 255

//Health Sensor Port Macro
//INT port initialization
#ifndef MAX30102_PORT_INIT_INT
#define MAX30102_PORT_INIT_INT  GPIO_Init(PTA10,GPI,0)
#endif
//IRD port initialization
#ifndef MAX30102_PORT_INIT_IRD
#define MAX30102_PORT_INIT_IRD  GPIO_Init(PTA7,GPO,0)
#endif
//RD port initialization (FTM)
#ifndef MAX30102_PORT_INIT_RD
#define MAX30102_PORT_INIT_RD   FTM_PWM_Init(FTM_FTM0,FTM_CH4,50,0);
#endif
//IRD port high level
#ifndef MAX30102_SET_IRD_H
#define MAX30102_SET_IRD_H      GPIO_SET(PTA7,1)
#endif
//IRD port low level
#ifndef MAX30102_SET_IRD_L
#define MAX30102_SET_IRD_L      GPIO_SET(PTA7,0)
#endif
//INT port input signal
#ifndef MAX30102_GET_INT_IN
#define MAX30102_GET_INT_IN     GPIO_GET(PTA10)
#endif

/*
**variate declaration
*/
typedef struct Node {
    uint32 data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int size;
    int capacity;
} LinkedList;

extern int32 SPO2;             //SPO2 value
extern int32 Heart_Rate;       //Heart Rate value
extern uint32 RD_Duty;

/*
**function declaration
*/
extern void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Init(void);
extern void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Clean(void);
extern void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Calculate(void);

#endif