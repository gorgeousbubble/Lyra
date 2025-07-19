/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       max30102.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "max30102.h"
#include "gpio.h"
#include "i2c.h"

/*
**I2C start signal
*/
void MAX30102_I2C_GPIO_Start(void)
{
  MAX30102_I2C_DDR_OUT_SDA;      //SDA output
  MAX30102_I2C_SET_SDA_H;        //SDA high level
  MAX30102_I2C_SET_SCL_H;        //SCL high level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  MAX30102_I2C_SET_SDA_L;        //SDA low level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  MAX30102_I2C_SET_SCL_L;        //SCL low level
}

/*
**I2C stop signal
*/
void MAX30102_I2C_GPIO_Stop(void)
{
  MAX30102_I2C_DDR_OUT_SDA;      //SDA output
  MAX30102_I2C_SET_SDA_L;        //SDA low level
  MAX30102_I2C_SET_SCL_H;        //SCL high level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  MAX30102_I2C_SET_SDA_H;        //SDA high level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
}

/*
**I2C sends response signal
*/
void MAX30102_I2C_GPIO_Send_Ack(int Ack)
{
  MAX30102_I2C_DDR_OUT_SDA;      //SDA output
  if(Ack == 0)
  {
    MAX30102_I2C_SET_SDA_L;      //SDA low level
  }
  else
  {
    MAX30102_I2C_SET_SDA_H;      //SDA high level
  }
  MAX30102_I2C_SET_SCL_H;        //SCL high level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  MAX30102_I2C_SET_SCL_L;        //SCL low level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
}

/*
**I2C receiving response signal
*/
uint8 MAX30102_I2C_GPIO_Recv_Ack(void)
{
  uint8 Recv=0;
  
  MAX30102_I2C_DDR_IN_SDA;       //SDA input
  MAX30102_I2C_SET_SCL_H;        //SCL high level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  Recv = MAX30102_I2C_GET_SDA_IN;//Read SDA signal
  MAX30102_I2C_SET_SCL_L;        //SCL low level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  return Recv;
}

/*
**I2C sends one byte of data
*/
uint8 MAX30102_I2C_GPIO_Send_Byte(uint8 I2C_Data)
{
  uint8 i=0;
  uint8 Ack=0;
  
  MAX30102_I2C_DDR_OUT_SDA;      //SDA output
  for(i=0;i<8;i++)
  {
    if((I2C_Data<<i)&0x80)
    {
      MAX30102_I2C_SET_SDA_H;    //SDA high level
    }
    else
    {
      MAX30102_I2C_SET_SDA_L;    //SDA low level
    }
    MAX30102_I2C_DELAY_TIME;     //I2C delay of 10 Nop
    MAX30102_I2C_SET_SCL_H;      //SCL high level
    MAX30102_I2C_DELAY_TIME;     //I2C delay of 10 Nop
    MAX30102_I2C_SET_SCL_L;      //SCL low level
    MAX30102_I2C_DELAY_TIME;     //I2C delay of 10 Nop
  }
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  MAX30102_I2C_SET_SDA_H;        //SDA high level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  MAX30102_I2C_SET_SCL_H;        //SCL high level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  MAX30102_I2C_DDR_IN_SDA;       //SDA input
  if(MAX30102_I2C_GET_SDA_IN == 1)
  {
    Ack = 0;
  }
  else
  {
    Ack = 1;
  }
  MAX30102_I2C_SET_SCL_L;        //SCL low level
  MAX30102_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  return Ack;
}

/*
**I2C Receive one byte of data
*/
uint8 MAX30102_I2C_GPIO_Recv_Byte(void)
{
  uint8 i=0;
  uint8 Data=0;
  
  MAX30102_I2C_DDR_OUT_SDA;      //SDA output
  MAX30102_I2C_SET_SDA_H;        //SDA high level
  MAX30102_I2C_DDR_IN_SDA;       //SDA input
  for(i=0;i<8;i++)
  {
    Data<<=1;
    MAX30102_I2C_SET_SCL_H;      //SCL high level
    MAX30102_I2C_DELAY_TIME;     //I2C delay of 10 Nop
    Data|=MAX30102_I2C_GET_SDA_IN;
    MAX30102_I2C_SET_SCL_L;      //SCL low level
    MAX30102_I2C_DELAY_TIME;     //I2C delay of 10 Nop
  }
  return Data;
}

