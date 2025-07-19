/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       max30102.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MAX30102_H__
#define __MAX30102_H__

#include "common.h"

/*
**MAX30102 Internal register macro
*/
//MAX30102 Device Address
#define MAX30102_DEVICE_ADDR    0xAE

//register addresses
#define REG_INTR_STATUS_1       0x00
#define REG_INTR_STATUS_2       0x01
#define REG_INTR_ENABLE_1       0x02
#define REG_INTR_ENABLE_2       0x03
#define REG_FIFO_WR_PTR         0x04
#define REG_OVF_COUNTER         0x05
#define REG_FIFO_RD_PTR         0x06
#define REG_FIFO_DATA           0x07
#define REG_FIFO_CONFIG         0x08
#define REG_MODE_CONFIG         0x09
#define REG_SPO2_CONFIG         0x0A
#define REG_LED1_PA             0x0C
#define REG_LED2_PA             0x0D
#define REG_PILOT_PA            0x10
#define REG_MULTI_LED_CTRL1     0x11
#define REG_MULTI_LED_CTRL2     0x12
#define REG_TEMP_INTR           0x1F
#define REG_TEMP_FRAC           0x20
#define REG_TEMP_CONFIG         0x21
#define REG_PROX_INT_THRESH     0x30
#define REG_REV_ID              0xFE
#define REG_PART_ID             0xFF

//I2C Port Macro
//I2C SCL port initialization
#ifndef MAX30102_I2C_PORT_INIT_SCL
#define MAX30102_I2C_PORT_INIT_SCL       GPIO_Init(PTE10,GPO,0)
#endif
//I2C SDA port initialization
#ifndef MAX30102_I2C_PORT_INIT_SDA
#define MAX30102_I2C_PORT_INIT_SDA       GPIO_Init(PTE11,GPO,0)
#endif
//I2C SCL port output
#ifndef MAX30102_I2C_DDR_OUT_SCL
#define MAX30102_I2C_DDR_OUT_SCL         GPIO_DDRX(PTE10,1)
#endif
//I2C SDA port output
#ifndef MAX30102_I2C_DDR_OUT_SDA
#define MAX30102_I2C_DDR_OUT_SDA         GPIO_DDRX(PTE11,1)
#endif
//I2C SDA port input
#ifndef MAX30102_I2C_DDR_IN_SDA
#define MAX30102_I2C_DDR_IN_SDA          GPIO_DDRX(PTE11,0)
#endif
//I2C SCL port high level
#ifndef MAX30102_I2C_SET_SCL_H
#define MAX30102_I2C_SET_SCL_H           GPIO_SET(PTE10,1)
#endif
//I2C SCL port low level
#ifndef MAX30102_I2C_SET_SCL_L
#define MAX30102_I2C_SET_SCL_L           GPIO_SET(PTE10,0)
#endif
//I2C SDA port high level
#ifndef MAX30102_I2C_SET_SDA_H
#define MAX30102_I2C_SET_SDA_H           GPIO_SET(PTE11,1)
#endif
//I2C SDA port low level
#ifndef MAX30102_I2C_SET_SDA_L
#define MAX30102_I2C_SET_SDA_L           GPIO_SET(PTE11,0)
#endif
//I2C SDA port input signal
#ifndef MAX30102_I2C_GET_SDA_IN
#define MAX30102_I2C_GET_SDA_IN          GPIO_GET(PTE11)
#endif

#define MAX30102_I2C_DELAY_TIME          I2C_Delay(10)

/*
**function declaration
*/
extern void MAX30102_I2C_GPIO_Start(void);
extern void MAX30102_I2C_GPIO_Stop(void);
extern void MAX30102_I2C_GPIO_Send_Ack(int Ack);
extern uint8 MAX30102_I2C_GPIO_Recv_Ack(void);
extern uint8 MAX30102_I2C_GPIO_Send_Byte(uint8 I2C_Data);
extern uint8 MAX30102_I2C_GPIO_Recv_Byte(void);
extern void MAX30102_I2C_GPIO_Write_Reg(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr,uint8 I2C_Data);
extern uint8 MAX30102_I2C_GPIO_Read_Reg_Byte(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr);
extern int MAX30102_I2C_GPIO_Read_Reg_Word(uint8 I2C_Div_Adr,uint8 I2C_Reg_Adr);
extern void MAX30102_Init(void);
extern void MAX30102_Reset(void);
extern void MAX30102_ReadFIFO(uint32* red, uint32* ir);

#endif