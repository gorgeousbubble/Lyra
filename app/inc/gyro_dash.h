/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       gyro_dash.h
 * @brief      Gyroscope dashboard: real-time waveform and bar chart of X/Y/Z angular velocity
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __GYRO_DASH_H__
#define __GYRO_DASH_H__

#include "common.h"

/*
 * Ring-buffer depth = number of historical samples shown in waveform.
 * At 100Hz (10ms per sample) this covers 800ms of history.
 */
#define GYRO_DASH_BUF_SIZE  80

/* Display modes */
#define GYRO_DASH_MODE_WAVE  0   // Scrolling waveform (default)
#define GYRO_DASH_MODE_BAR   1   // Bar chart

/*
 * GyroDash state
 */
typedef struct
{
    int16  gx_buf[GYRO_DASH_BUF_SIZE]; // Circular buffer for Gyro X
    int16  gy_buf[GYRO_DASH_BUF_SIZE]; // Circular buffer for Gyro Y
    int16  gz_buf[GYRO_DASH_BUF_SIZE]; // Circular buffer for Gyro Z
    int    head;                        // Next write index
    int    count;                       // Samples filled (0..GYRO_DASH_BUF_SIZE)
    uint8  mode;                        // GYRO_DASH_MODE_WAVE or _BAR
} GyroDashState;

extern GyroDashState GyroDash;

/*
 * Push one sample (call from main loop every 10ms after MPU6050 read).
 * gx, gy, gz: raw gyroscope values in LSB (250 dps range -> 131 LSB/°/s).
 */
extern void GyroDash_Update(int16 gx, int16 gy, int16 gz);

/* Toggle display mode (KEY0) */
extern void GyroDash_Toggle_Mode(void);

/* Render the dashboard to OLED */
extern void Render_GyroDash(int16 gx, int16 gy, int16 gz);

#endif
