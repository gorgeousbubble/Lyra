/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       filter.c
 * @brief      MPU6050 Kalman filter and complementary filter implementation
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "filter.h"
#include "mpu6050.h"
#include <math.h>

/*
 * Kalman filter initialization
 * p        : initial covariance matrix (2x2)
 * dt       : sample period in seconds
 * q_angle  : process noise for angle state
 * q_gyro   : process noise for gyro bias state
 * r_angle  : measurement noise
 */
void Kalman_Init(KalmanFilter *kf, float p[2][2], float dt, float q_angle, float q_gyro, float r_angle)
{
    kf->P[0][0] = p[0][0];
    kf->P[0][1] = p[0][1];
    kf->P[1][0] = p[1][0];
    kf->P[1][1] = p[1][1];
    kf->dt      = dt;
    kf->Q_angle = q_angle;
    kf->Q_gyro  = q_gyro;
    kf->R_angle = r_angle;
    kf->q_bias  = 0.0f;
    kf->angle_f = 0.0f;
    // Mark as NOT yet seeded from accelerometer.
    // Fusion_Filter will seed it on the first valid acc sample.
    kf->initialized = 0;
}

/*
 * Kalman filter update
 * angle_m : angle measured by accelerometer (degrees)
 * gyro_m  : angular velocity measured by gyroscope (degrees/s)
 * returns : filtered angle (degrees)
 */
