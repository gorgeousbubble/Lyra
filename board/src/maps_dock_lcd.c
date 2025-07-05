/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_lcd.c
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "dwt.h"
#include "ff.h"
#include "maps_dock_lcd.h"
#include "maps_dock_lcd_font.h"
#include "maps_dock_lcd_pic.h"
#include "port.h"
#include "port_cfg.h"

static uint16 ForeColor = BLACKCOLOR;
static uint16 BackColor = BLACKCOLOR;

//BMP image
uint8 BMP_Buffer[MAX_BMP_LINE_BYTE];

/*
 *  @brief      LCD sends commands
 *  @since      v1.0
 *  Sample usage:       LCDC_Send_CMD(0xCF);//LCD sends commands
 */
static inline void LCDC_Send_CMD(uint16 cmd)
{
  *(volatile uint16 *)LCDC_ADRESS_CMD = cmd;
}

/*
 *  @brief      LCD sends data
 *  @since      v1.0
 *  Sample usage:       LCDC_Send_DATA(0xCF);//LCD sends data
 */
static inline void LCDC_Send_DATA(uint16 data)
{
  *(volatile uint16 *)LCDC_ADRESS_DATA = data;
}

/*
 *  @brief      LCD fill color
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_Fill_Color(0xCF);//LCD fill color
 */
void MAPS_LCDC_Fill_Color(uint16 color)
{
  uint32 i=0;
  
  LCDC_Send_CMD(0x2A);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0x00);
  
  LCDC_Send_CMD(0x2B);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0x00);
  
  LCDC_Send_CMD(0x2C);
  
  for(i=0;i<(LCDC_SCREEN_PIXEL_X*LCDC_SCREEN_PIXEL_Y);i++)
  {
    LCDC_Send_DATA(color);
  }
  
}

/*
 *  @brief      LCD Clear Screen
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_CLS();//LCD Clear Screen
 */
void MAPS_LCDC_CLS(void)
{
  uint32 i=0;
  
  LCDC_Send_CMD(0x2A);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0x00);
  
  LCDC_Send_CMD(0x2B);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0x00);
  
  LCDC_Send_CMD(0x2C);
  
  for(i=0;i<(LCDC_SCREEN_PIXEL_X*LCDC_SCREEN_PIXEL_Y);i++)
  {
    LCDC_Send_DATA(0x00);
  }
}

/*
 *  @brief      MAPS_LCD_FlexBus Initialization
 *  @since      v1.0
 *  Sample usage:       MAPS_LCD_FlexBus_Init();//MAPS_LCD Initialization
 */
