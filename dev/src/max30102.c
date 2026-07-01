/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       max30102.c
 * @brief      MAX30102 heart rate / SpO2 sensor GPIO bit-bang I2C driver
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "max30102.h"
#include "delay.h"
#include "gpio.h"
#include "i2c.h"

void MAX30102_I2C_GPIO_Start(void)
{
  MAX30102_I2C_DDR_OUT_SDA;
  MAX30102_I2C_SET_SDA_H;
  MAX30102_I2C_SET_SCL_H;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_SET_SDA_L;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_SET_SCL_L;
}

void MAX30102_I2C_GPIO_Stop(void)
{
  MAX30102_I2C_DDR_OUT_SDA;
  MAX30102_I2C_SET_SDA_L;
  MAX30102_I2C_SET_SCL_H;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_SET_SDA_H;
  MAX30102_I2C_DELAY_TIME;
}

// Send ACK (Ack=0) or NACK (Ack=1)
void MAX30102_I2C_GPIO_Send_Ack(int Ack)
{
  MAX30102_I2C_DDR_OUT_SDA;
  if (Ack == 0) { MAX30102_I2C_SET_SDA_L; }
  else          { MAX30102_I2C_SET_SDA_H; }
  MAX30102_I2C_SET_SCL_H;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_SET_SCL_L;
  MAX30102_I2C_DELAY_TIME;
}

uint8 MAX30102_I2C_GPIO_Recv_Ack(void)
{
  uint8 Recv = 0;
  MAX30102_I2C_DDR_IN_SDA;
  MAX30102_I2C_SET_SCL_H;
  MAX30102_I2C_DELAY_TIME;
  Recv = MAX30102_I2C_GET_SDA_IN;
  MAX30102_I2C_SET_SCL_L;
  MAX30102_I2C_DELAY_TIME;
  return Recv;
}

uint8 MAX30102_I2C_GPIO_Send_Byte(uint8 I2C_Data)
{
  uint8 i   = 0;
  uint8 Ack = 0;

  MAX30102_I2C_DDR_OUT_SDA;
  for (i = 0; i < 8; i++)
  {
    if ((I2C_Data << i) & 0x80) { MAX30102_I2C_SET_SDA_H; }
    else                        { MAX30102_I2C_SET_SDA_L; }
    MAX30102_I2C_DELAY_TIME;
    MAX30102_I2C_SET_SCL_H;
    MAX30102_I2C_DELAY_TIME;
    MAX30102_I2C_SET_SCL_L;
    MAX30102_I2C_DELAY_TIME;
  }
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_SET_SDA_H;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_SET_SCL_H;
  MAX30102_I2C_DELAY_TIME;
  MAX30102_I2C_DDR_IN_SDA;
  Ack = (MAX30102_I2C_GET_SDA_IN == 0) ? 1 : 0;
  MAX30102_I2C_SET_SCL_L;
  MAX30102_I2C_DELAY_TIME;
  return Ack;
}

uint8 MAX30102_I2C_GPIO_Recv_Byte(void)
{
  uint8 i    = 0;
  uint8 Data = 0;

  MAX30102_I2C_DDR_OUT_SDA;
  MAX30102_I2C_SET_SDA_H;
  MAX30102_I2C_DDR_IN_SDA;
  for (i = 0; i < 8; i++)
  {
    Data <<= 1;
    MAX30102_I2C_SET_SCL_H;
    MAX30102_I2C_DELAY_TIME;
    Data |= MAX30102_I2C_GET_SDA_IN;
    MAX30102_I2C_SET_SCL_L;
    MAX30102_I2C_DELAY_TIME;
  }
  return Data;
}

// Write one byte to a register. Returns 1 on success, 0 if NACKed.
uint8 MAX30102_I2C_GPIO_Write_Reg(uint8 I2C_Div_Adr, uint8 I2C_Reg_Adr, uint8 I2C_Data)
{
  MAX30102_I2C_GPIO_Start();
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr))  { MAX30102_I2C_GPIO_Stop(); return 0; }
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Reg_Adr))  { MAX30102_I2C_GPIO_Stop(); return 0; }
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Data))      { MAX30102_I2C_GPIO_Stop(); return 0; }
  MAX30102_I2C_GPIO_Stop();
  return 1;
}

// Read one byte from a register. Returns 0xFF on NACK (bus error).
uint8 MAX30102_I2C_GPIO_Read_Reg_Byte(uint8 I2C_Div_Adr, uint8 I2C_Reg_Adr)
{
  uint8 I2C_Data = 0;

  MAX30102_I2C_GPIO_Start();
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr))      { MAX30102_I2C_GPIO_Stop(); return 0xFF; }
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Reg_Adr))      { MAX30102_I2C_GPIO_Stop(); return 0xFF; }
  MAX30102_I2C_GPIO_Start();
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr + 1))  { MAX30102_I2C_GPIO_Stop(); return 0xFF; }
  I2C_Data = MAX30102_I2C_GPIO_Recv_Byte();
  MAX30102_I2C_GPIO_Send_Ack(1);
  MAX30102_I2C_GPIO_Stop();

  return I2C_Data;
}

