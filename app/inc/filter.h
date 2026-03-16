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
// Attitude structure
typedef struct
{
    float roll;  // X-axis rotation
    float pitch; // Y-axis rotation
    float yaw;   // Z-axis rotation
} Attitude_t;

// Quaternion structure
typedef struct
{
    float q0, q1, q2, q3; // Quaternion [w, x, y, z]
} Quaternion_t;

// Fusion filter structure
typedef struct
{
    Attitude_t angle;
    Quaternion_t quat;
    float kp;
    float ki;
    float dt;
    float exInt;
    float eyInt;
    float ezInt;
    uint8_t initialized;
} FusionFilter;

/*
**variate declaration
*/

/*
**function declaration
*/
extern void Kalman_Init(KalmanFilter *kf, float p[2][2], float dt, float q_angle, float q_gyro, float r_angle);
extern float Kalman_Filter(KalmanFilter *kf, float angle_m, float gyro_m);
extern void Fusion_Sensor_Covert(float* acc_norm, float* gyro_norm, const int acc_raw[3], const int gyro_raw[3], int acc_range, int gyro_range);

#endif