static void MAPS_LCD_FlexBus_Init(void)
{
  //FlexBus clock initialization
  SIM_SOPT2 |= SIM_SOPT2_FBSL(3);             //FlexBus security level: Allow command and data access
  SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;        //enable clock
  
  //MAPs_LCD Port Configuration and FlexBus Initialization
  //FlexBus
  PORT_Init(FB_AD0 ,ALT5 | SSR);//PTD6
  PORT_Init(FB_AD1 ,ALT5 | SSR);//PTD5
  PORT_Init(FB_AD2 ,ALT5 | SSR);//PTD4
  PORT_Init(FB_AD3 ,ALT5 | SSR);//PTD3
  PORT_Init(FB_AD4 ,ALT5 | SSR);//PTD2
  PORT_Init(FB_AD5 ,ALT5 | SSR);//PTC10
  PORT_Init(FB_AD6 ,ALT5 | SSR);//PTC9
  PORT_Init(FB_AD7 ,ALT5 | SSR);//PTC8
  PORT_Init(FB_AD8 ,ALT5 | SSR);//PTC7
  PORT_Init(FB_AD9 ,ALT5 | SSR);//PTC6
  PORT_Init(FB_AD10 ,ALT5 | SSR);//PTC5
  PORT_Init(FB_AD11 ,ALT5 | SSR);//PTC4
  PORT_Init(FB_AD12 ,ALT5 | SSR);//PTC2
  PORT_Init(FB_AD13 ,ALT5 | SSR);//PTC1
  PORT_Init(FB_AD14 ,ALT5 | SSR);//PTC0
  PORT_Init(FB_AD15 ,ALT5 | SSR);//PTB18
  PORT_Init(FB_AD16 ,ALT5 | SSR);//PTB17
  PORT_Init(FB_AD17 ,ALT5 | SSR);//PTB16
  PORT_Init(FB_AD18 ,ALT5 | SSR);//PTB11
  PORT_Init(FB_AD19 ,ALT5 | SSR);//PTB10
  PORT_Init(FB_AD20 ,ALT5 | SSR);//PTB9
  //LCD PORT
  PORT_Init(MAPS_LCD_RD ,ALT5 | SSR);//PTB19
  PORT_Init(MAPS_LCD_RW ,ALT5 | SSR);//PTC11
  PORT_Init(MAPS_LCD_CLK ,ALT5 | SSR);//PTC3
  PORT_Init(MAPS_LCD_ALE ,ALT5 | SSR);//PTD0
  PORT_Init(MAPS_LCD_CS0 ,ALT5 | SSR);//PTD1
  PORT_Init(MAPS_LCD_CS2 ,ALT5 | SSR);//PTC18
  PORT_Init(MAPS_LCD_BE31_24 ,ALT5 | SSR);//PTC17
  PORT_Init(MAPS_LCD_BE23_16 ,ALT5 | SSR);//PTC16
  
  //FlexBus configuration
  FB_CSMR0 = (0
              | FB_CSMR_BAM(FLEX_ADRESS_MASK)   //Write FlexBus address mask
              | FB_CSMR_V_MASK                  //Selection signal available
              );
  
  FB_CSAR0 = (0 | FB_CSAR_BA(FLEX_BASE_ADDRESS));//Write FlexBus address
  
  FB_CSCR0 = (0
              | FB_CSCR_BLS_MASK                //Bate-Lane Shift (right aligned mode)
              | FB_CSCR_AA_MASK                 //Automatic response ACK
              | FB_CSCR_PS(2)                   //16Bit data transfer
              | FB_CSCR_ASET(3)                 //Set the address for the fourth rising edge
              | FB_CSCR_WS(3)                   //Waiting bit WS=3, waiting for 3 cycles
              | FB_CSCR_RDAH(3)                 //Read bus and wait for 4 cycles
              );
  
  //SRAM configuration
  FB_CSMR2 = (0
              | FB_CSMR_BAM(SRAM_ADRESS_MASK)   //Write SRAM address mask
              | FB_CSMR_V_MASK                  //Selection signal available
              );
  
  FB_CSAR2 = (0 | FB_CSAR_BA(SRAM_BASE_ADDRESS));//Write SRAM address
  
  FB_CSCR2 = (0
              | FB_CSCR_BLS_MASK                //Bate-Lane Shift(Right Alignment Mode)
              | FB_CSCR_AA_MASK                 //Automatic response ACK
              | FB_CSCR_PS(2)                   //16Bit data transfer
              | FB_CSCR_ASET(1)                 //Set the address for the second rising edge
              | FB_CSCR_WS(2)                   //Waiting bit WS=2, waiting for 2 cycles
              | FB_CSCR_RDAH(1)                 //Read bus and wait for 2 cycles
              | FB_CSCR_WRAH(1)                 //Write bus and wait for 2 cycles
              | FB_CSCR_BEM_MASK                //Enable Mode
              );
  
  FB_CSPMCR = (0
               | FB_CSPMCR_GROUP2(2)            //Select FB_BE_31_24
               | FB_CSPMCR_GROUP3(2)            //Select FB_BE_23_16
               | FB_CSPMCR_GROUP4(1)            //Select FB_CS2
               );
  
}

/*
 *  @brief      MAPS_LCD Initialization
 *  @since      v1.0
 *  Sample usage:       MAPS_LCD_Init();//Initialize MAPS_LCD
 */
