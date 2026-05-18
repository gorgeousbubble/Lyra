/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       mpu6050.c
 * @brief      MPU6050 GPIO bit-bang I2C driver
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "mpu6050.h"
#include "gpio.h"
#include "i2c.h"

// Generate I2C START condition: SDA falls while SCL is high
void I2C_GPIO_Start(void)
{
  I2C_DDR_OUT_SDA;
  I2C_SET_SDA_H;
  I2C_SET_SCL_H;
  I2C_DELAY_TIME;
  I2C_SET_SDA_L;
  I2C_DELAY_TIME;
  I2C_SET_SCL_L;
}

// Generate I2C STOP condition: SDA rises while SCL is high
void I2C_GPIO_Stop(void)
{
  I2C_DDR_OUT_SDA;
  I2C_SET_SDA_L;
  I2C_SET_SCL_H;
  I2C_DELAY_TIME;
  I2C_SET_SDA_H;
  I2C_DELAY_TIME;
}

// Send ACK (Ack=0) or NACK (Ack=1)
void I2C_GPIO_Send_Ack(int Ack)
{
  I2C_DDR_OUT_SDA;
  if (Ack == 0) { I2C_SET_SDA_L; }
  else          { I2C_SET_SDA_H; }
  I2C_SET_SCL_H;
  I2C_DELAY_TIME;
  I2C_SET_SCL_L;
  I2C_DELAY_TIME;
}

// Read ACK/NACK from slave; returns 0=NACK, 1=ACK
uint8 I2C_GPIO_Recv_Ack(void)
{
  uint8 Recv = 0;
  I2C_DDR_IN_SDA;
  I2C_SET_SCL_H;
  I2C_DELAY_TIME;
  Recv = I2C_GET_SDA_IN;
  I2C_SET_SCL_L;
  I2C_DELAY_TIME;
  return Recv;
}

// Send one byte MSB-first; returns 1 if slave ACKed, 0 if NACKed
uint8 I2C_GPIO_Send_Byte(uint8 I2C_Data)
{
  uint8 i   = 0;
  uint8 Ack = 0;

  I2C_DDR_OUT_SDA;
  for (i = 0; i < 8; i++)
  {
    if ((I2C_Data << i) & 0x80) { I2C_SET_SDA_H; }
    else                        { I2C_SET_SDA_L; }
    I2C_DELAY_TIME;
    I2C_SET_SCL_H;
    I2C_DELAY_TIME;
    I2C_SET_SCL_L;
    I2C_DELAY_TIME;
  }
  I2C_DELAY_TIME;
  I2C_SET_SDA_H;
  I2C_DELAY_TIME;
  I2C_SET_SCL_H;
  I2C_DELAY_TIME;
  I2C_DDR_IN_SDA;
  Ack = (I2C_GET_SDA_IN == 0) ? 1 : 0;
  I2C_SET_SCL_L;
  I2C_DELAY_TIME;
  return Ack;
}

// Receive one byte MSB-first (caller must send ACK/NACK separately)
uint8 I2C_GPIO_Recv_Byte(void)
{
  uint8 i    = 0;
  uint8 Data = 0;

  I2C_DDR_OUT_SDA;
  I2C_SET_SDA_H;
  I2C_DDR_IN_SDA;
  for (i = 0; i < 8; i++)
  {
    Data <<= 1;
    I2C_SET_SCL_H;
    I2C_DELAY_TIME;
    Data |= I2C_GET_SDA_IN;
    I2C_SET_SCL_L;
    I2C_DELAY_TIME;
  }
  return Data;
}

// Write one byte to a register. Returns 1 on success, 0 if any byte was NACKed.
uint8 I2C_GPIO_Write_Reg(uint8 I2C_Div_Adr, uint8 I2C_Reg_Adr, uint8 I2C_Data)
{
  I2C_GPIO_Start();
  if (!I2C_GPIO_Send_Byte(I2C_Div_Adr))  { I2C_GPIO_Stop(); return 0; }
  if (!I2C_GPIO_Send_Byte(I2C_Reg_Adr))  { I2C_GPIO_Stop(); return 0; }
  if (!I2C_GPIO_Send_Byte(I2C_Data))      { I2C_GPIO_Stop(); return 0; }
  I2C_GPIO_Stop();
  return 1;
}

