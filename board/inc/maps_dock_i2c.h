/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_i2c.h
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MAPS_DOCK_I2C_H__
#define __MAPS_DOCK_I2C_H__

#include "common.h"

/*
**I2C addresses
*/
#define I2C_ADR_24C02   0x50    //24C02 address(E2PROM)
#define I2C_ADR_CS43L22 0x4A    //CS43L22 address

/*
**Internal register address
*/
//24C02
#define I2C_24C02_MAX_ADR       0x00000100      //24C02 maximum addresses 255Byte

/*
**I2C communication related macros
*/
#define MAPS_Dock_I2C_Delay(us) DELAY_US(us)    //I2C delay us

/*
**function declaration
*/
extern void MAPS_Dock_24C02_Write_Byte(uint8 Addr,uint8 Data);
extern uint8 MAPS_Dock_24C02_Read_Byte(uint8 Addr);
extern void MAPS_Dock_24C02_Write_Buff(uint8 Addr,uint8 *Buff);
extern void MAPS_Dock_24C02_Read_Buff(uint8 Addr,uint8 *Buff,uint8 Length);

#endif