void MAPS_LCD_Init(void)
{
  MAPS_LCD_FlexBus_Init();//FlexBus Initialize
  
  LCDC_Send_CMD(0xCF);  //Power control B
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0xC1);
  LCDC_Send_DATA(0x30);
  
  LCDC_Send_CMD(0xED);  //Power on sequence control
  LCDC_Send_DATA(0x64);
  LCDC_Send_DATA(0x03);
  LCDC_Send_DATA(0x12);
  LCDC_Send_DATA(0x81);
  
  LCDC_Send_CMD(0xE8);  //Driver timing control A
  LCDC_Send_DATA(0x85); 
  LCDC_Send_DATA(0x10); 
  LCDC_Send_DATA(0x7A);
  
  LCDC_Send_CMD(0xCB);  //Power control A
  LCDC_Send_DATA(0x39); 
  LCDC_Send_DATA(0x2C); 
  LCDC_Send_DATA(0x00); 
  LCDC_Send_DATA(0x34); 
  LCDC_Send_DATA(0x02);
  
  LCDC_Send_CMD(0xF7);  //Pump ratio control
  LCDC_Send_DATA(0x20);
  
  LCDC_Send_CMD(0xEA);  //Driver timing control B
  LCDC_Send_DATA(0x00); 
  LCDC_Send_DATA(0x00);
  
  LCDC_Send_CMD(0xC0);  //Power control 
  LCDC_Send_DATA(0x1B); //VRH[5:0]  1B
    
  LCDC_Send_CMD(0xC1);  //Power control 
  LCDC_Send_DATA(0x01); //SAP[2:0];BT[3:0] 
    
  LCDC_Send_CMD(0xC5);  //VCM control 
  LCDC_Send_DATA(0x45); //3F
  LCDC_Send_DATA(0x25); //3C
    
  LCDC_Send_CMD(0xC7);  //VCM control2 
  LCDC_Send_DATA(0xB7); //b7
  
  LCDC_Send_CMD(0x36);  //Memory Access Control 
  LCDC_Send_DATA(0x28); 
    
  LCDC_Send_CMD(0x3A);  //Pixel Format Set
  LCDC_Send_DATA(0x55); 
    
  LCDC_Send_CMD(0xB1);  //Frame Rate Control
  LCDC_Send_DATA(0x00);   
  LCDC_Send_DATA(0x1A); 
    
  LCDC_Send_CMD(0xB6);  //Display Function Control 
  LCDC_Send_DATA(0x0A); 
  LCDC_Send_DATA(0x82); 
    
  LCDC_Send_CMD(0xF2);  //3Gamma Function Disable 
  LCDC_Send_DATA(0x00); 
    
  LCDC_Send_CMD(0x26);  //Gamma curve selected 
  LCDC_Send_DATA(0x01);
  
  LCDC_Send_CMD(0xE0);  //Set Gamma 
  LCDC_Send_DATA(0x0F); 
  LCDC_Send_DATA(0x2A); 
  LCDC_Send_DATA(0x28); 
  LCDC_Send_DATA(0x08); 
  LCDC_Send_DATA(0x0E); 
  LCDC_Send_DATA(0x08); 
  LCDC_Send_DATA(0x54); 
  LCDC_Send_DATA(0xA9); 
  LCDC_Send_DATA(0x43); 
  LCDC_Send_DATA(0x0A); 
  LCDC_Send_DATA(0x0F); 
  LCDC_Send_DATA(0x00); 
  LCDC_Send_DATA(0x00); 
  LCDC_Send_DATA(0x00); 
  LCDC_Send_DATA(0x00); 
    
  LCDC_Send_CMD(0xE1);  //Set Gamma 
  LCDC_Send_DATA(0x00); 
  LCDC_Send_DATA(0x15); 
  LCDC_Send_DATA(0x17); 
  LCDC_Send_DATA(0x07); 
  LCDC_Send_DATA(0x11); 
  LCDC_Send_DATA(0x06); 
  LCDC_Send_DATA(0x2B); 
  LCDC_Send_DATA(0x56); 
  LCDC_Send_DATA(0x3C); 
  LCDC_Send_DATA(0x05); 
  LCDC_Send_DATA(0x10); 
  LCDC_Send_DATA(0x0F); 
  LCDC_Send_DATA(0x3F); 
  LCDC_Send_DATA(0x3F); 
  LCDC_Send_DATA(0x0F); 
    
  LCDC_Send_CMD(0x2A);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0x01);
  LCDC_Send_DATA(0x3F);	
    
  LCDC_Send_CMD(0x2B);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0xEF);
  
  LCDC_Send_CMD(0x11);   //Exit Sleep
  LCDC_DELAY(10);
  LCDC_Send_CMD(0x29);   //display on
  LCDC_DELAY(10);
  LCDC_Send_CMD(0x2C);
  MAPS_LCDC_Fill_Color(BLACKCOLOR);//Initialize screen clearing
  LCDC_DELAY(100);
  MAPS_LCDC_Fill_Color(BackColor);//Set the background color to black
  
}

