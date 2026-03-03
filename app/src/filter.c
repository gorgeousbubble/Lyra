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

// Initialize complementary filter
void Complementary_Filter_Init(ComplementaryFilter *filter, float kp, float ki, float dt)
{
    filter->kp = kp;
    filter->ki = ki;
    filter->dt = dt;

    // Initial quaternion (no rotation)
    filter->quat.q0 = 1.0f;
    filter->quat.q1 = 0.0f;
    filter->quat.q2 = 0.0f;
    filter->quat.q3 = 0.0f;

    // Initial Euler angles
    filter->angle.roll = 0.0f;
    filter->angle.pitch = 0.0f;
    filter->angle.yaw = 0.0f;

    // Integral error initialization
    filter->exInt = 0.0f;
    filter->eyInt = 0.0f;
    filter->ezInt = 0.0f;

    filter->initialized = 1;
}

// Complementary Filter Update
void Complementary_Filter_Update(ComplementaryFilter *filter, float ax, float ay, float az, float gx, float gy, float gz)
{
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    float q0, q1, q2, q3;

    // get current quaternion values
    q0 = filter->quat.q0;
    q1 = filter->quat.q1;
    q2 = filter->quat.q2;
    q3 = filter->quat.q3;

    // normalize accelerometer measurement
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm > 0.01f)
    {
        ax /= norm;
        ay /= norm;
        az /= norm;
    }
    else
    {
        return;
    }

    // calculate the estimated direction of gravity
    vx = 2.0f * (q1 * q3 - q0 * q2);
    vy = 2.0f * (q0 * q1 + q2 * q3);
    vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

    // error is cross product between estimated and measured direction of gravity
    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);

    // integral error scaled by Ki
    filter->exInt += ex * filter->ki;
    filter->eyInt += ey * filter->ki;
    filter->ezInt += ez * filter->ki;

    // apply feedback terms
    gx += filter->kp * ex + filter->exInt;
    gy += filter->kp * ey + filter->eyInt;
    gz += filter->kp * ez + filter->ezInt;

    // forward integrate rate of change of quaternion
    gx *= 0.5f * filter->dt;
    gy *= 0.5f * filter->dt;
    gz *= 0.5f * filter->dt;

    // update quaternion values
    float qa = q0;
    float qb = q1;
    float qc = q2;

    q0 += (-qb * gx - qc * gy - q3 * gz);
    q1 += (qa * gx + qc * gz - q3 * gy);
    q2 += (qa * gy - qb * gz + q3 * gx);
    q3 += (qa * gz + qb * gy - qc * gx);

    // normalize quaternion
    norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    if (norm > 0.0f)
    {
        q0 /= norm;
        q1 /= norm;
        q2 /= norm;
        q3 /= norm;
    }

    // save updated quaternion values
    filter->quat.q0 = q0;
    filter->quat.q1 = q1;
    filter->quat.q2 = q2;
    filter->quat.q3 = q3;

    // calculate Euler angles from quaternion
    Calculate_Euler_Angles(filter);
}

// Convert Sensor Data
void Convert_Sensor_Data(float *accel, float *gyro, const int acc_raw[3], const int gyro_raw[3], float acc_range, float gyro_range)
{
    // accelarometer conversion coefficient
    float acc_scale;
    if (acc_range == 2)
        acc_scale = ACCEL_RANGE_2G;
    else if (acc_range == 4)
        acc_scale = ACCEL_RANGE_4G;
    else if (acc_range == 8)
        acc_scale = ACCEL_RANGE_8G;
    else
        acc_scale = ACCEL_RANGE_16G;

    // convert to g
    *accel = acc_raw[0] / acc_scale;
    *(accel + 1) = acc_raw[1] / acc_scale;
    *(accel + 2) = acc_raw[2] / acc_scale;

    // gyroscope conversion coefficient
    float gyro_scale;
    if (gyro_range == 250)
        gyro_scale = GYRO_RANGE_250;
    else if (gyro_range == 500)
        gyro_scale = GYRO_RANGE_500;
    else if (gyro_range == 1000)
        gyro_scale = GYRO_RANGE_1000;
    else
        gyro_scale = GYRO_RANGE_2000;

    // convert to degrees per second
    *gyro = gyro_raw[0] / gyro_scale * DEG_TO_RAD;
    *(gyro + 1) = gyro_raw[1] / gyro_scale * DEG_TO_RAD;
    *(gyro + 2) = gyro_raw[2] / gyro_scale * DEG_TO_RAD;
}

// Calculate Euler Angles
void Calculate_Euler_Angles(ComplementaryFilter *filter)
{
    float q0 = filter->quat.q0;
    float q1 = filter->quat.q1;
    float q2 = filter->quat.q2;
    float q3 = filter->quat.q3;

    // rotation around X-axis (roll)
    float sinr_cosp = 2.0f * (q0 * q1 + q2 * q3);
    float cosr_cosp = 1.0f - 2.0f * (q1 * q1 + q2 * q2);
    filter->angle.roll = atan2(sinr_cosp, cosr_cosp);

    // rotation around Y-axis (pitch)
    float sinp = 2.0f * (q0 * q2 - q3 * q1);
    if (fabs(sinp) >= 1.0f)
        filter->angle.pitch = copysign(PI / 2.0f, sinp);
    else
        filter->angle.pitch = asin(sinp);

    // rotation around Z-axis (yaw)
    float siny_cosp = 2.0f * (q0 * q3 + q1 * q2);
    float cosy_cosp = 1.0f - 2.0f * (q2 * q2 + q3 * q3);
    filter->angle.yaw = atan2(siny_cosp, cosy_cosp);

    // convert radians to degrees
    filter->angle.roll *= RAD_TO_DEG;
    filter->angle.pitch *= RAD_TO_DEG;
    filter->angle.yaw *= RAD_TO_DEG;

    // normalize angles to [-180, 180]
    filter->angle.roll = Normalize_Angle(filter->angle.roll);
    filter->angle.pitch = Normalize_Angle(filter->angle.pitch);
    filter->angle.yaw = Normalize_Angle(filter->angle.yaw);
}

// Normalize Angle
float Normalize_Angle(float angle)
{
    while (angle > 90.0f)
        angle -= 180.0f;
    while (angle < -90.0f)
        angle += 180.0f;
    return angle;
}

// Get Attitude Angles
void Get_Attitude_Angles(ComplementaryFilter *filter, float *roll, float *pitch, float *yaw)
{
    if (roll)
        *roll = filter->angle.roll;
    if (pitch)
        *pitch = filter->angle.pitch;
    if (yaw)
        *yaw = filter->angle.yaw;
}

// Get Quaternion
void Get_Quaternion(ComplementaryFilter *filter, float *q0, float *q1, float *q2, float *q3)
{
    if (q0)
        *q0 = filter->quat.q0;
    if (q1)
        *q1 = filter->quat.q1;
    if (q2)
        *q2 = filter->quat.q2;
    if (q3)
        *q3 = filter->quat.q3;
}