// Read one byte from a register. Returns 0xFF on NACK (bus error).
uint8 I2C_GPIO_Read_Reg_Byte(uint8 I2C_Div_Adr, uint8 I2C_Reg_Adr)
{
  uint8 I2C_Data = 0;

  I2C_GPIO_Start();
  if (!I2C_GPIO_Send_Byte(I2C_Div_Adr))      { I2C_GPIO_Stop(); return 0xFF; }
  if (!I2C_GPIO_Send_Byte(I2C_Reg_Adr))      { I2C_GPIO_Stop(); return 0xFF; }
  I2C_GPIO_Start();                    // Repeated START for read
  if (!I2C_GPIO_Send_Byte(I2C_Div_Adr + 1))  { I2C_GPIO_Stop(); return 0xFF; }
  I2C_Data = I2C_GPIO_Recv_Byte();
  I2C_GPIO_Send_Ack(1);                // NACK to end read
  I2C_GPIO_Stop();

  return I2C_Data;
}

/*
 * Read a 16-bit register atomically in a single I2C transaction.
 * Uses repeated START to avoid data tearing between high and low bytes.
 * Returns 0 on NACK (bus error).
 */
int16 I2C_GPIO_Read_Reg_Word(uint8 I2C_Div_Adr, uint8 I2C_Reg_Adr)
{
  uint8 I2C_Reg_H = 0;
  uint8 I2C_Reg_L = 0;

  I2C_GPIO_Start();
  if (!I2C_GPIO_Send_Byte(I2C_Div_Adr))      { I2C_GPIO_Stop(); return 0; }
  if (!I2C_GPIO_Send_Byte(I2C_Reg_Adr))      { I2C_GPIO_Stop(); return 0; }
  I2C_GPIO_Start();                    // Repeated START
  if (!I2C_GPIO_Send_Byte(I2C_Div_Adr + 1))  { I2C_GPIO_Stop(); return 0; }
  I2C_Reg_H = I2C_GPIO_Recv_Byte();
  I2C_GPIO_Send_Ack(0);                // ACK: more bytes to read
  I2C_Reg_L = I2C_GPIO_Recv_Byte();
  I2C_GPIO_Send_Ack(1);                // NACK: last byte
  I2C_GPIO_Stop();

  return (int16)((I2C_Reg_H << 8) | I2C_Reg_L);
}

int16 MPU_Get_Acc_X(void)  { return I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050, ACCEL_XOUT_H); }
int16 MPU_Get_Acc_Y(void)  { return I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050, ACCEL_YOUT_H); }
int16 MPU_Get_Acc_Z(void)  { return I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050, ACCEL_ZOUT_H); }
int16 MPU_Get_Gyro_X(void) { return I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050, GYRO_XOUT_H);  }
int16 MPU_Get_Gyro_Y(void) { return I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050, GYRO_YOUT_H);  }
int16 MPU_Get_Gyro_Z(void) { return I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050, GYRO_ZOUT_H);  }

/*
 * MPU6050 initialization
 * Sample rate : 125Hz  (SMPLRT_DIV=7, 1kHz / (1+7))
 * DLPF        : 20Hz   (CONFIG=4, reduces high-frequency noise)
 * Accel range : ±2g    (ACCEL_CONFIG=0x01)
 * Gyro range  : ±250°/s (GYRO_CONFIG=0x00)
 */
void MPU6050_Init(void)
{
  I2C_PORT_INIT_SCL;
  I2C_PORT_INIT_SDA;
  I2C_GPIO_Write_Reg(I2C_ADR_MPU6050, PWR_MGMT_1,   0x00); // Wake up
  I2C_GPIO_Write_Reg(I2C_ADR_MPU6050, SMPLRT_DIV,   0x07); // 125Hz
  I2C_GPIO_Write_Reg(I2C_ADR_MPU6050, CONFIG,        0x04); // DLPF 20Hz
  I2C_GPIO_Write_Reg(I2C_ADR_MPU6050, ACCEL_CONFIG,  0x01); // ±2g
  I2C_GPIO_Write_Reg(I2C_ADR_MPU6050, GYRO_CONFIG,   0x00); // ±250°/s
}