float Kalman_Filter(KalmanFilter *kf, float angle_m, float gyro_m)
{
    // Predict
    kf->angle_f += (gyro_m - kf->q_bias) * kf->dt;
    kf->P[0][0] += kf->dt * (kf->dt * kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
    kf->P[0][1] -= kf->dt * kf->P[1][1];
    kf->P[1][0] -= kf->dt * kf->P[1][1];
    kf->P[1][1] += kf->Q_gyro * kf->dt;

    // Update
    float angle_err = angle_m - kf->angle_f;
    float S  = kf->P[0][0] + kf->R_angle;
    if (S < 1e-9f) S = 1e-9f; // Guard against division by zero
    float K0 = kf->P[0][0] / S;
    float K1 = kf->P[1][0] / S;

    kf->angle_f += K0 * angle_err;
    kf->q_bias  += K1 * angle_err;

    float P00 = kf->P[0][0];
    float P01 = kf->P[0][1];
    kf->P[0][0] -= K0 * P00;
    kf->P[0][1] -= K0 * P01;
    kf->P[1][0] -= K1 * P00;
    kf->P[1][1] -= K1 * P01;

    return kf->angle_f;
}

/*
 * Kalman filter reset - reinitialize angle state from a known angle
 * Call this when the sensor is first powered on or after a long acc-rejection period
 */
void Kalman_Reset(KalmanFilter *kf, float initial_angle)
{
    kf->angle_f      = initial_angle;
    kf->q_bias       = 0.0f;
    kf->P[0][0]      = 1.0f;
    kf->P[0][1]      = 0.0f;
    kf->P[1][0]      = 0.0f;
    kf->P[1][1]      = 1.0f;
    kf->initialized  = 1;  // Caller has provided a known starting angle
}

/*
 * Complementary filter initialization
 * alpha : gyro weight (0~1), recommended 0.93~0.98
 * dt    : sample period in seconds
 */
void Fusion_Init(FusionFilter *ff, float alpha, float dt)
{
    ff->pitch.angle     = 0.0f;
    ff->pitch.acc_angle = 0.0f;
    ff->pitch.alpha     = alpha;
    ff->pitch.dt        = dt;
    ff->roll.angle      = 0.0f;
    ff->roll.acc_angle  = 0.0f;
    ff->roll.alpha      = alpha;
    ff->roll.dt         = dt;
    ff->yaw.angle       = 0.0f;
    ff->yaw.dt          = dt;
    ff->gyro_bias.bias_x     = 0.0f;
    ff->gyro_bias.bias_y     = 0.0f;
    ff->gyro_bias.bias_z     = 0.0f;
    ff->gyro_bias.sum_x      = 0.0f;
    ff->gyro_bias.sum_y      = 0.0f;
    ff->gyro_bias.sum_z      = 0.0f;
    ff->gyro_bias.count      = 0;
    ff->gyro_bias.calibrated = 0;

    // Kalman filter initialization
    // Q_angle: process noise for angle  - larger = faster response to acc, more noise
    // Q_gyro:  process noise for bias   - larger = faster bias tracking
    // R_angle: measurement noise        - smaller = more trust in accelerometer
    float p_init[2][2] = {{1.0f, 0.0f}, {0.0f, 1.0f}};
    Kalman_Init(&ff->kf_pitch, p_init, dt, 0.01f, 0.003f, 0.01f);
    Kalman_Init(&ff->kf_roll,  p_init, dt, 0.01f, 0.003f, 0.01f);
}

/*
 * Gyroscope zero-bias calibration
 * Call repeatedly at 100Hz while sensor is stationary.
 * Collects GYRO_CALIB_SAMPLES samples (2 seconds) then computes average bias.
 * Sets calibrated flag when done; subsequent calls are no-ops.
 */
#define GYRO_CALIB_SAMPLES 200
void Fusion_Calibrate(FusionFilter *ff, int16 gx, int16 gy, int16 gz)
{
    if (ff->gyro_bias.calibrated) return;

    ff->gyro_bias.sum_x += (float)gx / GYRO_RANGE_250;
    ff->gyro_bias.sum_y += (float)gy / GYRO_RANGE_250;
    ff->gyro_bias.sum_z += (float)gz / GYRO_RANGE_250;
    ff->gyro_bias.count++;

    if (ff->gyro_bias.count >= GYRO_CALIB_SAMPLES)
    {
        ff->gyro_bias.bias_x = ff->gyro_bias.sum_x / (float)ff->gyro_bias.count;
        ff->gyro_bias.bias_y = ff->gyro_bias.sum_y / (float)ff->gyro_bias.count;
        ff->gyro_bias.bias_z = ff->gyro_bias.sum_z / (float)ff->gyro_bias.count;
        ff->gyro_bias.calibrated = 1;
    }
}

// Wrap angle into [-180, 180) using fmodf. Guards against NaN/infinity.
static float Normalize_Angle(float angle)
{
    if (angle != angle || angle > 1e9f || angle < -1e9f)
        return 0.0f;
    angle = fmodf(angle + 180.0f, 360.0f);
    if (angle < 0.0f)
        angle += 360.0f;
    return angle - 180.0f;
}

#if FILTER_MODE != FILTER_MODE_KALMAN
/*
 * Single-axis complementary filter update with shortest-path correction.
 * Predicts with gyro, then corrects toward accelerometer angle via the
 * shortest angular path to avoid jumps at the ±180° boundary.
 * (Only compiled when the complementary filter is selected; the Kalman
 *  build does not reference it.)
 */
static float Complementary_Update(float prev_angle, float acc_angle, float gyro_dps, float alpha, float dt)
{
    float predicted = prev_angle + gyro_dps * dt;
    float err       = Normalize_Angle(acc_angle - predicted);
    return Normalize_Angle(predicted + (1.0f - alpha) * err);
}
#endif

/*
 * Complementary filter update for pitch, roll and yaw.
 *
 * Input (raw MPU6050 values):
 *   ax, ay, az : accelerometer  (±2g range, 16384 LSB/g)
 *   gx, gy, gz : gyroscope      (±250°/s range, 131 LSB/°/s)
 *
 * Pitch and roll use accelerometer + gyro fusion (complementary or Kalman,
 * selected by FILTER_MODE in filter.h).
 * Yaw uses gyro integration only (no absolute reference without magnetometer).
 * Accelerometer data is rejected when magnitude deviates >50% from 1g
 * (free-fall or strong vibration), falling back to gyro-only integration.
 */
void Fusion_Filter(FusionFilter *ff, int16 ax, int16 ay, int16 az, int16 gx, int16 gy, int16 gz)
{
    float fax = (float)ax;
    float fay = (float)ay;
    float faz = (float)az;

    float gyro_pitch = (float)gy / GYRO_RANGE_250 - ff->gyro_bias.bias_y;
    float gyro_roll  = (float)gx / GYRO_RANGE_250 - ff->gyro_bias.bias_x;
    float gyro_yaw   = (float)gz / GYRO_RANGE_250 - ff->gyro_bias.bias_z;

    // Reject accelerometer when magnitude is far from 1g
    float acc_mag = sqrtf(fax * fax + fay * fay + faz * faz);
    if (acc_mag < ACCEL_RANGE_2G * 0.5f || acc_mag > ACCEL_RANGE_2G * 1.5f)
    {
        ff->pitch.angle = Normalize_Angle(ff->pitch.angle + gyro_pitch * ff->pitch.dt);
        ff->roll.angle  = Normalize_Angle(ff->roll.angle  + gyro_roll  * ff->roll.dt);
        // Yaw: wrap here too for the same float-precision reason
        ff->yaw.angle   = Normalize_Angle(ff->yaw.angle   + gyro_yaw   * ff->yaw.dt);
        // Keep Kalman internal state in sync to avoid output jump when acc recovers
        ff->kf_pitch.angle_f = ff->pitch.angle;
        ff->kf_roll.angle_f  = ff->roll.angle;
        return;
    }

    // atan2(-ax, az) gives continuous ±180° pitch; atan2(ay, az) gives roll
    float acc_pitch = atan2f(-fax, faz) * RAD_TO_DEG;
    float acc_roll  = atan2f(fay,  faz) * RAD_TO_DEG;

    ff->pitch.acc_angle = acc_pitch;
    ff->roll.acc_angle  = acc_roll;

#if FILTER_MODE == FILTER_MODE_KALMAN
    // ----------------------------------------------------------------
    // Seed Kalman state from accelerometer on the very first valid sample.
    //
    // OLD (buggy):
    //   if (kf_pitch.angle_f == 0.0f && kf_roll.angle_f == 0.0f)
    //
    // The old check fails when the device starts truly horizontal
    // (acc_pitch == 0 AND acc_roll == 0): both angle_f fields stay 0
    // after the seed assignment, so the condition remains true on every
    // subsequent call and the Kalman state is re-seeded each frame —
    // effectively bypassing the filter entirely.
    //
    // FIX: use a dedicated 'initialized' flag that is set once and never
    // cleared except by an explicit Kalman_Init / Kalman_Reset call.
    // ----------------------------------------------------------------
    if (!ff->kf_pitch.initialized)
    {
        ff->kf_pitch.angle_f    = acc_pitch;
        ff->kf_pitch.initialized = 1;
    }
    if (!ff->kf_roll.initialized)
    {
        ff->kf_roll.angle_f    = acc_roll;
        ff->kf_roll.initialized = 1;
    }
    ff->pitch.angle = Kalman_Filter(&ff->kf_pitch, acc_pitch, gyro_pitch);
    ff->roll.angle  = Kalman_Filter(&ff->kf_roll,  acc_roll,  gyro_roll);
#else
    ff->pitch.angle = Complementary_Update(ff->pitch.angle, acc_pitch, gyro_pitch, ff->pitch.alpha, ff->pitch.dt);
    ff->roll.angle  = Complementary_Update(ff->roll.angle,  acc_roll,  gyro_roll,  ff->roll.alpha,  ff->roll.dt);
#endif

    // ----------------------------------------------------------------
    // Yaw: gyro-only integration.
    //
    // Wrap into [-180, 180) after EVERY step.
    //
    // WHY: float has ~7 significant digits.  An unclamped yaw value
    // accumulating at 10 °/s reaches ~864 000 ° in 24 hours, at which
    // point the representable step size is ~0.06 ° — larger than the
    // sensor noise floor (~0.02 °/s × 0.01 s = 0.0002 °/step).
    // Keeping the value in [-180, 180) preserves full float precision
    // indefinitely, at the cost of a single fmodf call per 10 ms.
    // ----------------------------------------------------------------
    ff->yaw.angle = Normalize_Angle(ff->yaw.angle + gyro_yaw * ff->yaw.dt);
}
