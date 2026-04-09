/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       func.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "func.h"
#include "it.h"
#include "mpu6050.h"
#include "uart.h"

// Send a 16-bit signed value over UART (big-endian)
static void UART_Send_Int16(int16 val)
{
    UART_PutChar(UART_UART4, (uint8)(val >> 8));
    UART_PutChar(UART_UART4, (uint8)(val & 0xFF));
}

/*
 * @brief      UART_Send_Parameters
 *             Send MPU6050 filter data to host for curve visualization
 *             Protocol: [0xFF 0x00] [CH1~CH8 x 2bytes] [0xAA 0x55]
 *
 *             Curve(1): Pitch acc raw angle   (degrees)
 *             Curve(2): Pitch filtered angle  (degrees)
 *             Curve(3): Roll acc raw angle    (degrees)
 *             Curve(4): Roll filtered angle   (degrees)
 *             Curve(5): Yaw gyro integrated   (degrees)
 *             Curve(6): Gyro Z raw            (raw int16)
 *             Curve(7): Acc X raw             (raw int16)
 *             Curve(8): Acc Y raw             (raw int16)
 *
 * @return     void
 * @since      v1.0
 */
void UART_Send_Parameters(void)
{
    // Frame header
    UART_PutChar(UART_UART4, 0xFF);
    UART_PutChar(UART_UART4, 0x00);

    // Pitch: acc raw angle vs filtered angle (direct degrees)
    UART_Send_Int16((int16)FF.pitch.acc_angle);
    UART_Send_Int16((int16)FF.pitch.angle);

    // Roll: acc raw angle vs filtered angle
    UART_Send_Int16((int16)FF.roll.acc_angle);
    UART_Send_Int16((int16)FF.roll.angle);

    // Yaw: gyro integration angle
    UART_Send_Int16((int16)FF.yaw.angle);

    // Gyro Z raw (yaw axis reference)
    UART_Send_Int16(MPU6050.Gyro.Z);

    // Accelerometer raw data
    UART_Send_Int16(MPU6050.Acc.X);
    UART_Send_Int16(MPU6050.Acc.Y);

    // Frame tail
    UART_PutChar(UART_UART4, 0xAA);
    UART_PutChar(UART_UART4, 0x55);
}