/*
 *  @brief      MAPs_LCDC Settings Window Form
 *  @param      uint16    x
 *  @param      uint16    y
 *  @param      uint16    w
 *  @param      uint16    h
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_Set_Window();//MAPs_LCD Settings Form
 */
void MAPS_LCDC_Set_Window(uint16 x,uint16 y,uint16 w,uint16 h)
{
  LCDC_Send_CMD(0x2A);
  LCDC_Send_DATA(x>>8);
  LCDC_Send_DATA(x&0xFF);
  LCDC_Send_DATA((x+w-1)>>8);
  LCDC_Send_DATA((x+w-1)&0xFF);	
    
  LCDC_Send_CMD(0x2B);
  LCDC_Send_DATA(y>>8);
  LCDC_Send_DATA(y&0xFF);
  LCDC_Send_DATA((y+h-1)>>8);
  LCDC_Send_DATA((y+h-1)&0xFF);
}

/*
 *  @brief      MAPS_LCDC draw pixel
 *  @param      uint16    x
 *  @param      uint16    y
 *  @param      uint16    color
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_DrawPixel();//MAPS_LCD draw pixel
 */
void MAPS_LCDC_DrawPixel(uint16 x,uint16 y,uint16 color)
{
  LCDC_Send_CMD(0x2A);
  LCDC_Send_DATA(x>>8);
  LCDC_Send_DATA(x&0xFF);
  LCDC_Send_CMD(0x2B);
  LCDC_Send_DATA(y>>8);
  LCDC_Send_DATA(y&0xFF);
  LCDC_Send_CMD(0x2C);
  LCDC_Send_DATA(color);
}

/*
 *  @brief      Write a 6x8 character MAPS LCDC
 *  @param      uint16    x
 *  @param      uint16    y
 *  @param      uint8     bc
 *  @param      uint8     c
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_DrawChar_6x8();//Write a 6x8 character MAPS LCDC
 */
void MAPS_LCDC_DrawChar_6x8(uint16 x,uint16 y,uint8 bc,uint8 c)
{
  int i=0,j=0,k=0;
  
  if(c>0x1f && c<0x90)
  {
    k = (c - 0x20) * 8;
    for(i=0;i<8;i++)
    {
      for(j=0;j<6;j++)
      {
        if(Font_6x8_h[k+i] & (0x01<<(7-j)))
        {
          MAPS_LCDC_DrawPixel(x+j,y+i,ForeColor);
        }
        else
        {
          if(bc)
          {
            MAPS_LCDC_DrawPixel(x+j,y+i,BackColor);
          }
        }
      }
    }
  }
}

/*
 *  @brief      Write an 8x16 character MAPS LCDC
 *  @param      uint16    x
 *  @param      uint16    y
 *  @param      uint8     bc
 *  @param      uint8     c
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_DrawChar_8x16();//Write an 8x16 character MAPS LCDC
 */
