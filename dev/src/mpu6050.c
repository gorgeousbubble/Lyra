/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       mpu6050.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "mpu6050.h"
#include "gpio.h"
#include "i2c.h"

/*
**I2C start signal
*/
void I2C_GPIO_Start(void)
{
  I2C_DDR_OUT_SDA;      //SDA output
  I2C_SET_SDA_H;        //SDA high level
  I2C_SET_SCL_H;        //SCL high level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
  I2C_SET_SDA_L;        //SDA low level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
  I2C_SET_SCL_L;        //SCL low level
}

/*
**I2C stop signal
*/
void I2C_GPIO_Stop(void)
{
  I2C_DDR_OUT_SDA;      //SDA output
  I2C_SET_SDA_L;        //SDA low level
  I2C_SET_SCL_H;        //SCL high level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
  I2C_SET_SDA_H;        //SDA high level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
}

/*
**I2C sends response signal
*/
void I2C_GPIO_Send_Ack(int Ack)
{
  I2C_DDR_OUT_SDA;      //SDA output
  if(Ack == 0)
  {
    I2C_SET_SDA_L;      //SDA low level
  }
  else
  {
    I2C_SET_SDA_H;      //SDA high level
  }
  I2C_SET_SCL_H;        //SCL high level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
  I2C_SET_SCL_L;        //SCL low level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
}

/*
**I2C receiving response signal
*/
uint8 I2C_GPIO_Recv_Ack(void)
{
  uint8 Recv=0;
  
  I2C_DDR_IN_SDA;       //SDA input
  I2C_SET_SCL_H;        //SCL high level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
  Recv = I2C_GET_SDA_IN;//Read SDA signal
  I2C_SET_SCL_L;        //SCL low level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
  return Recv;
}

/*
**I2C sends one byte of data
*/
uint8 I2C_GPIO_Send_Byte(uint8 I2C_Data)
{
  uint8 i=0;
  uint8 Ack=0;
  
  I2C_DDR_OUT_SDA;      //SDA output
  for(i=0;i<8;i++)
  {
    if((I2C_Data<<i)&0x80)
    {
      I2C_SET_SDA_H;    //SDA high level
    }
    else
    {
      I2C_SET_SDA_L;    //SDA low level
    }
    I2C_DELAY_TIME;     //I2C delay of 10 Nop
    I2C_SET_SCL_H;      //SCL high level
    I2C_DELAY_TIME;     //I2C delay of 10 Nop
    I2C_SET_SCL_L;      //SCL low level
    I2C_DELAY_TIME;     //I2C delay of 10 Nop
  }
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
  I2C_SET_SDA_H;        //SDA high level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
  I2C_SET_SCL_H;        //SCL high level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
  I2C_DDR_IN_SDA;       //SDA input
  if(I2C_GET_SDA_IN == 1)
  {
    Ack = 0;
  }
  else
  {
    Ack = 1;
  }
  I2C_SET_SCL_L;        //SCL low level
  I2C_DELAY_TIME;       //I2C delay of 10 Nop
  return Ack;
}

/*
**I2C Receive one byte of data
*/
uint8 I2C_GPIO_Recv_Byte(void)
{
  uint8 i=0;
  uint8 Data=0;
  
  I2C_DDR_OUT_SDA;      //SDA output
  I2C_SET_SDA_H;        //SDA high level
  I2C_DDR_IN_SDA;       //SDA input
  for(i=0;i<8;i++)
  {
    Data<<=1;
    I2C_SET_SCL_H;      //SCL high level
    I2C_DELAY_TIME;     //I2C delay of 10 Nop
    Data|=I2C_GET_SDA_IN;
    I2C_SET_SCL_L;      //SCL low level
    I2C_DELAY_TIME;     //I2C delay of 10 Nop
  }
  return Data;
}

/*
**I2C Write register
*/
void I2C_GPIO_Write_Reg(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr,uint8 I2C_Data)
{
  I2C_GPIO_Start();
  I2C_GPIO_Send_Byte(I2C_Div_Adr);
  I2C_GPIO_Send_Byte(I2C_Reg_Adr);
  I2C_GPIO_Send_Byte(I2C_Data);
  I2C_GPIO_Stop();
}

/*
**I2C Read Register
*/
//Read a byte
uint8 I2C_GPIO_Read_Reg_Byte(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr)
{
  uint8 I2C_Data=0;
  
  I2C_GPIO_Start();
  I2C_GPIO_Send_Byte(I2C_Div_Adr);
  I2C_GPIO_Send_Byte(I2C_Reg_Adr);
  I2C_GPIO_Start();
  I2C_GPIO_Send_Byte(I2C_Div_Adr+1);
  I2C_Data = I2C_GPIO_Recv_Byte();
  I2C_GPIO_Send_Ack(1);
  I2C_GPIO_Stop();
  
  return I2C_Data;
}

//Read a byte
int I2C_GPIO_Read_Reg_Word(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr)
{
  uint8 I2C_Reg_H=0;
  uint8 I2C_Reg_L=0;
  
  I2C_Reg_H = I2C_GPIO_Read_Reg_Byte(I2C_Div_Adr,I2C_Reg_Adr);
  I2C_Reg_L = I2C_GPIO_Read_Reg_Byte(I2C_Div_Adr,I2C_Reg_Adr+1);
  
  return ((I2C_Reg_H<<8)+I2C_Reg_L);
}

/*
**MPU6050 X-axis acceleration value
*/
int MPU_Get_Acc_X(void)
{
  int Temp=0;
  Temp = I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050,ACCEL_XOUT_H);
  return Temp;
}

/*
**MPU6050 Y-axis acceleration value
*/
int MPU_Get_Acc_Y(void)
{
  int Temp=0;
  Temp = I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050,ACCEL_YOUT_H);
  return Temp;
}

/*
**MPU6050 Z-axis acceleration value
*/
int MPU_Get_Acc_Z(void)
{
  int Temp=0;
  Temp = I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050,ACCEL_ZOUT_H);
  return Temp;
}

/*
**MPU6050 X-axis angular velocity value
*/
int MPU_Get_Gyro_X(void)
{
  int Temp=0;
  Temp = I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050,GYRO_XOUT_H);
  return Temp;
}

/*
**MPU6050 Y-axis angular velocity value
*/
int MPU_Get_Gyro_Y(void)
{
  int Temp=0;
  Temp = I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050,GYRO_YOUT_H);
  return Temp;
}

/*
**MPU6050 Z-axis angular velocity value
*/
int MPU_Get_Gyro_Z(void)
{
  int Temp=0;
  Temp = I2C_GPIO_Read_Reg_Word(I2C_ADR_MPU6050,GYRO_ZOUT_H);
  return Temp;
}

/*
**MPU6050 initialization
*/
void MPU6050_Init(void)
{
  I2C_PORT_INIT_SCL;
  I2C_PORT_INIT_SDA;
  I2C_GPIO_Write_Reg(I2C_ADR_MPU6050,PWR_MGMT_1,0x00);
  I2C_GPIO_Write_Reg(I2C_ADR_MPU6050,SMPLRT_DIV,0x00);
  I2C_GPIO_Write_Reg(I2C_ADR_MPU6050,CONFIG,0x00);
  I2C_GPIO_Write_Reg(I2C_ADR_MPU6050,ACCEL_CONFIG,0x09);
  I2C_GPIO_Write_Reg(I2C_ADR_MPU6050,GYRO_CONFIG,0x18);
}
