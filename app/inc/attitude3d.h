/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       attitude3d.h
 * @brief      3D attitude indicator using pitch/roll/yaw from Kalman/complementary filter
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __ATTITUDE3D_H__
#define __ATTITUDE3D_H__

#include "common.h"

/*
 * Render the 3D attitude indicator to OLED (128x64).
 *
 * Left area (0..79, 64 rows):
 *   Draws three coordinate axes (X/Y/Z) rotated by pitch, roll, yaw.
 *   Uses isometric projection with integer fixed-point sin/cos tables.
 *   - X axis: solid line   (→ right in body frame)
 *   - Y axis: dashed line  (→ forward in body frame)
 *   - Z axis: dotted line  (→ up in body frame)
 *   Each axis has an arrowhead and a label.
 *
 * Right area (82..127, 64 rows):
 *   P: +XXX.X  (pitch, degrees)
 *   R: +XXX.X  (roll,  degrees)
 *   Y: +XXX.X  (yaw,   degrees, note: drifts)
 *
 * Parameters:
 *   pitch  - Pitch angle in degrees (from FF.pitch.angle or FF.kf_pitch.angle_f)
 *   roll   - Roll  angle in degrees (from FF.roll.angle  or FF.kf_roll.angle_f)
 *   yaw    - Yaw   angle in degrees (from FF.yaw.angle, gyro-only, drifts)
 */
extern void Render_Attitude3D(float pitch, float roll, float yaw);

#endif
