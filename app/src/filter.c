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
#include <math.h>

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

// Initialize Fusion filter (complementary filter)
void Fusion_Init(FusionFilter *ff, float alpha, float dt)
{
    ff->pitch.angle = 0.0f;
    ff->pitch.acc_angle = 0.0f;
    ff->pitch.alpha = alpha;
    ff->pitch.dt = dt;
    ff->roll.angle = 0.0f;
    ff->roll.acc_angle = 0.0f;
    ff->roll.alpha = alpha;
    ff->roll.dt = dt;
    ff->yaw.angle = 0.0f;
    ff->yaw.dt = dt;
}

// Fusion filter calculation (complementary filter)
// Input: raw MPU6050 accelerometer (ax, ay, az) and gyroscope (gx, gy) values
// Assumes: accel range ±2g (16384 LSB/g), gyro range ±250°/s (131 LSB/°/s)
void Fusion_Filter(FusionFilter *ff, int16 ax, int16 ay, int16 az, int16 gx, int16 gy, int16 gz)
{
    float fax = (float)ax;
    float fay = (float)ay;
    float faz = (float)az;

    // Reject invalid accelerometer data (free-fall or extreme vibration)
    float acc_mag = sqrtf(fax * fax + fay * fay + faz * faz);
    if (acc_mag < ACCEL_RANGE_2G * 0.5f || acc_mag > ACCEL_RANGE_2G * 1.5f)
    {
        float gyro_pitch = (float)gy / GYRO_RANGE_250;
        float gyro_roll  = (float)gx / GYRO_RANGE_250;
        ff->pitch.angle += gyro_pitch * ff->pitch.dt;
        ff->roll.angle  += gyro_roll  * ff->roll.dt;
        ff->yaw.angle   += (float)gz / GYRO_RANGE_250 * ff->yaw.dt;
        return;
    }

    // Calculate angles from accelerometer using atan2
    // atan2(-ax, az) gives continuous ±180° pitch without jump at ±90°
    // atan2(ay, az)  gives continuous ±180° roll  without jump at ±90°
    float acc_pitch = atan2f(-fax, faz) * RAD_TO_DEG;
    float acc_roll  = atan2f(fay, faz)  * RAD_TO_DEG;

    // Save raw accelerometer angles for comparison
    ff->pitch.acc_angle = acc_pitch;
    ff->roll.acc_angle  = acc_roll;

    // Convert gyroscope raw data to °/s
    float gyro_pitch = (float)gy / GYRO_RANGE_250;
    float gyro_roll  = (float)gx / GYRO_RANGE_250;

    // Complementary filter: angle = alpha * (angle + gyro * dt) + (1 - alpha) * acc_angle
    ff->pitch.angle = ff->pitch.alpha * (ff->pitch.angle + gyro_pitch * ff->pitch.dt) + (1.0f - ff->pitch.alpha) * acc_pitch;
    ff->roll.angle  = ff->roll.alpha  * (ff->roll.angle  + gyro_roll  * ff->roll.dt)  + (1.0f - ff->roll.alpha)  * acc_roll;

    // Yaw: pure gyro integration (no accelerometer reference)
    ff->yaw.angle += (float)gz / GYRO_RANGE_250 * ff->yaw.dt;
}
