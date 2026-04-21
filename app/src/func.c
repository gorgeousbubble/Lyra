/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       func.c
 * @brief      UART data transmission for host-side curve visualization
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "func.h"
#include "it.h"
#include "mpu6050.h"
#include "uart.h"

// Send a signed 16-bit value big-endian over UART4
static void UART_Send_Int16(int16 val)
{
    UART_PutChar(UART_UART4, (uint8)(val >> 8));
    UART_PutChar(UART_UART4, (uint8)(val & 0xFF));
}

/*
 * Send one frame of MPU6050 filter data to the host PC.
 *
 * Frame format: [0xFF 0x00] [CH1..CH8, 2 bytes each, big-endian] [0xAA 0x55]
 *
 * CH1 : Pitch accelerometer raw angle  (degrees, int16)
 * CH2 : Pitch complementary filtered   (degrees, int16)
 * CH3 : Roll  accelerometer raw angle  (degrees, int16)
 * CH4 : Roll  complementary filtered   (degrees, int16)
 * CH5 : Yaw   gyro integration         (degrees, int16, drifts over time)
 * CH6 : Gyro Z raw                     (LSB, int16)
 * CH7 : Acc  X raw                     (LSB, int16)
 * CH8 : Acc  Y raw                     (LSB, int16)
 *
 * Called from main loop when UART_Send_Flag is set by PIT1 ISR (every 10ms).
 */
void UART_Send_Parameters(void)
{
    UART_PutChar(UART_UART4, 0xFF);
    UART_PutChar(UART_UART4, 0x00);

    UART_Send_Int16((int16)FF.pitch.acc_angle);
    UART_Send_Int16((int16)FF.pitch.angle);
    UART_Send_Int16((int16)FF.roll.acc_angle);
    UART_Send_Int16((int16)FF.roll.angle);
    UART_Send_Int16((int16)FF.yaw.angle);
    UART_Send_Int16(MPU6050.Gyro.Z);
    UART_Send_Int16(MPU6050.Acc.X);
    UART_Send_Int16(MPU6050.Acc.Y);

    UART_PutChar(UART_UART4, 0xAA);
    UART_PutChar(UART_UART4, 0x55);
}
