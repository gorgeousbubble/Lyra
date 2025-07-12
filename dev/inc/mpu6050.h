/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       mpu6050.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "common.h"

/*
**MPU6050 Internal register macro
*/
//MPU6050 Device Address
#define I2C_ADR_MPU6050 0xD0

//MPU6050 Internal register address
#define	PWR_MGMT_1      0x6B	//Power management, typical value: 0x00 (normally enabled)
#define	SMPLRT_DIV	0x19	//Gyroscope sampling rate, typical value: 0x07 (125Hz)
#define	CONFIG		0x1A	//Low pass filtering frequency, typical value: 0x06 (5Hz)
#define	GYRO_CONFIG	0x1B	//Gyroscope self-test and measurement range, typical value: 0x18 (no self-test, 2000deg/s)
#define	ACCEL_CONFIG	0x1C	//Accelerometer self-test, measurement range and high pass filtering frequency, typical value: 0x01 (no self-test, 2G, 5Hz)
#define	ACCEL_XOUT_H	0x3B    //High level output of accelerometer X-axis
#define	ACCEL_XOUT_L	0x3C    //Low level output of accelerometer X-axis
#define	ACCEL_YOUT_H	0x3D    //High level output of accelerometer Y-axis
#define	ACCEL_YOUT_L	0x3E    //Low level output of accelerometer Y-axis
#define	ACCEL_ZOUT_H	0x3F    //High level output of accelerometer Z-axis
#define	ACCEL_ZOUT_L	0x40    //Low level output of accelerometer Z-axis
#define	TEMP_OUT_H      0x41    //High level output of temperature sensor
#define	TEMP_OUT_L	0x42    //Low level output of temperature sensor
#define	GYRO_XOUT_H	0x43    //Gyroscope X-axis high-level output
#define	GYRO_XOUT_L	0x44	//Gyroscope X-axis low-level output
#define	GYRO_YOUT_H	0x45    //Gyroscope Y-axis high-level output
#define	GYRO_YOUT_L	0x46    //Gyroscope Y-axis low-level output
#define	GYRO_ZOUT_H	0x47    //Gyroscope Z-axis high-level output
#define	GYRO_ZOUT_L	0x48    //Gyroscope Z-axis low-level output
#define	WHO_AM_I        0x75	//I2C address register (default value 0x68, read-only)

//I2C Port Macro
//I2C SCL port initialization
#ifndef I2C_PORT_INIT_SCL
#define I2C_PORT_INIT_SCL       GPIO_Init(PTA29,GPO,0)
#endif
//I2C SDA port initialization
#ifndef I2C_PORT_INIT_SDA
#define I2C_PORT_INIT_SDA       GPIO_Init(PTA28,GPO,0)
#endif
//I2C SCL port output
#ifndef I2C_DDR_OUT_SCL
#define I2C_DDR_OUT_SCL         GPIO_DDRX(PTA29,1)
#endif
//I2C SDA port output
#ifndef I2C_DDR_OUT_SDA
#define I2C_DDR_OUT_SDA         GPIO_DDRX(PTA28,1)
#endif
//I2C SDA port input
#ifndef I2C_DDR_IN_SDA
#define I2C_DDR_IN_SDA          GPIO_DDRX(PTA28,0)
#endif
//I2C SCL port high level
#ifndef I2C_SET_SCL_H
#define I2C_SET_SCL_H           GPIO_SET(PTA29,1)
#endif
//I2C SCL port low level
#ifndef I2C_SET_SCL_L
#define I2C_SET_SCL_L           GPIO_SET(PTA29,0)
#endif
//I2C SDA port high level
#ifndef I2C_SET_SDA_H
#define I2C_SET_SDA_H           GPIO_SET(PTA28,1)
#endif
//I2C SDA port low level
#ifndef I2C_SET_SDA_L
#define I2C_SET_SDA_L           GPIO_SET(PTA28,0)
#endif
//I2C SDA port input signal
#ifndef I2C_GET_SDA_IN
#define I2C_GET_SDA_IN          GPIO_GET(PTA28)
#endif

#define I2C_DELAY_TIME          I2C_Delay(10)

/*
**function declaration
*/
extern void I2C_GPIO_Start(void);
extern void I2C_GPIO_Stop(void);
extern void I2C_GPIO_Send_Ack(int Ack);
extern uint8 I2C_GPIO_Recv_Ack(void);
extern uint8 I2C_GPIO_Send_Byte(uint8 I2C_Data);
extern uint8 I2C_GPIO_Recv_Byte(void);
extern void I2C_GPIO_Write_Reg(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr,uint8 I2C_Data);
extern uint8 I2C_GPIO_Read_Reg_Byte(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr);
extern int I2C_GPIO_Read_Reg_Word(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr);
extern int MPU_Get_Acc_X(void);
extern int MPU_Get_Acc_Y(void);
extern int MPU_Get_Acc_Z(void);
extern int MPU_Get_Gyro_X(void);
extern int MPU_Get_Gyro_Y(void);
extern int MPU_Get_Gyro_Z(void);
extern void MPU6050_Init(void);

#endif