// Read a 16-bit register atomically in a SINGLE I2C transaction.
// Uses repeated START to read the high and low bytes back-to-back without
// releasing the bus, so the two bytes are guaranteed to come from the same
// register snapshot (no tearing between independent transactions).
// Returns the 16-bit value, or 0 on NACK (bus error).
int MAX30102_I2C_GPIO_Read_Reg_Word(uint8 I2C_Div_Adr, uint8 I2C_Reg_Adr)
{
  uint8 I2C_Reg_H = 0;
  uint8 I2C_Reg_L = 0;

  MAX30102_I2C_GPIO_Start();
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr))      { MAX30102_I2C_GPIO_Stop(); return 0; }
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Reg_Adr))      { MAX30102_I2C_GPIO_Stop(); return 0; }
  MAX30102_I2C_GPIO_Start();                           // Repeated START for read
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr + 1))  { MAX30102_I2C_GPIO_Stop(); return 0; }
  I2C_Reg_H = MAX30102_I2C_GPIO_Recv_Byte();
  MAX30102_I2C_GPIO_Send_Ack(0);                       // ACK: more bytes to read
  I2C_Reg_L = MAX30102_I2C_GPIO_Recv_Byte();
  MAX30102_I2C_GPIO_Send_Ack(1);                       // NACK: last byte
  MAX30102_I2C_GPIO_Stop();

  return ((I2C_Reg_H << 8) | I2C_Reg_L);
}

// Read 6 consecutive bytes in a single I2C transaction (used for FIFO read)
// Returns 1 on success, 0 on NACK.
uint8 MAX30102_I2C_GPIO_Read_Reg_Six_Byte(uint8 I2C_Div_Adr, uint8 I2C_Reg_Adr, uint8 *data)
{
  MAX30102_I2C_GPIO_Start();
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr))      { MAX30102_I2C_GPIO_Stop(); return 0; }
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Reg_Adr))      { MAX30102_I2C_GPIO_Stop(); return 0; }
  MAX30102_I2C_GPIO_Start();
  if (!MAX30102_I2C_GPIO_Send_Byte(I2C_Div_Adr + 1))  { MAX30102_I2C_GPIO_Stop(); return 0; }
  for (uint8 i = 0; i < 6; i++)
  {
    data[i] = MAX30102_I2C_GPIO_Recv_Byte();
    MAX30102_I2C_GPIO_Send_Ack(i < 5 ? 0 : 1); // ACK all but last byte
  }
  MAX30102_I2C_GPIO_Stop();
  return 1;
}

/*
 * MAX30102 initialization
 * Mode    : SpO2 (RED + IR)
 * Sample  : 100Hz, 18-bit ADC, 411μs pulse width
 * LED PA  : 0x24 (~7.2mA)
 */
void MAX30102_Init(void)
{
  MAX30102_I2C_PORT_INIT_SCL;
  MAX30102_I2C_PORT_INIT_SDA;
  MAX30102_Reset();
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_INTR_ENABLE_1, 0xC0); // PPG_RDY + ALC_OVF interrupts
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_INTR_ENABLE_2, 0x00);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_FIFO_WR_PTR,   0x00); // Clear FIFO pointers
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_OVF_COUNTER,   0x00);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_FIFO_RD_PTR,   0x00);
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_FIFO_CONFIG,   0x0F); // No averaging, FIFO rollover on
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_MODE_CONFIG,   0x03); // SpO2 mode
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_SPO2_CONFIG,   0x27); // 100Hz, 18-bit, 411μs
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_LED1_PA,       0x24); // RED LED ~7.2mA
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_LED2_PA,       0x24); // IR  LED ~7.2mA
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_PILOT_PA,      0x7F); // Proximity pilot LED
}

// Software reset; waits 2ms for chip to complete reset (datasheet: ≥1ms)
void MAX30102_Reset(void)
{
  MAX30102_I2C_GPIO_Write_Reg(MAX30102_DEVICE_ADDR, REG_MODE_CONFIG, 0x40);
  DELAY_ms(2);
}

// Read one RED + IR sample from FIFO (6 bytes, 18-bit values)
void MAX30102_ReadFIFO(uint32 *red, uint32 *ir)
{
  uint8 data[6];
  MAX30102_I2C_GPIO_Read_Reg_Six_Byte(MAX30102_DEVICE_ADDR, REG_FIFO_DATA, data);
  *red = (((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | data[2]) & 0x03FFFF;
  *ir  = (((uint32_t)data[3] << 16) | ((uint32_t)data[4] << 8) | data[5]) & 0x03FFFF;
}
