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

// Complementary filter structure (single axis)
typedef struct
{
    float angle;     // Filtered angle (degrees)
    float acc_angle; // Accelerometer raw angle (degrees, before filtering)
    float alpha;     // Complementary coefficient (0~1, weight for gyro integration)
    float dt;        // Sample time (seconds)
} ComplementaryFilter;

// Yaw gyro integration structure (no accelerometer reference)
typedef struct
{
    float angle; // Integrated yaw angle (degrees)
    float dt;    // Sample time (seconds)
} YawIntegrator;

// Gyro zero-bias calibration structure
typedef struct
{
    float bias_x;    // X-axis gyro bias (°/s)
    float bias_y;    // Y-axis gyro bias (°/s)
    float bias_z;    // Z-axis gyro bias (°/s)
    uint8 calibrated; // Calibration done flag
} GyroBias;

// Fusion filter structure (pitch & roll complementary + yaw gyro integration)
typedef struct
{
    ComplementaryFilter pitch; // Pitch axis filter
    ComplementaryFilter roll;  // Roll axis filter
    YawIntegrator yaw;         // Yaw axis (gyro integration only)
    GyroBias gyro_bias;        // Gyro zero-bias calibration
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
extern void Fusion_Calibrate(FusionFilter *ff, int16 gx, int16 gy, int16 gz);
extern void Fusion_Filter(FusionFilter *ff, int16 ax, int16 ay, int16 az, int16 gx, int16 gy, int16 gz);

#endif