/*
**I2C Write register
*/
void MAX30102_I2C_GPIO_Write_Reg(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr,uint8 I2C_Data)
{
  MAX30102_I2C_GPIO_Start();
  MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr);
  MAX30102_I2C_GPIO_Send_Byte(I2C_Reg_Adr);
  MAX30102_I2C_GPIO_Send_Byte(I2C_Data);
  MAX30102_I2C_GPIO_Stop();
}

/*
**I2C Read Register
*/
//Read a byte
uint8 MAX30102_I2C_GPIO_Read_Reg_Byte(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr)
{
  uint8 I2C_Data=0;
  
  MAX30102_I2C_GPIO_Start();
  MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr);
  MAX30102_I2C_GPIO_Send_Byte(I2C_Reg_Adr);
  MAX30102_I2C_GPIO_Start();
  MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr+1);
  I2C_Data = MAX30102_I2C_GPIO_Recv_Byte();
  MAX30102_I2C_GPIO_Send_Ack(1);
  MAX30102_I2C_GPIO_Stop();
  
  return I2C_Data;
}

//Read a word
int MAX30102_I2C_GPIO_Read_Reg_Word(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr)
{
  uint8 I2C_Reg_H=0;
  uint8 I2C_Reg_L=0;
  
  I2C_Reg_H = MAX30102_I2C_GPIO_Read_Reg_Byte(I2C_Div_Adr,I2C_Reg_Adr);
  I2C_Reg_L = MAX30102_I2C_GPIO_Read_Reg_Byte(I2C_Div_Adr,I2C_Reg_Adr+1);
  
  return ((I2C_Reg_H<<8)+I2C_Reg_L);
}

//Read six bytes
void MAX30102_I2C_GPIO_Read_Reg_Six_Byte(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr,uint8* data)
{
  MAX30102_I2C_GPIO_Start();
  MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr);
  MAX30102_I2C_GPIO_Send_Byte(I2C_Reg_Adr);
  MAX30102_I2C_GPIO_Start();
  MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr+1);
  for (uint8 i = 0; i < 6; i++)
  {
    data[i] = MAX30102_I2C_GPIO_Recv_Byte();
    if (i < 5) // Send ACK for all but the last byte
    {
      MAX30102_I2C_GPIO_Send_Ack(0);
    }
    else // Send NACK for the last byte
    {
      MAX30102_I2C_GPIO_Send_Ack(1);
    }
  }
  MAX30102_I2C_GPIO_Stop();
}

/*
**MAX30102 initialization
*/
void MAX30102_Init(void)
{
  MAX30102_I2C_PORT_INIT_SCL;
  MAX30102_I2C_PORT_INIT_SDA;
  MAX30102_Reset(); //Reset the MAX30102 sensor
  // Configure the sensor
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_INTR_ENABLE_1,0xc0);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_INTR_ENABLE_2,0x00);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_FIFO_WR_PTR,0x00);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_OVF_COUNTER,0x00);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_FIFO_RD_PTR,0x00);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_FIFO_CONFIG,0x0F);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_MODE_CONFIG,0x03);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_SPO2_CONFIG,0x27);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_LED1_PA,0x24);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_LED2_PA,0x24);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR,REG_PILOT_PA,0x7F);  
}

/*
**MAX30102 reset
*/
void MAX30102_Reset(void) 
{
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_MODE_CONFIG, 0x40);
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_DELAY_TIME;
}

/*
**MAX30102 read fifo
*/
void MAX30102_ReadFIFO(uint32* red, uint32* ir)
{
    uint8 data[6];
    // read 6 bytes from FIFO
    /*for (int i = 0; i < 6; i++) {
        data[i] = MAX30102_I2C_GPIO_Read_Reg_Byte(MAX30102_DEVICE_ADDR, REG_FIFO_DATA + i);
    }*/
    MAX30102_I2C_GPIO_Read_Reg_Six_Byte(MAX30102_DEVICE_ADDR, REG_FIFO_DATA, data);
    // combine the bytes into 24-bit values
    *red = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | data[2];
    *ir = ((uint32_t)data[3] << 16) | ((uint32_t)data[4] << 8) | data[5];
    *red &= 0x03FFFF;
    *ir  &= 0x03FFFF;
}
