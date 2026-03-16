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
void Fusion_Init()
{

}

// Initialize Fusion filter
void Fusion_Sensor_Covert(float* acc_norm, float* gyro_norm, const int acc_raw[3], const int gyro_raw[3], int acc_range, int gyro_range)
{
    // accelerometer‌ coefficient convertion
    float acc_scale = 0.0f;
    if (acc_range == 2) 
    {
        acc_scale = ACCEL_RANGE_2G;
    }
    else if (acc_range == 4) 
    {
        acc_scale = ACCEL_RANGE_4G;
    }
    else if (acc_range == 8) 
    {
        acc_scale = ACCEL_RANGE_8G;
    }
    else 
    {
        acc_scale = ACCEL_RANGE_16G;
    }
    // accelerometer data conversion
    acc_norm[0] = (float)acc_raw[0] / acc_scale; // X-axis acceleration
    acc_norm[1] = (float)acc_raw[1] / acc_scale;
    acc_norm[2] = (float)acc_raw[2] / acc_scale;

    // gyroscope coefficient convertion
    float gyro_scale = 0.0f;
    if (gyro_range == 250)
    {
        gyro_scale = GYRO_RANGE_250;
    }
    else if (gyro_range == 500)
    {
        gyro_scale = GYRO_RANGE_500;
    }
    else if (gyro_range == 1000)
    {
        gyro_scale = GYRO_RANGE_1000;
    }
    else
    {
        gyro_scale = GYRO_RANGE_2000;
    }
    // gyroscope data conversion
    gyro_norm[0] = (float)gyro_raw[0] / gyro_scale * DEG_TO_RAD; // X-axis angular velocity
    gyro_norm[1] = (float)gyro_raw[1] / gyro_scale * DEG_TO_RAD;
    gyro_norm[2] = (float)gyro_raw[2] / gyro_scale * DEG_TO_RAD;
}
