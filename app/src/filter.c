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
#include "mpu6050.h"

// Initialize Kalman filter
void Kalman_Init(KalmanFilter *kf, float p[2][2], float dt, float q_angle, float q_gyro, float r_angle)
{
    kf->P[0][0] = p[0][0];
    kf->P[0][1] = p[0][1];
    kf->P[1][0] = p[1][0];
    kf->P[1][1] = p[1][1];
    kf->dt = dt;           // Default sampling period of 1ms
    kf->Q_angle = q_angle; // Process noise parameters (angle)
    kf->Q_gyro = q_gyro;   // Process noise parameters (gyroscope)
    kf->R_angle = r_angle; // Measure noise parameters
    kf->q_bias = 0.0f;     // Initial value of gyroscope offset
    kf->angle_f = 0.0f;    // Initial Angle
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
void Fusion_Init(FusionFilter *ff, float alpha, float beta, float dt)
{
    ff->alpha = alpha;  // Default fusion filter coefficient of 0.98
    ff->beta = beta;    // Gyroscope coefficient
    ff->dt = dt;        // Default sampling period of 1ms
    ff->angle_f = 0.0f; // Initial Angle
    ff->angle_l = 0.0f; // Last Angle
    ff->acc_f = 0.0f;   // Initial normalized accelerometer value
    ff->gyro_f = 0.0f;  // Initial normalized gyroscope value
    ff->acc_m = 0.0f;   // Initial accelerometer measurement
    ff->gyro_m = 0.0f;  // Initial gyroscope measurement
}

// Fusion filter calculation
void Fusion_Filter(FusionFilter *ff, float acc_m, float gyro_m)
{
    // Fusion filter calculation
    ff->acc_m = acc_m / 16384.0f;   // Assuming accelerometer measurement is the angle
    ff->gyro_m = gyro_m / 131.0f; // Assuming gyroscope measurement is the angular velocity
    // Normalization of accelerometer and gyroscope data
    // ff->acc_f = ff->acc_m / 16384.0f; // Assuming accelerometer range is ±2g
    // ff->gyro_f = ff->gyro_m / 131.0f; // Assuming gyroscope range is ±250°/s
    ff->acc_f = (int16)(ff->acc_m / 4.0f * 180.0f - 90.0f); // No normalization for accelerometer
    ff->gyro_f = (int16)(ff->gyro_m * ff->beta);                // No normalization for gyroscope

    // Fusion of accelerometer and gyroscope data
    ff->angle_f += (int16)(ff->alpha * (ff->acc_f - ff->angle_l) + (1 - ff->alpha) * (ff->gyro_f * ff->dt));
    ff->angle_l = ff->angle_f; // Update last angle
}
