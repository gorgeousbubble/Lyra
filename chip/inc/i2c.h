/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       i2c.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __I2C_H__
#define __I2C_H__

#include "common.h"

/*
**I2C module enumeration
*/
typedef enum
{
  I2C_I2C0,
  I2C_I2C1,
  I2C_I2C2,
  I2C_I2C_MAX,
} I2C_I2Cn;

/*
**I2 Cmodule enumeration
*/
typedef enum
{
  MWSR = 0x00, // Host read mode
  MRSW = 0x01, // Host write mode
} I2C_Mode;

/*
**I2C Macro
*/
// Start signal
#define I2C_Start(I2Cn) I2C_C1_REG(I2CN[I2Cn]) |= (I2C_C1_MST_MASK | I2C_C1_TX_MASK)
// Stop signal
#define I2C_Stop(I2Cn) I2C_C1_REG(I2CN[I2Cn]) &= ~(I2C_C1_MST_MASK | I2C_C1_TX_MASK)
// Repetition priming
#define I2C_ReStart(I2Cn) I2C_C1_REG(I2CN[I2Cn]) |= I2C_C1_RSTA_MASK
// Receiving mode response
#define I2C_EnterRx(I2Cn) I2C_C1_REG(I2CN[I2Cn]) &= ~(I2C_C1_TX_MASK | I2C_C1_TXAK_MASK)
// Receiving mode does not respond
#define I2C_PutInRx(I2Cn)                    \
  I2C_C1_REG(I2CN[I2Cn]) &= ~I2C_C1_TX_MASK; \
  I2C_C1_REG(I2CN[I2Cn]) |= I2C_C1_TXAK_MASK
// Disable response
#define I2C_DisableAck(I2Cn) I2C_C1_REG(I2CN[I2Cn]) |= I2C_C1_TXAK_MASK;
// Maximum spin iterations while waiting for the I2C interrupt flag (IICIF).
// Bounds the busy-wait so a stuck/undriven bus (missing pull-ups, a wedged
// slave holding SCL/SDA, or a missing 24C02 EEPROM) cannot hang the CPU
// forever and trip the watchdog into a reboot loop. At 120 MHz this spin is
// a few milliseconds -- far longer than a normal byte transfer (~25 us @
// 400 kHz), so it never expires during healthy communication.
#define I2C_WAIT_TIMEOUT 100000UL

// Waiting for I2C_S (bounded).  On timeout we still clear IICIF and fall
// through so the caller issues STOP and returns rather than dead-locking.
#define I2C_Wait(I2Cn)                                        \
  do                                                          \
  {                                                           \
    uint32 _i2c_to = I2C_WAIT_TIMEOUT;                        \
    while ((I2C_S_REG(I2CN[I2Cn]) & I2C_S_IICIF_MASK) == 0)   \
    {                                                         \
      if (--_i2c_to == 0)                                     \
        break;                                                \
    }                                                         \
    I2C_S_REG(I2CN[I2Cn]) |= I2C_S_IICIF_MASK;                \
  } while (0)
// I2C writes one byte
#define I2C_Write_Byte(I2Cn, I2C_Data) \
  I2C_D_REG(I2CN[I2Cn]) = I2C_Data;    \
  I2C_Wait(I2Cn)

/*
**variate declaration
*/
extern I2C_MemMapPtr I2CN[I2C_I2C_MAX];

/*
**function declaration
*/
extern uint32 I2C_Init(I2C_I2Cn I2C_I2Cx, uint32 I2C_Baud);
extern void I2C_Delay(uint16 n);
extern uint8 I2C_Read_Reg(I2C_I2Cn I2C_I2Cx, uint8 I2C_SlaveID, uint8 I2C_Reg);
extern void I2C_Write_Reg(I2C_I2Cn I2C_I2Cx, uint8 I2C_SlaveID, uint8 I2C_Reg, uint8 I2C_Data);

#endif