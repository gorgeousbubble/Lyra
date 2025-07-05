/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_i2c.c
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "dwt.h"
#include "i2c.h"
#include "maps_dock_i2c.h"

/*
 *  @brief      I2C writes one byte to the specified address
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_I2C_Write_Reg();
 */
static inline void MAPS_Dock_I2C_Write_Reg(I2C_I2Cn I2C_I2Cx,uint8 I2C_SlaveID,uint8 I2C_Reg,uint8 I2C_Data)
{
  I2C_Start(I2C_I2Cx);//Send start signal
  I2C_Write_Byte(I2C_I2Cx,(I2C_SlaveID << 1) | MWSR);
  I2C_Write_Byte(I2C_I2Cx,I2C_Reg);
  I2C_Write_Byte(I2C_I2Cx,I2C_Data);
  I2C_Stop(I2C_I2Cx);
  MAPS_Dock_I2C_Delay(10);
}

/*
 *  @brief      I2C reads one byte from the specified address
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_I2C_Read_Reg();
 */
static inline uint8 MAPS_Dock_I2C_Read_Reg(I2C_I2Cn I2C_I2Cx,uint8 I2C_SlaveID,uint8 I2C_Reg)
{
  uint8 I2C_Result=0;
  
  I2C_Start(I2C_I2Cx);//Send start signal
  I2C_Write_Byte(I2C_I2Cx,(I2C_SlaveID << 1) | MWSR);
  I2C_Write_Byte(I2C_I2Cx,I2C_Reg);
  I2C_ReStart(I2C_I2Cx);
  I2C_Write_Byte(I2C_I2Cx,(I2C_SlaveID << 1) | MRSW);
  I2C_PutInRx(I2C_I2Cx);
  I2C_Result = I2C_D_REG(I2CN[I2C_I2Cx]);
  I2C_Wait(I2C_I2Cx);
  I2C_Stop(I2C_I2Cx);
  I2C_Result = I2C_D_REG(I2CN[I2C_I2Cx]);
  MAPS_Dock_I2C_Delay(10);
  
  return I2C_Result;
}

/*
 *  @brief      Write one byte to the specified address 24C02
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_24C02_Write_Byte();
 */
void MAPS_Dock_24C02_Write_Byte(uint8 Addr,uint8 Data)
{
  MAPS_Dock_I2C_Write_Reg(I2C_I2C0,I2C_ADR_24C02,Addr,Data);
  MAPS_Dock_I2C_Delay(5000);//Writing E2PROM requires a delay of 5Ms
}

/*
 *  @brief      24C02 reads one byte from the specified address
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_24C02_Read_Byte();
 */
uint8 MAPS_Dock_24C02_Read_Byte(uint8 Addr)
{
  uint8 I2C_Result=0;
  
  I2C_Result = MAPS_Dock_I2C_Read_Reg(I2C_I2C0,I2C_ADR_24C02,Addr);
  
  return I2C_Result;
}

/*
 *  @brief      Write string array to specified address 24C02
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_24C02_Write_Buff();
 */
void MAPS_Dock_24C02_Write_Buff(uint8 Addr,uint8 *Buff)
{
  uint8 i=0;
  uint8 Len=0;//array length
  uint8 *Buff_Size=Buff;
  
  while(*Buff_Size++ != 0)
  {
    Len++;
  }
  
  for(i=0;i<Len;i++)
  {
    MAPS_Dock_I2C_Write_Reg(I2C_I2C0,I2C_ADR_24C02,(Addr+i),*(Buff+i));
    MAPS_Dock_I2C_Delay(5000);//Writing E2PROM requires a delay of 5Ms
  }
}

/*
 *  @brief      24C02 reads string array at specified address
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_24C02_Read_Buff();
 */
void MAPS_Dock_24C02_Read_Buff(uint8 Addr,uint8 *Buff,uint8 Length)
{
  uint8 i=0;
  uint8 Len=Length;//array length
  
  for(i=0;i<Len;i++)
  {
    *(Buff+i) = MAPS_Dock_I2C_Read_Reg(I2C_I2C0,I2C_ADR_24C02,(Addr+i));
  }
}