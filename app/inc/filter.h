/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       filter.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __FILTER_H__
#define __FILTER_H__

#include "common.h"

// Kalman filter structure
typedef struct
{
    float dt;      // Sample time
    float angle_f; // Filtered angle
    float angle_m; // Measuring angles
    float wb_m;    // Angular velocity measurement
    float wb_f;    // After angular velocity filtering
    float q_bias;  // Angular velocity offset
    float P[2][2]; // Covariance matrix
    float Q_angle; // Process noise covariance (angle)
    float Q_gyro;  // Process noise covariance (gyroscope)
    float R_angle; // Measurement noise covariance
} KalmanFilter;

// Fusion filter structure
typedef struct
{
    float acc_m;  // Measuring accelerometer
    float acc_f;  // Normalized accelerometer
    float gyro_m; // Measuring gyroscope
    float gyro_f; // Normalized gyroscope
    float angle_f;  // After angle filtering
    float alpha;  // Fusion filter coefficient
    float dt;     // Sample time
} FusionFilter;

/*
**variate declaration
*/

/*
**function declaration
*/
extern void Kalman_Init(KalmanFilter *kf, float p[2][2], float dt, float q_angle, float q_gyro, float r_angle);
extern float Kalman_Filter(KalmanFilter *kf, float angle_m, float gyro_m);
extern void Fusion_Init(FusionFilter *ff, float alpha, float dt);
extern void Fusion_Filter(FusionFilter *ff, float acc_m, float gyro_m);

#endif