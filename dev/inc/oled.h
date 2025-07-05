/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       oled.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __OLED_H__
#define __OLED_H__

#include "common.h"

/*
**SPI port definition
*/

/*
**CK: Clock Input DI: Data Input RST: Reset Signal DC: Data/Command
*/
#define OLED_PORT_INIT_CK       GPIO_Init(PTB21,GPO,0)  //Clock signal CK               //PTC16 of SPI
#define OLED_PORT_INIT_DI       GPIO_Init(PTB23,GPO,0)  //SPI data signal DI            //PTC17
#define OLED_PORT_INIT_RST      GPIO_Init(PTB19,GPO,0)  //Reset signal RST              //PTC18 of SPI
#define OLED_PORT_INIT_DC       GPIO_Init(PTB20,GPO,0)  //SPI data command signal DC    //PTC19

/*
**Port level macro
*/
#define OLED_SET_CK_H           GPIO_SET(PTB21,1)       //SPI setting CK high level     //PTC16
#define OLED_SET_CK_L           GPIO_SET(PTB21,0)       //SPI setting CK low level      //PTC16
#define OLED_SET_DI_H           GPIO_SET(PTB23,1)       //SPI setting DI high level     //PTC17
#define OLED_SET_DI_L           GPIO_SET(PTB23,0)       //SPI setting DI low level      //PTC17
#define OLED_SET_RST_H          GPIO_SET(PTB19,1)       //SPI setting RST high level    //PTC18
#define OLED_SET_RST_L          GPIO_SET(PTB19,0)       //SPI setting RST low level     //PTC18
#define OLED_SET_DC_H           GPIO_SET(PTB20,1)       //SPI setting DC high level     //PTC19
#define OLED_SET_DC_L           GPIO_SET(PTB20,0)       //SPI setting DC low level      //PTC19

/*
**Oled coordinate definition
*/
#define OLED_XLevelL            0x00                            //OledX axis low position
#define OLED_XLevelH	        0x10                            //OledX axis high position
#define OLED_XLevel	        ((XLevelH&0x0F)*16+XLevelL)     //OledX axis coordinate
#define OLED_Max_COL	        128                             //Oled maximum number of columns
#define OLED_Max_ROW		64                              //Oled maximum number of rows
#define	OLED_BRIGHTNESS	        0xCF                            //Oled brightness
#define OLED_X_WIDTH            128                             //OledX axis width
#define OLED_Y_WIDTH            64                              //OledY axis width

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
extern void Oled_WrDat(uint8 data);
extern void Oled_WrCmd(uint8 cmd);
extern void Oled_Set_Pos(uint8 x,uint8 y);
extern void Oled_Fill(uint8 data);
extern void Oled_Clean(void);
extern void Oled_Reset(void);
extern void Oled_Init(void);
extern void Oled_Put_Pixel(uint8 x,uint8 y);
extern void Oled_Put_Str_6x8(uint8 x,uint8 y,uint8 ch[]);
extern void Oled_Put_Str_8x16(uint8 x,uint8 y,uint8 ch[]);
extern void Oled_Put_Str_12x24(uint8 x,uint8 y,uint8 ch[]);
extern void Oled_Draw_Picture_128x64(const uint8 *Picture);
extern void Oled_Put_Para_6x8(uint8 x,uint8 y,uint8 ch[],int Value);
extern void Oled_Put_Para_Pot_6x8(uint8 x,uint8 y,uint8 ch[],float Value);
extern void Oled_Put_Para_8x16(uint8 x,uint8 y,uint8 ch[],int Value);
extern void Oled_Put_Para_Pot_8x16(uint8 x,uint8 y,uint8 ch[],float Value);

#endif