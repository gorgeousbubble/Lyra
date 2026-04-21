/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       filter.h
 * @brief      MPU6050 Kalman filter and complementary filter structures
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __FILTER_H__
#define __FILTER_H__

#include "common.h"

// Kalman filter state (2-state: angle + gyro bias)
typedef struct
{
    float dt;      // Sample period (s)
    float angle_f; // Filtered angle output (degrees)
    float angle_m; // Reserved
    float wb_m;    // Reserved
    float wb_f;    // Reserved
    float q_bias;  // Estimated gyro bias (degrees/s)
    float P[2][2]; // Error covariance matrix
    float Q_angle; // Process noise: angle state
    float Q_gyro;  // Process noise: gyro bias state
    float R_angle; // Measurement noise
} KalmanFilter;

// Single-axis complementary filter state
typedef struct
{
    float angle;     // Filtered angle (degrees)
    float acc_angle; // Raw accelerometer angle before fusion (degrees)
    float alpha;     // Gyro weight [0,1]; higher = smoother but slower response
    float dt;        // Sample period (s)
} ComplementaryFilter;

// Yaw axis: gyro integration only (no absolute reference)
typedef struct
{
    float angle; // Integrated yaw angle (degrees, drifts over time)
    float dt;    // Sample period (s)
} YawIntegrator;

// Gyro zero-bias calibration state
typedef struct
{
    float bias_x;     // Calibrated X-axis bias (degrees/s)
    float bias_y;     // Calibrated Y-axis bias (degrees/s)
    float bias_z;     // Calibrated Z-axis bias (degrees/s)
    float sum_x;      // Accumulator (cleared by Fusion_Init)
    float sum_y;
    float sum_z;
    int   count;      // Samples collected so far
    uint8 calibrated; // 1 = calibration complete
} GyroBias;

// Filter mode selection: 0 = complementary fusion filter, 1 = Kalman filter
#define FILTER_MODE_FUSION  0
#define FILTER_MODE_KALMAN  1
#define FILTER_MODE         FILTER_MODE_FUSION

// Full fusion filter: pitch + roll (complementary or Kalman) + yaw (gyro integration)
typedef struct
{
    ComplementaryFilter pitch;
    ComplementaryFilter roll;
    YawIntegrator       yaw;
    GyroBias            gyro_bias;
    KalmanFilter        kf_pitch; // Kalman filter for pitch
    KalmanFilter        kf_roll;  // Kalman filter for roll
} FusionFilter;

extern void  Kalman_Init(KalmanFilter *kf, float p[2][2], float dt, float q_angle, float q_gyro, float r_angle);
extern float Kalman_Filter(KalmanFilter *kf, float angle_m, float gyro_m);
extern void  Fusion_Init(FusionFilter *ff, float alpha, float dt);
extern void  Fusion_Calibrate(FusionFilter *ff, int16 gx, int16 gy, int16 gz);
extern void  Fusion_Filter(FusionFilter *ff, int16 ax, int16 ay, int16 az, int16 gx, int16 gy, int16 gz);

#endif
