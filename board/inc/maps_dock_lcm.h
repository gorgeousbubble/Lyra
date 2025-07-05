/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_lcm.h
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MAPS_DOCK_LCM_H__
#define __MAPS_DOCK_LCM_H__

#include "common.h"

/*
**LCD is Manley's ZYMG12864 and driver IC is ST7565R
*/

//LCM printing color enumeration
typedef enum
{
  LCM_Pure_Color,       //Orthochromatic
  LCM_Invert_Color,     //Reverse color
}LCM_Color_e;

//LCM port setting macro (GPIO needs to simulate SPI, DSPI only requires DC pin to set CMD/DATA)
#define LCM_PORT_INIT_DC        GPIO_Init(PTD10,GPO,0)  //LCM's data command signal DC
#define LCM_PORT_INIT_CS        GPIO_Init(PTD15,GPO,0)  //LCM data command signal CS

#define LCM_SET_DC_H            GPIO_SET(PTD10,1)       //LCM's DC port high level
#define LCM_SET_DC_L            GPIO_SET(PTD10,0)       //LCM's DC port low level
#define LCM_SET_CS_H            GPIO_SET(PTD15,1)       //LCM CS port high level
#define LCM_SET_CS_L            GPIO_SET(PTD15,0)       //LCM CS port low level

//LCM power on and reset delay
#define LCM_Delay(us)           DELAY_US(us)            //delay


//define the arrtibute of ZYMG12864(LCM)
#define GUI_LCM_XMAX            128             //defined the lcd's line-number is 128
#define GUI_LCM_YMAX 		64		//defined the lcd's column-number is 64
#define GUI_LCM_PAGE		8		//defined the lcd's page-number is 8(GUI_LCM_YMAX/8)

//LCM page
#define LCM_PAGES               5

/*
**variate declaration
*/
extern const uint8 FONTTYPE8_8[][8];
extern const uint8 LCM_Freescale_logo[];

/*
**function declaration
*/
extern void MAPS_Dock_LCM_Fill_Line(uint8 Row,uint8 data);
extern void MAPS_Dock_LCM_Fill(uint8 data);
extern void MAPS_Dock_LCM_CLS(void);
extern void MAPS_Dock_LCM_Reset(void);
extern void MAPS_Dock_LCM_Init(void);
extern void MAPS_Dock_LCM_Put_Pixel(uint8 x,uint8 y);
extern void MAPS_Dock_LCM_Put_Str_6x8(uint8 x,uint8 y,uint8 *ch,LCM_Color_e LCM_Color_x);
extern void MAPS_Dock_LCM_Put_Str_8x16(uint8 x,uint8 y,uint8 *ch,LCM_Color_e LCM_Color_x);
extern void MAPS_Dock_LCM_Put_Str_12x24(uint8 x,uint8 y,uint8 *ch,LCM_Color_e LCM_Color_x);
extern void MAPS_Dock_LCM_Draw_Picture_128x64(const uint8 *Picture,LCM_Color_e LCM_Color_x);
extern void MAPS_Dock_LCM_Put_Para_6x8(uint8 x,uint8 y,uint8 *ch,int Value,LCM_Color_e LCM_Color_x);
extern void MAPS_Dock_LCM_Put_Para_Pot_6x8(uint8 x,uint8 y,uint8 *ch,float Value,LCM_Color_e LCM_Color_x);
extern void MAPS_Dock_LCM_Put_Para_8x16(uint8 x,uint8 y,uint8 *ch,int Value,LCM_Color_e LCM_Color_x);
extern void MAPS_Dock_LCM_Put_Para_Pot_8x16(uint8 x,uint8 y,uint8 *ch,float Value,LCM_Color_e LCM_Color_x);
extern void MAPS_Dock_LCM_Put_Para_Buff_6x8(uint8 x,uint8 y,uint8 *ch,uint8 *buff,LCM_Color_e LCM_Color_x);
extern void MAPS_Dock_LCM_DrawBMP(const uint8 *Buff,LCM_Color_e LCM_Color_x);

#endif