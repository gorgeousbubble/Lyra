/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       kalman_filter.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __KALMAN_FILTER_H__
#define __KALMAN_FILTER_H__

#include "common.h"

// Kalman filter structure
typedef struct {
    float dt;       // Sample time
    float angle_f;  // Filtered angle
    float angle_m;  // Measuring angles
    float wb_m;     // Angular velocity measurement
    float wb_f;     // After angular velocity filtering
    float q_bias;   // Angular velocity offset
    float P[2][2];  // Covariance matrix
    float Q_angle;  // Process noise covariance (angle)
    float Q_gyro;   // Process noise covariance (gyroscope)
    float R_angle;  // Measurement noise covariance
} KalmanFilter;

/*
**variate declaration
*/


/*
**function declaration
*/
extern void Kalman_Init(KalmanFilter* kf);
extern float Kalman_Filter(KalmanFilter* kf, float angle_m, float gyro_m);


#endif