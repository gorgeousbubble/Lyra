/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       mpu6050.h
 * @brief      MPU6050 6-axis IMU driver (GPIO bit-bang I2C, PTA28/PTA29)
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "common.h"

// I2C device address (AD0=0 → 0x68 << 1 = 0xD0)
#define I2C_ADR_MPU6050 0xD0

// Register addresses
#define PWR_MGMT_1   0x6B // Power management
#define SMPLRT_DIV   0x19 // Sample rate divider
#define CONFIG       0x1A // DLPF configuration
#define GYRO_CONFIG  0x1B // Gyroscope full-scale range
#define ACCEL_CONFIG 0x1C // Accelerometer full-scale range
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75 // Should read 0x68

// Unit conversion constants
#define RAD_TO_DEG 57.2957795130823208768f
#define DEG_TO_RAD 0.01745329251994329577f

// Accelerometer sensitivity (LSB/g)
#define ACCEL_RANGE_2G  16384.0f
#define ACCEL_RANGE_4G   8192.0f
#define ACCEL_RANGE_8G   4096.0f
#define ACCEL_RANGE_16G  2048.0f

// Gyroscope sensitivity (LSB/°/s)
#define GYRO_RANGE_250  131.0f
#define GYRO_RANGE_500   65.5f
#define GYRO_RANGE_1000  32.8f
#define GYRO_RANGE_2000  16.4f

// GPIO bit-bang I2C pins: SCL=PTA29, SDA=PTA28
#ifndef I2C_PORT_INIT_SCL
#define I2C_PORT_INIT_SCL GPIO_Init(PTA29, GPO, 0)
#endif
#ifndef I2C_PORT_INIT_SDA
#define I2C_PORT_INIT_SDA GPIO_Init(PTA28, GPO, 0)
#endif
#ifndef I2C_DDR_OUT_SCL
#define I2C_DDR_OUT_SCL   GPIO_DDRX(PTA29, 1)
#endif
#ifndef I2C_DDR_OUT_SDA
#define I2C_DDR_OUT_SDA   GPIO_DDRX(PTA28, 1)
#endif
#ifndef I2C_DDR_IN_SDA
#define I2C_DDR_IN_SDA    GPIO_DDRX(PTA28, 0)
#endif
#ifndef I2C_SET_SCL_H
#define I2C_SET_SCL_H     GPIO_SET(PTA29, 1)
#endif
#ifndef I2C_SET_SCL_L
#define I2C_SET_SCL_L     GPIO_SET(PTA29, 0)
#endif
#ifndef I2C_SET_SDA_H
#define I2C_SET_SDA_H     GPIO_SET(PTA28, 1)
#endif
#ifndef I2C_SET_SDA_L
#define I2C_SET_SDA_L     GPIO_SET(PTA28, 0)
#endif
#ifndef I2C_GET_SDA_IN
#define I2C_GET_SDA_IN    GPIO_GET(PTA28)
#endif

#define I2C_DELAY_TIME I2C_Delay(10)

extern void  I2C_GPIO_Start(void);
extern void  I2C_GPIO_Stop(void);
extern void  I2C_GPIO_Send_Ack(int Ack);
extern uint8 I2C_GPIO_Recv_Ack(void);
extern uint8 I2C_GPIO_Send_Byte(uint8 I2C_Data);
extern uint8 I2C_GPIO_Recv_Byte(void);
extern uint8 I2C_GPIO_Write_Reg(uint8 I2C_Div_Adr, uint8 I2C_Reg_Adr, uint8 I2C_Data);
extern uint8 I2C_GPIO_Read_Reg_Byte(uint8 I2C_Div_Adr, uint8 I2C_Reg_Adr);
extern int16 I2C_GPIO_Read_Reg_Word(uint8 I2C_Div_Adr, uint8 I2C_Reg_Adr);
extern int16 MPU_Get_Acc_X(void);
extern int16 MPU_Get_Acc_Y(void);
extern int16 MPU_Get_Acc_Z(void);
extern int16 MPU_Get_Gyro_X(void);
extern int16 MPU_Get_Gyro_Y(void);
extern int16 MPU_Get_Gyro_Z(void);
extern void  MPU6050_Init(void);

#endif
