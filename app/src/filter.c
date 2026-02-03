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
#include <math.h>

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

// Fusion filter calculation
void Fusion_Filter(FusionFilter *ff, float angle_roll, float angle_pitch, float angle_yaw, float gyro_roll, float gyro_pitch, float gyro_yaw)
{
    // Accelerometer angle
    ff->accAngleRoll = atan2f(angle_pitch, angle_yaw) * (180.0f / 3.14159265358979323846f);
    ff->accAnglePitch = atan2f(-angle_roll, sqrtf(angle_pitch * angle_pitch + angle_yaw * angle_yaw)) * (180.0f / 3.14159265358979323846f);
    ff->accAngleYaw = atan2f(angle_roll, angle_pitch) * (180.0f / 3.14159265358979323846f);

    // Gyroscope angular velocity
    ff->gyroRateRoll = gyro_roll / 16.4f;   // Assuming sensitivity is set to ±2000°/s
    ff->gyroRatePitch = gyro_pitch / 16.4f; // Assuming sensitivity is set to ±2000°/s
    ff->gyroRateYaw = gyro_yaw / 16.4f;     // Assuming sensitivity is set to ±2000°/s

    // Calculate fused angles
    ff->fusedAngleRoll = ff->alpha * (ff->fusedAngleRoll + ff->gyroRateRoll * ff->dt) + (1.0f - ff->alpha) * ff->accAngleRoll;
    ff->fusedAnglePitch = ff->alpha * (ff->fusedAnglePitch + ff->gyroRatePitch * ff->dt) + (1.0f - ff->alpha) * ff->accAnglePitch;
    ff->fusedAngleYaw = ff->alpha * (ff->fusedAngleYaw + ff->gyroRateYaw * ff->dt) + (1.0f - ff->alpha) * ff->accAngleYaw;
}