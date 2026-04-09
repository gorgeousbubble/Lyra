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
    ff->gyro_bias.bias_x = 0.0f;
    ff->gyro_bias.bias_y = 0.0f;
    ff->gyro_bias.bias_z = 0.0f;
    ff->gyro_bias.calibrated = 0;
}

// Gyro zero-bias calibration (call multiple times while sensor is stationary)
// Averages N samples to determine gyro offset
#define GYRO_CALIB_SAMPLES 200
void Fusion_Calibrate(FusionFilter *ff, int16 gx, int16 gy, int16 gz)
{
    static float sum_x = 0.0f, sum_y = 0.0f, sum_z = 0.0f;
    static int count = 0;

    if (ff->gyro_bias.calibrated) return;

    sum_x += (float)gx / GYRO_RANGE_250;
    sum_y += (float)gy / GYRO_RANGE_250;
    sum_z += (float)gz / GYRO_RANGE_250;
    count++;

    if (count >= GYRO_CALIB_SAMPLES)
    {
        ff->gyro_bias.bias_x = sum_x / (float)count;
        ff->gyro_bias.bias_y = sum_y / (float)count;
        ff->gyro_bias.bias_z = sum_z / (float)count;
        ff->gyro_bias.calibrated = 1;
    }
}

// Normalize angle to [-180, 180) range
static float Normalize_Angle(float angle)
{
    while (angle > 180.0f)  angle -= 360.0f;
    while (angle <= -180.0f) angle += 360.0f;
    return angle;
}

// Complementary filter for single axis
// Uses shortest-path correction to avoid ±180° boundary jump
static float Complementary_Update(float prev_angle, float acc_angle, float gyro_dps, float alpha, float dt)
{
    // Gyro prediction
    float predicted = prev_angle + gyro_dps * dt;

    // Shortest-path error from acc (handles ±180° wrap-around)
    float err = Normalize_Angle(acc_angle - predicted);

    // Correction: blend gyro prediction with acc error
    return Normalize_Angle(predicted + (1.0f - alpha) * err);
}

// Fusion filter calculation (complementary filter)
// Input: raw MPU6050 accelerometer (ax, ay, az) and gyroscope (gx, gy, gz) values
// Assumes: accel range ±2g (16384 LSB/g), gyro range ±250°/s (131 LSB/°/s)
void Fusion_Filter(FusionFilter *ff, int16 ax, int16 ay, int16 az, int16 gx, int16 gy, int16 gz)
{
    float fax = (float)ax;
    float fay = (float)ay;
    float faz = (float)az;

    // Convert gyroscope raw data to °/s and subtract bias
    float gyro_pitch = (float)gy / GYRO_RANGE_250 - ff->gyro_bias.bias_y;
    float gyro_roll  = (float)gx / GYRO_RANGE_250 - ff->gyro_bias.bias_x;
    float gyro_yaw   = (float)gz / GYRO_RANGE_250 - ff->gyro_bias.bias_z;

    // Reject invalid accelerometer data (free-fall or extreme vibration)
    float acc_mag = sqrtf(fax * fax + fay * fay + faz * faz);
    if (acc_mag < ACCEL_RANGE_2G * 0.5f || acc_mag > ACCEL_RANGE_2G * 1.5f)
    {
        ff->pitch.angle = Normalize_Angle(ff->pitch.angle + gyro_pitch * ff->pitch.dt);
        ff->roll.angle  = Normalize_Angle(ff->roll.angle  + gyro_roll  * ff->roll.dt);
        ff->yaw.angle   += gyro_yaw * ff->yaw.dt;
        return;
    }

    // Calculate angles from accelerometer using atan2
    float acc_pitch = atan2f(-fax, faz) * RAD_TO_DEG;
    float acc_roll  = atan2f(fay, faz)  * RAD_TO_DEG;

    // Save raw accelerometer angles for comparison
    ff->pitch.acc_angle = acc_pitch;
    ff->roll.acc_angle  = acc_roll;

    // Complementary filter with wrap-around handling
    ff->pitch.angle = Complementary_Update(ff->pitch.angle, acc_pitch, gyro_pitch, ff->pitch.alpha, ff->pitch.dt);
    ff->roll.angle  = Complementary_Update(ff->roll.angle,  acc_roll,  gyro_roll,  ff->roll.alpha,  ff->roll.dt);

    // Yaw: pure gyro integration (no accelerometer reference)
    ff->yaw.angle += gyro_yaw * ff->yaw.dt;
}