void MAPS_LCDC_DrawChar_8x16(uint16 x,uint16 y,uint8 bc,uint8 c)
{
  int i=0,j=0,k=0;
  
  if(c>0x1f && c<0x90)
  {
    k = c * 16;
    for(i=0;i<16;i++)
    {
      for(j=0;j<8;j++)
      {
        if(Font_8x16_h[k+i] & (0x01<<(7-j)))
        {
          MAPS_LCDC_DrawPixel(x+j,y+i,ForeColor);
        }
        else
        {
          if(bc)
          {
            MAPS_LCDC_DrawPixel(x+j,y+i,BackColor);
          }
        }
      }
    }
  }
}

/*
 *  @brief      MAPs_LCDC displays characters
 *  @param      uint16    x
 *  @param      uint16    y
 *  @param      uint8     font
 *  @param      uint8     bc
 *  @param      char      c
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_DisplayChar();//MAPs_LCDC displays characters
 */
void MAPS_LCDC_DisplayChar(uint16 x,uint16 y,uint8 font,uint8 bc,char c)
{
  if(c & 0x80)
  {
    return;
  }
  switch(font)
  {
    case 0:
            MAPS_LCDC_DrawChar_6x8(x,y,bc,c);
            break;
    case 1:
            MAPS_LCDC_DrawChar_8x16(x,y,bc,c);
            break;
    default:
            MAPS_LCDC_DrawChar_8x16(x,y,bc,c);
            break;
  }
}

/*
 *  @brief      MAPs_LCDC display string
 *  @param      uint16    x
 *  @param      uint16    y
 *  @param      uint8     font
 *  @param      uint8     bc
 *  @param      char      *String
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_DisplayString();//MAPS_LCD display string
 */
void MAPS_LCDC_DisplayString(uint16 x,uint16 y,uint8 font,uint8 bc,char *String)
{
  while(*String)
  {
    switch(font)
    {
      case 0:
              MAPS_LCDC_DisplayChar(x,y,font,bc,*String++);
              x += 6;
              break;
      case 1:
              MAPS_LCDC_DisplayChar(x,y,font,bc,*String++);
              x += 8;
              break;
      default:
              MAPS_LCDC_DisplayChar(x,y,font,bc,*String++);
              x += 8;
              break;
    }
  }
}

/*
 *  @brief      MAPS_LCDC background
 *  @param      uint16  *Color
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_Show();//MAPS_LCD background
 */
void MAPS_LCDC_Show(uint16 *Color)
{
  uint32 i=0;
  
  LCDC_Send_CMD(0x2A);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0x00);;	
    
  LCDC_Send_CMD(0x2B);
  LCDC_Send_DATA(0x00);
  LCDC_Send_DATA(0x00);
    
  LCDC_Send_CMD(0x2C);
  for(i=0;i<(LCDC_SCREEN_PIXEL_X*LCDC_SCREEN_PIXEL_Y);i++)
  {
    LCDC_Send_DATA(*Color++);
  }
}

/*
 *  @brief      MAPs_LCDC displays images
 *  @param      uint16    x
 *  @param      uint16    y
 *  @param      uint16    w
 *  @param      uint16    h
 *  @param      uint16   *BMP_PTemp
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_BMP();//MAPs_LCDC displays images
 */
void MAPS_LCDC_BMP(uint16 x,uint16 y,uint16 w,uint16 h,uint16 *BMP_PTemp)
{
  uint32 i=0,j=0;
  uint16 *BMP_DATA_PTR=(uint16*)BMP_PTemp;
  uint16 *BMP_SRAM_PTR=(uint16*)(SRAM_BASE_ADDRESS<<16);
  
  for(i=0,j=0;i<(w*h);i++,j++)
  {
    *BMP_SRAM_PTR++ = *((uint16*)(BMP_DATA_PTR++));
  }
  
  MAPS_LCDC_Set_Window(x,y,w,h);
  
  LCDC_Send_CMD(0x2C);
  
  BMP_SRAM_PTR=(uint16*)(SRAM_BASE_ADDRESS<<16);
    
  for(i=0;i<(w*h);i++)
  {
    LCDC_Send_DATA(*BMP_SRAM_PTR++);
  }
    
}

