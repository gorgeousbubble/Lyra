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
 * MPU_Get_All — burst-read all 6 motion axes in a SINGLE I2C transaction.
 *
 * The MPU6050 output registers are contiguous:
 *   0x3B ACCEL_XOUT_H  0x3C ACCEL_XOUT_L
 *   0x3D ACCEL_YOUT_H  0x3E ACCEL_YOUT_L
 *   0x3F ACCEL_ZOUT_H  0x40 ACCEL_ZOUT_L
 *   0x41 TEMP_OUT_H    0x42 TEMP_OUT_L
 *   0x43 GYRO_XOUT_H   0x44 GYRO_XOUT_L
 *   0x45 GYRO_YOUT_H   0x46 GYRO_YOUT_L
 *   0x47 GYRO_ZOUT_H   0x48 GYRO_ZOUT_L
 *
 * Reading all 14 bytes with a single repeated-START transaction replaces
 * the previous 6 separate word reads (6× START/STOP + 6× register-address
 * write overhead).  On the bit-bang bus this cuts the per-sample I2C time
 * by roughly 75-80% and — more importantly — guarantees all six axes come
 * from the SAME sensor sample (no tearing between independent reads).
 *
 * Any output pointer may be NULL if that axis is not needed; the
 * temperature bytes (index 6-7) are always skipped.
 * Returns 1 on success, 0 on NACK (bus error) — outputs left unchanged on error.
 */
uint8 MPU_Get_All(int16 *ax, int16 *ay, int16 *az,
                  int16 *gx, int16 *gy, int16 *gz)
{
  uint8 buf[14];
  uint8 i;

  I2C_GPIO_Start();
  if (!I2C_GPIO_Send_Byte(I2C_ADR_MPU6050))     { I2C_GPIO_Stop(); return 0; }
  if (!I2C_GPIO_Send_Byte(ACCEL_XOUT_H))        { I2C_GPIO_Stop(); return 0; }
  I2C_GPIO_Start();                              // Repeated START for read
  if (!I2C_GPIO_Send_Byte(I2C_ADR_MPU6050 + 1)) { I2C_GPIO_Stop(); return 0; }

  for (i = 0; i < 14; i++)
  {
    buf[i] = I2C_GPIO_Recv_Byte();
    I2C_GPIO_Send_Ack(i < 13 ? 0 : 1);          // ACK all but last byte
  }
  I2C_GPIO_Stop();

  if (ax) *ax = (int16)((buf[0]  << 8) | buf[1]);
  if (ay) *ay = (int16)((buf[2]  << 8) | buf[3]);
  if (az) *az = (int16)((buf[4]  << 8) | buf[5]);
  /* buf[6..7] = temperature — skipped */
  if (gx) *gx = (int16)((buf[8]  << 8) | buf[9]);
  if (gy) *gy = (int16)((buf[10] << 8) | buf[11]);
  if (gz) *gz = (int16)((buf[12] << 8) | buf[13]);

  return 1;
}

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

/*
 * ---------------------------------------------------------------------------
 * Hot-plug support
 * ---------------------------------------------------------------------------
 * If the sensor's wiring is momentarily disconnected (e.g. a loose dupont
 * jumper), two things go wrong:
 *   1. The transfer in progress is aborted; the slave may be left holding SDA
 *      low mid-byte, leaving the bus stuck for every subsequent transaction.
 *   2. If VCC/GND were interrupted, the MPU6050 power-cycles and comes back in
 *      its default SLEEP state, so it produces no data until it is woken and
 *      reconfigured again.
 *
 * MPU6050_Bus_Recovery / MPU6050_Present / MPU6050_Recover let the main loop
 * detect the loss (MPU_Get_All returns 0) and transparently bring the sensor
 * back once the connection is restored.
 */

/*
 * Free a bus that a slave is holding: clock SCL up to 9 times with SDA
 * released so the slave can finish/abort its current byte, then issue a STOP.
 */
void MPU6050_Bus_Recovery(void)
{
  int i;

  I2C_DDR_OUT_SCL;   // we always drive SCL
  I2C_DDR_IN_SDA;    // release SDA so the slave (or pull-up) drives it

  for (i = 0; i < 9; i++)
  {
    I2C_SET_SCL_H;
    I2C_DELAY_TIME;
    if (I2C_GET_SDA_IN)   // SDA released high => bus is free again
    {
      I2C_SET_SCL_L;
      I2C_DELAY_TIME;
      break;
    }
    I2C_SET_SCL_L;
    I2C_DELAY_TIME;
  }

  I2C_GPIO_Stop();   // leave the bus in a clean idle state
}

/*
 * Return 1 if the MPU6050 answers on the bus (WHO_AM_I == 0x68), else 0.
 * A disconnected or unpowered sensor NACKs, so the read returns 0xFF.
 */
uint8 MPU6050_Present(void)
{
  return (I2C_GPIO_Read_Reg_Byte(I2C_ADR_MPU6050, WHO_AM_I) == 0x68) ? 1 : 0;
}

/*
 * Attempt to recover a lost MPU6050 after a hot-unplug/replug:
 *   - unstick the bus,
 *   - confirm the sensor is present again,
 *   - re-initialize it (wake from sleep + reconfigure).
 * Returns 1 if the sensor is back and configured, 0 otherwise.
 * Cheap enough (a few hundred us of bit-bang) to call from the 10 ms slot;
 * while the sensor is absent WHO_AM_I NACKs quickly and it returns 0.
 */
uint8 MPU6050_Recover(void)
{
  MPU6050_Bus_Recovery();

  if (!MPU6050_Present())
    return 0;              // still absent or bus not yet restored

  MPU6050_Init();          // wake from sleep + reapply configuration
  return MPU6050_Present(); // confirm it really came back
}
