/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       filter.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "filter.h"

// Initialize Kalman filter
void Kalman_Init(KalmanFilter *kf, float p[2][2], float dt, float q_angle, float q_gyro, float r_angle)
{
    kf->P[0][0] = p[0][0];
    kf->P[0][1] = p[0][1];
    kf->P[1][0] = p[1][0];
    kf->P[1][1] = p[1][1];
    kf->dt = dt;      // Default sampling period of 1ms
    kf->Q_angle = q_angle; // Process noise parameters (angle)
    kf->Q_gyro = q_gyro;  // Process noise parameters (gyroscope)
    kf->R_angle = r_angle;   // Measure noise parameters
    kf->q_bias = 0.0f;    // Initial value of gyroscope offset
    kf->angle_f = 0.0f;   // Initial Angle
}

// Kalman filter calculation
float Kalman_Filter(KalmanFilter *kf, float angle_m, float gyro_m)
{
    // Prediction stage
    kf->angle_f += (gyro_m - kf->q_bias) * kf->dt;
    kf->P[0][0] += kf->dt * (kf->dt * kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
    kf->P[0][1] -= kf->dt * kf->P[1][1];
    kf->P[1][0] -= kf->dt * kf->P[1][1];
    kf->P[1][1] += kf->Q_gyro * kf->dt;

    // Updating phase
    float angle_err = angle_m - kf->angle_f;
    float S = kf->P[0][0] + kf->R_angle;
    float K[2];
    K[0] = kf->P[0][0] / S;
    K[1] = kf->P[1][0] / S;

    // Status update
    kf->angle_f += K[0] * angle_err;
    kf->q_bias += K[1] * angle_err;

    // Covariance update
    float P00_temp = kf->P[0][0];
    float P01_temp = kf->P[0][1];
    kf->P[0][0] -= K[0] * P00_temp;
    kf->P[0][1] -= K[0] * P01_temp;
    kf->P[1][0] -= K[1] * P00_temp;
    kf->P[1][1] -= K[1] * P01_temp;

    return kf->angle_f;
}

// Initialize Fusion filter
void Fusion_Init(FusionFilter *ff, float accMax, float accMin, float alpha, float beta, float dt)
{
    ff->accAngle = 0.0f;
    ff->gyroRate = 0.0f;
    ff->fusedAngle = 0.0f;
    ff->accMax = accMax; // Maximum angle from accelerometer
    ff->accMin = accMin; // Minimum angle from accelerometer
    ff->alpha = alpha; // Fusion coefficient
    ff->beta = beta; // Fusion coefficient
    ff->dt = dt; // Sample time
}

// Fusion filter calculation
float Fusion_Filter(FusionFilter *ff, float angle_m, float gyro_m)
{
    // Calculate accelerometer angle
    ff->accAngle = (angle_m - ff->accMin) / (ff->accMax - ff->accMin) * 180.0f - 90.0f;
    ff->gyroRate = (gyro_m - ff->gyroMin) / (ff->gyroMax - ff->gyroMin) * 2000.0f - 1000.0f;

    // Fuse accelerometer and gyroscope data
    ff->fusedAngle += (ff->beta * (ff->accAngle - ff->fusedAngle) + ff->gyroRate * ff->alpha) * ff->dt;
    return ff->fusedAngle;
}
