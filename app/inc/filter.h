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
    float accAngleRoll;     // Angle roll(X) calculated from accelerometer
    float accAnglePitch;    // Angle pitch(Y) calculated from accelerometer
    float accAngleYaw;      // Angle yaw(Z) calculated from accelerometer
    float gyroRateRoll;    // Angular velocity roll(X) from gyroscope
    float gyroRatePitch;   // Angular velocity pitch(Y) from gyroscope
    float gyroRateYaw;     // Angular velocity yaw(Z) from gyroscope
    float fusedAngleRoll;   // Fused angle roll(X)
    float fusedAnglePitch;  // Fused angle pitch(Y)
    float fusedAngleYaw;    // Fused angle yaw(Z)
    float alpha;        // Fusion coefficient
    float dt;           // Sample time
} FusionFilter;

/*
**variate declaration
*/

/*
**function declaration
*/
extern void Kalman_Init(KalmanFilter *kf, float p[2][2], float dt, float q_angle, float q_gyro, float r_angle);
extern float Kalman_Filter(KalmanFilter *kf, float angle_m, float gyro_m);
extern void Fusion_Filter(FusionFilter *ff, float angle_roll, float angle_pitch, float angle_yaw, float gyro_roll, float gyro_pitch, float gyro_yaw);

#endif