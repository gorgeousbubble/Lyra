/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_lcd.h
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MAPS_DOCK_LCD_H__
#define __MAPS_DOCK_LCD_H__

#include "common.h"

/*
**MAPs_LCD related port macros
*/
#define MAPS_LCD_RD             PTB19
#define MAPS_LCD_RW             PTC11
#define MAPS_LCD_CLK            PTC3
#define MAPS_LCD_ALE            PTD0
#define MAPS_LCD_CS0            PTD1
#define MAPS_LCD_CS2            PTC18
#define MAPS_LCD_BE31_24        PTC17
#define MAPS_LCD_BE23_16        PTC16

#define MAPS_LCD_BLC            PTD9
#define MAPS_LCD_DC             PTD10

/*
**FLEXBUS related macros
*/
#define FLEX_BASE_ADDRESS       0x6000
#define SRAM_BASE_ADDRESS       0x8000
#define FLEX_ADRESS_MASK        0x000f
#define SRAM_ADRESS_MASK        0x001f
#define SRAM_CHIP_SELECTION     2
#define LCD_CHIP_SELECTION      0

/*
**MAPs_LCD related settings
*/

#define LCDC_ADRESS_CMD         0x60000000      //LCD Command Address
#define LCDC_ADRESS_DATA        0x60000002      //LCD Data Address

#define LCDC_SCREEN_PIXEL_X     320             //X-axis pixel coordinates of LCD screen
#define LCDC_SCREEN_PIXEL_Y     240             //Y-axis pixel coordinates of LCD screen

//Common colors
#define BLACKCOLOR              0x0000          //black color
#define WHITECOLOR              0xffff          //WHITECOLOR color
#define REDCOLOR                0xf800          //red color
#define YELLOWCOLOR             0xffe0          //yellow color
#define BLUECOLOR               0x001f          //blue color
#define GREENCOLOR              0x07e0          //green color
#define CELESTECOLOR            0x07ff          //celeste color
#define PURPLECOLOR             0xf81f          //purple color

//Extract RGB primary colors, RGB565 is 16 bit data
#define RGB565_RED(rgb565)      (uint8)(((uint16)(rgb565)>>11)&0x1f)
#define RGB565_GREEN(rgb565)    (uint8)(((uint16)(rgb565)>>5)&0x3f)
#define RGB565_BLUE(rgb565)     (uint8)((uint16)(rgb565)&0x1f)

//Grayscale image and RGB565 image conversion
#define GRAY_TO_RGB565(gray)    ((uint16)((((uint8)(gray)>>3)<<11)|(((uint8)(gray)>>2)<<5)|((uint8)(gray)>>3)))
#define RGB565_TO_GRAY(rgb565)  ((uint8)(((RGB565_RED(rgb565)*235+RGB565_GREEN(rgb565)*613+RGB565_BLUE(rgb565)*625)+1)>>8))

#define RGB565_H(rgb565)        ((uint8)(((uint16)(rgb565))>>8))
#define RGB565_L(rgb565)        (uint8)(rgb565))

#define RGB24_RGB565(R,G,B)     ((uint16)((((uint8)(R)>>3)<<11)|(((uint8)(G)>>2)<<5)|((uint8)(B)>>3)))
#define RGB555_RGB565(rgb555)   (((((uint16)(rgb555)) & (uint16)(~0x1F)) << 1) |  (((uint16)(rgb555)) & 0x1F))
#define BGR555_RGB565(bgr555)   ((((uint16)(bgr555)<<11) & (uint16)(0x1F<<11)) | (((uint16)(bgr555)) & (uint16)(0x1F<<5)) | (((uint16)(bgr555)>>10) & (uint16)(0x1F)))

//LCDC delay macros
#define LCDC_DELAY(us)          DELAY_US(us)

/*
**BMP image information
*/
#define MAX_BMP_H               240         //BMP Support maximum height
#define MAX_BMP_W               320         //BMP Support maximum weight
#define MAX_BMP_BIT             24          //BMP Support maximum bit

//Image compression enumeration
typedef enum
{
  BI_RGB,               //No compression
  BI_RLE8,              //Each pixel is encoded with 8-bit RLE compression, and the compression format consists of 2 bytes (repeated pixel count and color index);
  BI_RLE4,              //4 bits of RLE compression encoding per pixel, with a compression format consisting of 2 bytes
  BI_BITFIELDS,         //The bits of each pixel are determined by a specified mask.
}biCompression_e;