/*
 *  @brief      MAPs_LCDC reads an image from an SD card
 *  @param      char*           file
 *  @param      Site_t          Site
 *  @since      v1.0
 *  Sample usage:       MAPS_LCDC_BMP_From_SD();//MAPs_LCDC reads an image from an SD card
 */
uint8 MAPS_LCDC_BMP_From_SD(char *file,Site_t Site)
{
  FIL fdst;     //file
  FATFS fs;     //file system
  FRESULT res;  //file return
  BMP_FILEHEADER BMP_FileHead;//BMP image file
  uint32 Br;
  
  int Width,Height;
  uint32 OffBits,Bitcnt;
  
  uint8 r,g,b;
  uint16 rgb;

  Size_t size;
  Site_t site;

  int x=0,y=0,BytesPerLine,Step;

  uint8 *pColor;

  
  res = f_mount(0, &fs);//Mount file system
  res = f_open(&fdst, file, FA_OPEN_EXISTING | FA_READ);//Open file, read-only
  if(res)//If opening the file fails
  {
    goto BMP_EXIT_FALSE;//Jump to exit function
  }
  
  res = f_read(&fdst, &BMP_FileHead, sizeof(BMP_FileHead), &Br);//Read file data to BMP_SileHead array
  if(res)//If reading the file fails
  {
    goto BMP_EXIT_FALSE;//Jump to exit function
  }
  
  Width = BMP_FileHead.biWidth;         //image width
  Height = BMP_FileHead.biHeight;       //image height
  OffBits = BMP_FileHead.bfOffBits;     //The offset between the beginning of the file and the beginning of the bitmap data
  Bitcnt = BMP_FileHead.biBitCount;     //Explain the number of bits per pixel, with values of 1, 4, 8, 16, 24, and 32
  
  if((BMP_FileHead.bfType != 0x4d42) || (BMP_FileHead.biCompression))//Not BMP bitmap type or uncompressed
  {
    goto BMP_EXIT_FALSE;//Jump to exit function
  }
  
  if(Width > MAX_BMP_W)//Width exceeds the maximum display width of LCD
  {
    goto BMP_EXIT_FALSE;//Jump to exit function
  }
  
  if(Height > 0)//If the height is a positive number, read the data from the bottom left corner upwards
  {
    y = Height - 1 + (LCDC_SCREEN_PIXEL_Y - Height)/2;
    Step = -1;
  }
  else if(Height < 0)//If the height is negative, read the data from the top left corner downwards
  {
    Height = -Height;
    y = (LCDC_SCREEN_PIXEL_Y - Height)/2;
    Step = 1;
  }
  else
  {
    goto BMP_EXIT_FALSE;//Jump to exit function
  }
  
  size.H= 1;
  size.W = Width;
  site.x = Site.x;
  BytesPerLine = BMP_LINE_BYTE(Width,Bitcnt);
  
  switch(Bitcnt)
  {
    case 24://24 bit true color: RGB24
            res = f_lseek(&fdst,OffBits);//Find the starting position of BMP color data
            for(;(y>=((LCDC_SCREEN_PIXEL_Y-Height)/2))&&(y<(Height+(LCDC_SCREEN_PIXEL_Y-Height)/2));y+=Step)
            {
              res = f_read(&fdst, BMP_Buffer, BytesPerLine, &Br);//Read a line of image information
              if(res)//If reading the file fails
              {
                goto BMP_EXIT_FALSE;//Jump to exit function
              }
              pColor = BMP_Buffer;//BMP_Buffer address
              
              site.y = Site.y + y;
              MAPS_LCDC_Set_Window(site.x,site.y,size.W,size.H);//Set window form
              LCDC_Send_CMD(0x2C);//Write Mode
              
              for(x=0;x<Width;x++)
              {
                b = *pColor++;
                g = *pColor++;
                r = *pColor++;
                
                rgb = RGB24_RGB565(r,g,b);
                LCDC_Send_DATA(rgb);
              }
              
            }
            break;
    case 16://16 bit bitmap
            res = f_lseek(&fdst,OffBits);//Find the starting position of BMP color data
            for(;(y>=((LCDC_SCREEN_PIXEL_Y-Height)/2))&&(y<(Height+(LCDC_SCREEN_PIXEL_Y-Height)/2));y+=Step)
            {
              res = f_read(&fdst, BMP_Buffer, BytesPerLine, &Br);//Read a line of image information
              if(res)//If reading the file fails
              {
                goto BMP_EXIT_FALSE;//Jump to exit function
              }
              pColor = BMP_Buffer;//BMP_Buffer address
              
              site.y = Site.y + y;
              MAPS_LCDC_Set_Window(site.x,site.y,size.W,size.H);//Set window form
              LCDC_Send_CMD(0x2C);//Write Mode
              
              for(x=0;x<Width;x++)
              {
                rgb = *(uint16 *)pColor;
                rgb = RGB555_RGB565(rgb);
                LCDC_Send_DATA(rgb);
                pColor += 2;
              }
              
            }
            break;
    case 1://Monochrome BMP
            res = f_read(&fdst, BMP_Buffer, 2*sizeof(RGBQUAD), &Br);
            uint16 color[2];
            color[0] = RGB24_RGB565(BMP_Buffer[2],BMP_Buffer[1],BMP_Buffer[0]);
            color[1] = RGB24_RGB565(BMP_Buffer[6],BMP_Buffer[5],BMP_Buffer[4]);
            
            res = f_lseek(&fdst,OffBits);//Find the starting position of BMP color data
            
            Width = (Width+7)/8;//8 pixels per point (+7 equals 1)
            for(;(y>=((LCDC_SCREEN_PIXEL_Y-Height)/2))&&(y<(Height+(LCDC_SCREEN_PIXEL_Y-Height)/2));y+=Step)
            {
              res = f_read(&fdst, BMP_Buffer, BytesPerLine, &Br);//Read a line of image information
              if(res)//If reading the file fails
              {
                goto BMP_EXIT_FALSE;//Jump to exit function
              }
              pColor = BMP_Buffer;//BMP_Buffer address
              
              site.y = Site.y + y;
              MAPS_LCDC_Set_Window(site.x,site.y,size.W,size.H);//Set window form
              LCDC_Send_CMD(0x2C);//Write Mode
              
              for(x=0;x<Width;x++)
              {
                rgb = color[(*pColor >> 7)&0x01];
                LCDC_Send_DATA(rgb);
                rgb = color[(*pColor >> 6)&0x01];
                LCDC_Send_DATA(rgb);
                rgb = color[(*pColor >> 5)&0x01];
                LCDC_Send_DATA(rgb);
                rgb = color[(*pColor >> 4)&0x01];
                LCDC_Send_DATA(rgb);
                rgb = color[(*pColor >> 3)&0x01];
                LCDC_Send_DATA(rgb);
                rgb = color[(*pColor >> 2)&0x01];
                LCDC_Send_DATA(rgb);
                rgb = color[(*pColor >> 1)&0x01];
                LCDC_Send_DATA(rgb);
                rgb = color[(*pColor >> 0)&0x01];
                LCDC_Send_DATA(rgb);
                
                pColor += 1;
              }
              
            }
            break;
    default:
            goto BMP_EXIT_FALSE;//Jump to exit function
            break;
  }
  res = f_close(&fdst);
  res = f_mount(0,NULL);
  return TRUE;
  
 BMP_EXIT_FALSE://exit function
  res = f_close(&fdst);
  res = f_mount(0,NULL);
  return FALSE;
}