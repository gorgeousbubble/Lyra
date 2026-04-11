/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       health.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "ftm.h"
#include "gpio.h"
#include "health.h"
#include "max30102.h"
#include "max30102_algo.h"

// Static ring buffers (no malloc, no fragmentation)
static RingBuffer IR_Buff = {0};
static RingBuffer RED_Buff = {0};

int32 SPO2 = 0;
int32 Heart_Rate = 0;
uint32 RD_Duty = 0;

// Add a sample to ring buffer
static void RingBuffer_Add(RingBuffer *rb, uint32 data)
{
    rb->data[rb->head] = data;
    rb->head = (rb->head + 1) % HEALTH_BUFFER_SIZE;
    if (rb->count < HEALTH_BUFFER_SIZE)
        rb->count++;
}

// Get sample by index (0 = oldest)
static uint32 RingBuffer_Get(RingBuffer *rb, int index)
{
    int pos = (rb->head - rb->count + index + HEALTH_BUFFER_SIZE) % HEALTH_BUFFER_SIZE;
    return rb->data[pos];
}

/*
**heart rate and oxygen saturation sensor initialization
*/
void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Init(void)
{
    MAX30102_PORT_INIT_RD;
    MAX30102_PORT_INIT_IRD;
    MAX30102_PORT_INIT_INT;
    MAX30102_Init();
}

/*
**Collect one sample from MAX30102 (call from ISR at 100Hz)
*/
void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Collect(uint32 red, uint32 ir)
{
    RingBuffer_Add(&IR_Buff, ir);
    RingBuffer_Add(&RED_Buff, red);
}

/*
**Run heart rate and SpO2 algorithm (call from main loop, not ISR)
*/
void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Calculate(void)
{
    int32 spo2 = 0;
    int8 spo2_valid = 0;
    int32 heart_rate = 0;
    int8 heart_rate_valid = 0;

    // Need full buffer for algorithm
    if (IR_Buff.count < HEALTH_BUFFER_SIZE)
        return;

    // Copy ring buffer to contiguous arrays for algorithm
    static uint32 ir_array[HEALTH_BUFFER_SIZE];
    static uint32 red_array[HEALTH_BUFFER_SIZE];
    for (int i = 0; i < HEALTH_BUFFER_SIZE; i++)
    {
        ir_array[i] = RingBuffer_Get(&IR_Buff, i);
        red_array[i] = RingBuffer_Get(&RED_Buff, i);
    }

    // Calculate heart rate and SpO2
    maxim_heart_rate_and_oxygen_saturation(
        ir_array,
        HEALTH_BUFFER_SIZE,
        red_array,
        &spo2,
        &spo2_valid,
        &heart_rate,
        &heart_rate_valid);

    if (spo2_valid)
        SPO2 = spo2;
    if (heart_rate_valid)
        Heart_Rate = heart_rate;
}
