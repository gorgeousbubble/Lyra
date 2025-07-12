/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       oled_i2c.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __OLED_I2C_H__
#define __OLED_I2C_H__

#include "common.h"

/*
**I2C port definition
*/
//I2C Port Macro
//I2C SCL port initialization
#ifndef OLED_I2C_PORT_INIT_SCL
#define OLED_I2C_PORT_INIT_SCL       GPIO_Init(PTE26,GPO,0)
#endif
//I2C SDA port initialization
#ifndef OLED_I2C_PORT_INIT_SDA
#define OLED_I2C_PORT_INIT_SDA       GPIO_Init(PTE28,GPO,0)
#endif
//I2C SCL port output
#ifndef OLED_I2C_DDR_OUT_SCL
#define OLED_I2C_DDR_OUT_SCL         GPIO_DDRX(PTE26,1)
#endif
//I2C SDA port output
#ifndef OLED_I2C_DDR_OUT_SDA
#define OLED_I2C_DDR_OUT_SDA         GPIO_DDRX(PTE28,1)
#endif
//I2C SDA port input
#ifndef OLED_I2C_DDR_IN_SDA
#define OLED_I2C_DDR_IN_SDA          GPIO_DDRX(PTE28,0)
#endif
//I2C SCL port high level
#ifndef OLED_I2C_SET_SCL_H
#define OLED_I2C_SET_SCL_H           GPIO_SET(PTE26,1)
#endif
//I2C SCL port low level
#ifndef OLED_I2C_SET_SCL_L
#define OLED_I2C_SET_SCL_L           GPIO_SET(PTE26,0)
#endif
//I2C SDA port high level
#ifndef OLED_I2C_SET_SDA_H
#define OLED_I2C_SET_SDA_H           GPIO_SET(PTE28,1)
#endif
//I2C SDA port low level
#ifndef OLED_I2C_SET_SDA_L
#define OLED_I2C_SET_SDA_L           GPIO_SET(PTE28,0)
#endif
//I2C SDA port input signal
#ifndef OLED_I2C_GET_SDA_IN
#define OLED_I2C_GET_SDA_IN          GPIO_GET(PTE28)
#endif

#define OLED_I2C_DELAY_TIME          I2C_Delay(10)

/*
**Oled coordinate definition
*/
#define OLED_I2C_XLevelL                0x00                            //OledX axis low position
#define OLED_I2C_XLevelH                0x10                            //OledX axis high position
#define OLED_I2C_XLevel	        ((XLevelH&0x0F)*16+XLevelL)             //OledX axis coordinate
#define OLED_I2C_Max_COL	        128                             //Oled maximum number of columns
#define OLED_I2C_Max_ROW		64                              //Oled maximum number of rows
#define	OLED_I2C_BRIGHTNESS	        0xCF                            //Oled brightness
#define OLED_I2C_X_WIDTH                128                             //OledX axis width
#define OLED_I2C_Y_WIDTH                64                              //OledY axis width

/*
**variate declaration
*/
extern const uint8 Oled_FontLib_6x8[][6];
extern const uint8 Oled_FontLib_8x16[];
extern const uint8 Oled_FontLib_12x24[];
extern const uint8 Oled_Picture_128x64_Star[][16];
extern const uint8 Oled_Picture_128x64_JBD_Logo[][16];

/*
**function declaration
*/
extern void Oled_I2C_WrDat(uint8 data);
extern void Oled_I2C_WrCmd(uint8 cmd);
extern void Oled_I2C_Set_Pos(uint8 x,uint8 y);
extern void Oled_I2C_Fill(uint8 data);
extern void Oled_I2C_Clean(void);
extern void Oled_I2C_Reset(void);
extern void Oled_I2C_Init(void);
extern void Oled_I2C_Put_Pixel(uint8 x,uint8 y);
extern void Oled_I2C_Put_Str_6x8(uint8 x,uint8 y,uint8 ch[]);
extern void Oled_I2C_Put_Str_8x16(uint8 x,uint8 y,uint8 ch[]);
extern void Oled_I2C_Put_Str_12x24(uint8 x,uint8 y,uint8 ch[]);
extern void Oled_I2C_Draw_Picture_128x64(const uint8 *Picture);
extern void Oled_I2C_Put_Para_6x8(uint8 x,uint8 y,uint8 ch[],int Value);
extern void Oled_I2C_Put_Para_Pot_6x8(uint8 x,uint8 y,uint8 ch[],float Value);
extern void Oled_I2C_Put_Para_8x16(uint8 x,uint8 y,uint8 ch[],int Value);
extern void Oled_I2C_Put_Para_Pot_8x16(uint8 x,uint8 y,uint8 ch[],float Value);

extern void Oled_I2C_Put_Time_6x8_12(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second, int isAM);
extern void Oled_I2C_Put_Time_6x8_24(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second);
extern void Oled_I2C_Put_Time_8x16_12(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second,int isAM);
extern void Oled_I2C_Put_Time_8x16_24(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second);
extern void Oled_I2C_Put_Date_6x8(uint8 x,uint8 y,uint8 ch[],int year,int month,int day);

#endif