//Image Information Structure
#pragma pack(2)

typedef struct          //Must be double byte aligned (or single byte aligned)
{
  //BMP bitmap file header
  uint16 bfType;         //Offset: 0x00, file flag 'BM', Used to identify BMP bitmap types
  uint32 bfSize;         //Offset: 0x02, file size
  uint16 bfReserved1;    //Offset: 0x06, reserved
  uint16 bfReserved2;    //Offset: 0x08, reserved
  uint32 bfOffBits;      //Offset: 0x0A, the offset between the beginning of the file and the beginning of the bitmap data

  //BMP bitmap header
  uint32 biSize;         //Offset: 0x0E, the number of bytes in the bitmap information header.
  int32  biWidth;        //Offset: 0x12, image width
  int32  biHeight;       //Offset: 0x16, image height. If it is a positive number, it indicates that the image data is scanned from the bottom left corner of the image. If it is a negative number, it is scanned from the top left corner
  uint16 biPlanes;       //Offset: 0x1A, indicates the number of bit planes for the target device, and its value will always be set to 1
  uint16 biBitCount;     //Offset: 0x1C, indicating the number of bits per pixel, with values of 1, 4, 8, 16, 24, or 32
  uint32 biCompression;  //Offset: 0x1E, indicating the type of image data compression. Its value type is biCompression-e
  uint32 biSizeImage;    //Offset: 0x22, indicating the size of the image in bytes. When using BI_SGB format, it can be set to 0
  uint32 biXPelsPerMeter;//Offset: 0x26, indicating horizontal resolution, expressed in pixels/meter, signed integer
  uint32 biYPelsPerMeter;//Offset: 0x2A, indicating vertical resolution, expressed in pixels/meter, signed integer
  uint32 biClrUsed;      //Offset: 0x2E, indicating the color index number in the actual color table used for the bitmap
  uint32 biClrImportant; //Offset: 0x32, indicating the number of color indices that have a significant impact on image display. If it is 0, it means they are all important.
}BMP_FILEHEADER;

//Color palette color structure
typedef struct tagRGBQUAD
{
  uint8 rgbBlue;
  uint8 rgbGreen;
  uint8 rgbRed;
  uint8 rgbReserved;
}RGBQUAD;

//Image related macros
#define BMP_SIZE(W,H)           ((((W)*4+3)/4)*(H))
#define BMP_BYTECNT(W,H,BIT)    (BMP_SIZE(W,H)*BIT/8)

#define MAX_BMP_SIZE            BMP_SIZE(MAX_BMP_W,MAX_BMP_H)
#define MAX_BMP_BYTECNT         BMP_BYTECNT(MAX_BMP_W,MAX_BMP_H,MAX_BMP_BIT)

#define BMP_LINE_BYTE(W,BIT)    ((((W)*(BIT)+31)/32)*4)
#define MAX_BMP_LINE_BYTE       BMP_LINE_BYTE(MAX_BMP_W,MAX_BMP_BIT)

/*
**variate declaration
*/
extern const uint8 Font_6x8_h[];
extern const uint8 Font_8x16_h[256*16];
extern const uint8 LCDC_Freescale_Logo[153600];

/*
**function declaration
*/
extern void MAPS_LCD_Init(void);
extern void MAPS_LCDC_CLS(void);
extern void MAPS_LCDC_Fill_Color(uint16 color);
extern void MAPS_LCDC_Set_Window(uint16 x,uint16 y,uint16 w,uint16 h);
extern void MAPS_LCDC_DrawPixel(uint16 x,uint16 y,uint16 color);
extern void MAPS_LCDC_DrawChar_6x8(uint16 x,uint16 y,uint8 bc,uint8 c);
extern void MAPS_LCDC_DrawChar_8x16(uint16 x,uint16 y,uint8 bc,uint8 c);
extern void MAPS_LCDC_DisplayChar(uint16 x,uint16 y,uint8 font,uint8 bc,char c);
extern void MAPS_LCDC_DisplayString(uint16 x,uint16 y,uint8 font,uint8 bc,char *String);
extern void MAPS_LCDC_Show(uint16 *Color);
extern void MAPS_LCDC_BMP(uint16 x,uint16 y,uint16 w,uint16 h,uint16 *BMP_PTemp);
extern uint8 MAPS_LCDC_BMP_From_SD(char *file,Site_t Site);

#endif