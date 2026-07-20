/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       health.c
 * @brief      MAX30102 heart rate and SpO2 measurement
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "ftm.h"
#include "gpio.h"
#include "health.h"
#include "max30102.h"
#include "max30102_algo.h"

// IR and RED channel ring buffers (static, no heap allocation)
static RingBuffer IR_Buff  = {0};
static RingBuffer RED_Buff = {0};

int32  SPO2       = 0;
int32  Heart_Rate = 0;
uint32 RD_Duty    = 0;

/* Number of new samples collected since the last full algorithm run.
 * The SpO2/HR algorithm processes a 5-second (HEALTH_BUFFER_SIZE) window,
 * so there is no benefit to re-running it on almost-identical data every
 * main-loop iteration.  We only recompute once at least HEALTH_RECALC_INTERVAL
 * fresh samples have arrived (1 second of new data).  Both Collect() and
 * Calculate() run in the main loop, so no locking is needed. */
#define HEALTH_RECALC_INTERVAL  FS   /* FS = 100 samples = 1 second @ 100Hz */
static uint32 s_new_samples = 0;

// Write one sample into the ring buffer (overwrites oldest when full)
static void RingBuffer_Add(RingBuffer *rb, uint32 data)
{
    rb->data[rb->head] = data;
    rb->head = (rb->head + 1) % HEALTH_BUFFER_SIZE;
    if (rb->count < HEALTH_BUFFER_SIZE)
        rb->count++;
}

// Read sample at logical index (0 = oldest, count-1 = newest)
static uint32 RingBuffer_Get(RingBuffer *rb, int index)
{
    int pos = ((rb->head - rb->count + index) % HEALTH_BUFFER_SIZE + HEALTH_BUFFER_SIZE) % HEALTH_BUFFER_SIZE;
    return rb->data[pos];
}

// Initialize MAX30102 sensor and health-related GPIO ports (IRD LED, INT pin)
void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Init(void)
{
    MAX30102_PORT_INIT_RD;
    MAX30102_PORT_INIT_IRD;
    MAX30102_PORT_INIT_INT;
    MAX30102_Init();
}

/*
 * Collect one RED/IR sample into the ring buffers.
 * Call from the main loop at 100Hz -- driven by MAX30102_Read_Flag, which the
 * PIT1 ISR sets every 10ms -- to match the algorithm's expected sample rate.
 * (The I2C read was moved out of the ISR into the main loop; this runs there.)
 */
void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Collect(uint32 red, uint32 ir)
{
    RingBuffer_Add(&IR_Buff,  ir);
    RingBuffer_Add(&RED_Buff, red);
    if (s_new_samples < 0xFFFFFFFFUL)
        s_new_samples++;
}

/*
 * Run the heart rate and SpO2 algorithm.
 * Call from the main loop (NOT from ISR) — computation takes several milliseconds.
 * Does nothing until the ring buffer holds HEALTH_BUFFER_SIZE samples (5 seconds at 100Hz).
 * Results are written to the global SPO2 and Heart_Rate variables.
 */
void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Calculate(void)
{
    int32 spo2           = 0;
    int8  spo2_valid     = 0;
    int32 heart_rate     = 0;
    int8  heart_rate_valid = 0;

    if (IR_Buff.count < HEALTH_BUFFER_SIZE)
        return;

    /* Throttle: only recompute after enough fresh samples have arrived.
     * Running the O(N) algorithm + 1000-sample copy every main-loop
     * iteration wastes CPU on near-identical data.  One recompute per
     * second (HEALTH_RECALC_INTERVAL new samples) is more than adequate. */
    if (s_new_samples < HEALTH_RECALC_INTERVAL)
        return;
    s_new_samples = 0;

    // Copy ring buffer into contiguous arrays required by the algorithm
    static uint32 ir_array[HEALTH_BUFFER_SIZE];
    static uint32 red_array[HEALTH_BUFFER_SIZE];
    for (int i = 0; i < HEALTH_BUFFER_SIZE; i++)
    {
        ir_array[i]  = RingBuffer_Get(&IR_Buff,  i);
        red_array[i] = RingBuffer_Get(&RED_Buff, i);
    }

    maxim_heart_rate_and_oxygen_saturation(
        ir_array, HEALTH_BUFFER_SIZE, red_array,
        &spo2, &spo2_valid, &heart_rate, &heart_rate_valid);

    if (spo2_valid)      SPO2       = spo2;
    if (heart_rate_valid) Heart_Rate = heart_rate;
}
