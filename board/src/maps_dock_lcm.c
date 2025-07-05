/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_lcm.c
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "dwt.h"
#include "gpio.h"
#include "maps_dock_lcm.h"
#include "maps_dock_lcm_lib.h"
#include "misc.h"
#include "oled.h"
#include "spi.h"
#include "system_init.h"

/*
**LCM_Put_Para
*/
//The 5-digit LCM parameter
int LCM_TenThousand=0;
int LCM_Thousand=0;
int LCM_Hundred=0;
int LCM_Ten=0;
int LCM_Single=0;
int LCM_Pot_Ten=0;
int LCM_Pot_Hundred=0;

/*
 *  @brief      LCM write command
 *  @since      v1.0
 *  Sample usage:       LCM_Write_CMD();//LCM write command
 */
static inline void LCM_Write_CMD(uint8 cmd)
{ 
  LCM_SET_DC_L;//DC low level, SPI write command
  
  SPI_Send(SPI_SPI2,SPI_PCS1,cmd);
}

/*
 *  @brief      LCM writes data
 *  @since      v1.0
 *  Sample usage:       LCM_Write_DATA();//LCM writes data
 */
static inline void LCM_Write_DATA(uint8 data)
{ 
  LCM_SET_DC_H;//DC high level, SPI writes data
  
  SPI_Send(SPI_SPI2,SPI_PCS1,data);
}

/*
 *  @brief      Fill one line with LCM
 *  @since      v1.0
 *  Sample usage:       LCM_Fill_Line();//Fill one line with LCM
 */
void MAPS_Dock_LCM_Fill_Line(uint8 Row,uint8 data)
{
  uint8 j=0;
  
  LCM_Write_CMD(0xB0|Row);    //Write Page
  LCM_Write_CMD(0x10);        //High Address
  LCM_Write_CMD(0x00);        //Low Address
  for(j=0;j<GUI_LCM_XMAX;j++)
  {
    LCM_Write_DATA(data);
  }
}

/*
 *  @brief      LCM filling
 *  @since      v1.0
 *  Sample usage:       LCM_Fill();//LCM filling
 */
void MAPS_Dock_LCM_Fill(uint8 data)
{
  uint8 i=0,j=0;
  
  for(i=0;i<GUI_LCM_PAGE;i++)
  {
    LCM_Write_CMD(0xB0|i);      //Write Page
    LCM_Write_CMD(0x10);        //High Address
    LCM_Write_CMD(0x00);        //Low Address
    for(j=0;j<GUI_LCM_XMAX;j++)
    {
      LCM_Write_DATA(data);
    }
  }
}

/*
 *  @brief      LCM screen clearing
 *  @since      v1.0
 *  Sample usage:       LCM_CLS();//LCM screen clearing
 */
void MAPS_Dock_LCM_CLS(void)
{
  uint8 i=0,j=0;
  
  for(i=0;i<GUI_LCM_PAGE;i++)
  {
    LCM_Write_CMD(0xB0|i);      //Write Page
    LCM_Write_CMD(0x10);        //High Address
    LCM_Write_CMD(0x00);        //Low Address
    for(j=0;j<GUI_LCM_XMAX;j++)
    {
      LCM_Write_DATA(0x00);
    }
  }
}

/*
 *  @brief      LCM reset
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Reset();
 */
void MAPS_Dock_LCM_Reset(void)
{
  DisableInterrupts;
  MAPS_Dock_LCM_Init();
  MAPS_Dock_LCM_CLS();
  EnableInterrupts;
}

/*
 *  @brief      MAPs_Dock_LCM initialization
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Init();//Initialize MAPs_LCM
 */
void MAPS_Dock_LCM_Init(void)
{
  
  //SPI initialization
  SPI_Init(SPI_SPI2,SPI_PCS1,MASTER,10000000);//Initialize SPI2, SCLK: PTD12, MOSI: PTD13, MISO: PTD14, CS1: PTD15, baud rate 10Mbps
  LCM_PORT_INIT_DC;//Initialize data command signal DC: PTD10
  
  //LCM initialization
  LCM_Write_CMD(0xAE);//Turn off display
  LCM_Delay(10);
  LCM_Write_CMD(0xE2);//LCM reset
  LCM_Delay(10);
  LCM_Write_CMD(0xA2);//Bias settings
  LCM_Delay(10);
  LCM_Write_CMD(0xA0);//Display settings, displayed from left to right
  LCM_Delay(10);
  LCM_Write_CMD(0xC8);//Display settings, displayed from top to bottom
  LCM_Delay(10);
  
  LCM_Write_CMD(0x2C);//Power boost setting
  LCM_Delay(50000);
  LCM_Write_CMD(0x2E);//Power calibration settings
  LCM_Delay(50000);
  LCM_Write_CMD(0x2F);//Voltage Follow Setting
  LCM_Delay(50000);
  
  LCM_Write_CMD(0x25);//Set voltage register
  LCM_Delay(10);
  LCM_Write_CMD(0x81);//Set voltage mode
  LCM_Delay(10);
  LCM_Write_CMD(0x20);//Set voltage register
  LCM_Delay(10);
  LCM_Write_CMD(0xB0);//Set page address
  LCM_Delay(10);
  LCM_Write_CMD(0x10);//Set column high-order address
  LCM_Delay(10);
  LCM_Write_CMD(0x00);//Set column low order address
  LCM_Delay(10);
  LCM_Write_CMD(0xAF);//Open Display
  LCM_Delay(10);
  
  LCM_Write_CMD(0xA5);//Open and display all
  LCM_Delay(10);
  LCM_Write_CMD(0xAE);//Turn off display
  LCM_Delay(10);
  LCM_Write_CMD(0xAF);//Open Display
  LCM_Delay(10);
  LCM_Write_CMD(0xA4);//Normal mode
  LCM_Delay(10);
  MAPS_Dock_LCM_CLS();
}

/*
 *  @brief      LCM sets coordinate points
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @since      v1.0
 *  Sample usage:       LCM_Set_Pos();//LCM sets coordinate points
 */
static inline void MAPS_Dock_LCM_Set_Pos(uint8 x,uint8 y)
{
  LCM_Write_CMD(0xB0|y);                //Write Page
  LCM_Write_CMD(0x10|((x&0xf0)>>4));    //High Address
  LCM_Write_CMD(0x00|(x&0x0f));         //Low address
}

/*
 *  @brief      LCM draws a coordinate point
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~63
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Pixel(0,0);
 */
void MAPS_Dock_LCM_Put_Pixel(uint8 x,uint8 y)
{
  uint8 data;
  
  MAPS_Dock_LCM_Set_Pos(x,y);
  data = 0x01<<(y%8);
  LCM_Write_CMD(0xB0|(y>>3));           //Write Page
  LCM_Write_CMD(0x10|((x&0xf0)>>4));    //High Address
  LCM_Write_CMD(0x00|(x&0x0f));         //Low address
  LCM_Write_DATA(data);                 //Write data
}

/*
 *  @brief      LCM draws 6x8 characters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @param      uint8   *ch     ch array
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Put_Str_6x8(0,0,"hello");
 */
void MAPS_Dock_LCM_Put_Str_6x8(uint8 x,uint8 y,uint8 *ch,LCM_Color_e LCM_Color_x)
{
  uint8 c=0,i=0,j=0;
  
  if(LCM_Color_x == LCM_Pure_Color)
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=6,j++)
    {
      c=*(ch+j)-32;
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(Oled_FontLib_6x8[c][i]);
      }
    }
  }
  else
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=6,j++)
    {
      c=*(ch+j)-32;
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_6x8[c][i]);
      }
    }
  }
  
}

/*
 *  @brief      LCM draws 8x16 characters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0,2,4,6
 *  @param      uint8   *ch     ch array
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Put_Str_8x16(0,0,"hello");
 */
void MAPS_Dock_LCM_Put_Str_8x16(uint8 x,uint8 y,uint8 *ch,LCM_Color_e LCM_Color_x)
{
  uint8 c=0,i=0,j=0;
  
  if(LCM_Color_x == LCM_Pure_Color)
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=8,j++)
    {
      c=*(ch+j)-32;
      
      if(x>119)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(Oled_FontLib_8x16[c*16+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(Oled_FontLib_8x16[c*16+i+8]);
      }
    }
  }
  else
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=8,j++)
    {
      c=*(ch+j)-32;
      
      if(x>119)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_8x16[c*16+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_8x16[c*16+i+8]);
      }
    }
  }
}

/*
 *  @brief      LCM draws 12x24 characters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0,4
 *  @param      uint8   *ch     ch array
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Put_Str_12x24(0,0,"hello");
 */
void MAPS_Dock_LCM_Put_Str_12x24(uint8 x,uint8 y,uint8 *ch,LCM_Color_e LCM_Color_x)
{
  uint8 c=0,i=0,j=0;
  
  if(LCM_Color_x == LCM_Pure_Color)
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=12,j++)
    {
      c=*(ch+j)-32;
      
      if(x>115)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<12;i++)
      {
        LCM_Write_DATA(Oled_FontLib_12x24[c*36+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<12;i++)
      {
        LCM_Write_DATA(Oled_FontLib_12x24[c*36+i+12]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+2);
      
      for(i=0;i<12;i++)
      {
        LCM_Write_DATA(Oled_FontLib_12x24[c*36+i+24]);
      }
    }
  }
  else
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=12,j++)
    {
      c=*(ch+j)-32;
      
      if(x>115)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<12;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_12x24[c*36+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<12;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_12x24[c*36+i+12]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+2);
      
      for(i=0;i<12;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_12x24[c*36+i+24]);
      }
    }
  }
}

/*
 *  @brief      LCM draws 128x64 static images
 *  @param      uint8   Picture[]    Picture array
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Draw_Picture_128x64(&Oled_Picture_128x64_Star[0][0]);
 */
void MAPS_Dock_LCM_Draw_Picture_128x64(const uint8 *Picture,LCM_Color_e LCM_Color_x)
{
  int i=0,j=0,k=0;
  uint32 Pixel=0;
  
  if(LCM_Color_x == LCM_Pure_Color)
  {
    for(i=0;i<8;i++)
    {
      MAPS_Dock_LCM_Set_Pos(0,i);
      
      for(j=0;j<16;j++)
      {
        for(k=7;k>=0;k--)
        {
          Pixel=(((*(Picture+(i*8+0)*16+j))&(0x01<<k))>>k)*(1<<0)
                +(((*(Picture+(i*8+1)*16+j))&(0x01<<k))>>k)*(1<<1)
                  +(((*(Picture+(i*8+2)*16+j))&(0x01<<k))>>k)*(1<<2)
                    +(((*(Picture+(i*8+3)*16+j))&(0x01<<k))>>k)*(1<<3)
                      +(((*(Picture+(i*8+4)*16+j))&(0x01<<k))>>k)*(1<<4)
                        +(((*(Picture+(i*8+5)*16+j))&(0x01<<k))>>k)*(1<<5)
                          +(((*(Picture+(i*8+6)*16+j))&(0x01<<k))>>k)*(1<<6)
                            +(((*(Picture+(i*8+7)*16+j))&(0x01<<k))>>k)*(1<<7);
          LCM_Write_DATA(Pixel);
        }
      }
    }
  }
  else
  {
    for(i=0;i<8;i++)
    {
      MAPS_Dock_LCM_Set_Pos(0,i);
      
      for(j=0;j<16;j++)
      {
        for(k=7;k>=0;k--)
        {
          Pixel=(((*(Picture+(i*8+0)*16+j))&(0x01<<k))>>k)*(1<<0)
                +(((*(Picture+(i*8+1)*16+j))&(0x01<<k))>>k)*(1<<1)
                  +(((*(Picture+(i*8+2)*16+j))&(0x01<<k))>>k)*(1<<2)
                    +(((*(Picture+(i*8+3)*16+j))&(0x01<<k))>>k)*(1<<3)
                      +(((*(Picture+(i*8+4)*16+j))&(0x01<<k))>>k)*(1<<4)
                        +(((*(Picture+(i*8+5)*16+j))&(0x01<<k))>>k)*(1<<5)
                          +(((*(Picture+(i*8+6)*16+j))&(0x01<<k))>>k)*(1<<6)
                            +(((*(Picture+(i*8+7)*16+j))&(0x01<<k))>>k)*(1<<7);
          LCM_Write_DATA(~Pixel);
        }
      }
    }
  }
}

/*
 *  @brief      LCM draws 6x8 integer parameters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @param      uint8   *ch     CH array, displaying parameter names
 *  @param      int     Value   Value integer parameter (Value is a positive number)
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Put_Para_6x8(0,0,"hello:",Value);
 */
void MAPS_Dock_LCM_Put_Para_6x8(uint8 x,uint8 y,uint8 *ch,int Value,LCM_Color_e LCM_Color_x)
{
  uint8 c=0,i=0,j=0;
  int LCM_Num[5]={0};
  
  LCM_TenThousand = Value / 10000;
  LCM_Thousand = (Value % 10000) / 1000;
  LCM_Hundred = (Value % 1000) / 100;
  LCM_Ten = (Value % 100) / 10;
  LCM_Single = Value % 10;
  
  if(LCM_TenThousand != 0)
  {
    LCM_Num[0] = LCM_TenThousand + 16;
    LCM_Num[1] = LCM_Thousand + 16;
    LCM_Num[2] = LCM_Hundred + 16;
    LCM_Num[3] = LCM_Ten + 16;
    LCM_Num[4] = LCM_Single + 16;
  }
  else if(LCM_Thousand != 0)
  {
    LCM_Num[0] = LCM_Thousand + 16;
    LCM_Num[1] = LCM_Hundred + 16;
    LCM_Num[2] = LCM_Ten + 16;
    LCM_Num[3] = LCM_Single + 16;
    LCM_Num[4] = 0;
  }
  else if(LCM_Hundred != 0)
  {
    LCM_Num[0] = LCM_Hundred + 16;
    LCM_Num[1] = LCM_Ten + 16;
    LCM_Num[2] = LCM_Single + 16;
    LCM_Num[3] = 0;
    LCM_Num[4] = 0;
  }
  else if(LCM_Ten != 0)
  {
    LCM_Num[0] = LCM_Ten + 16;
    LCM_Num[1] = LCM_Single + 16;
    LCM_Num[2] = 0;
    LCM_Num[3] = 0;
    LCM_Num[4] = 0;
  }
  else if(LCM_Single != 0)
  {
    LCM_Num[0] = LCM_Single + 16;
    LCM_Num[1] = 0;
    LCM_Num[2] = 0;
    LCM_Num[3] = 0;
    LCM_Num[4] = 0;
  }
  else
  {
    LCM_Num[0] = 16;
    LCM_Num[1] = 0;
    LCM_Num[2] = 0;
    LCM_Num[3] = 0;
    LCM_Num[4] = 0;
  }
  
  if(LCM_Color_x == LCM_Pure_Color)
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=6,j++)
    {
      c=*(ch+j)-32;
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(Oled_FontLib_6x8[c][i]);
      }
    }
    
    for(c=0,i=0,j=0;j<5;x+=6,j++)
    {
      c=LCM_Num[j];
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(Oled_FontLib_6x8[c][i]);
      }
    }
  }
  else
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=6,j++)
    {
      c=*(ch+j)-32;
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_6x8[c][i]);
      }
    }
    
    for(c=0,i=0,j=0;j<5;x+=6,j++)
    {
      c=LCM_Num[j];
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_6x8[c][i]);
      }
    }
  }
  
}

/*
 *  @brief      LCM draws 6x8 floating-point parameters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @param      uint8   *ch     CH array, displaying parameter names
 *  @param      float   Value   Value Floating Point Parameter (Value is a positive number)
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Put_Para_Pot_6x8(0,0,"hello:",Value);
 */
void MAPS_Dock_LCM_Put_Para_Pot_6x8(uint8 x,uint8 y,uint8 *ch,float Value,LCM_Color_e LCM_Color_x)
{
  uint8 c=0,i=0,j=0;
  int LCM_Num[5]={0};
  
  LCM_Ten = ((int)Value % 100) / 10;
  LCM_Single = (int)Value % 10;
  LCM_Pot_Ten = (int)(Value * 10) % 10;
  LCM_Pot_Hundred = (int)(Value * 100) % 10;
  
  if(LCM_Ten != 0)
  {
    LCM_Num[0] = LCM_Ten + 16;
    LCM_Num[1] = LCM_Single + 16;
    LCM_Num[2] = 14;
    LCM_Num[3] = LCM_Pot_Ten + 16;
    LCM_Num[4] = LCM_Pot_Hundred + 16;
  }
  else if(LCM_Single != 0)
  {
    LCM_Num[0] = LCM_Single + 16;
    LCM_Num[1] = 14;
    LCM_Num[2] = LCM_Pot_Ten + 16;
    LCM_Num[3] = LCM_Pot_Hundred + 16;
    LCM_Num[4] = 0;
  }
  else
  {
    LCM_Num[0] = 16;
    LCM_Num[1] = 14;
    LCM_Num[2] = LCM_Pot_Ten + 16;
    LCM_Num[3] = LCM_Pot_Hundred + 16;
    LCM_Num[4] = 0;
  }
  
  if(LCM_Color_x == LCM_Pure_Color)
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=6,j++)
    {
      c=*(ch+j)-32;
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(Oled_FontLib_6x8[c][i]);
      }
    }
    
    for(c=0,i=0,j=0;j<5;x+=6,j++)
    {
      c=LCM_Num[j];
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(Oled_FontLib_6x8[c][i]);
      }
    }
  }
  else
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=6,j++)
    {
      c=*(ch+j)-32;
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_6x8[c][i]);
      }
    }
    
    for(c=0,i=0,j=0;j<5;x+=6,j++)
    {
      c=LCM_Num[j];
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_6x8[c][i]);
      }
    }
  }
  
}

/*
 *  @brief      LCM draws 8x16 integer parameters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0,2,4,6
 *  @param      uint8   *ch     CH array, displaying parameter names
 *  @param      int     Value   Value integer parameter (Value is a positive number)
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Put_Para_8x16(0,0,"hello:",Value);
 */
void MAPS_Dock_LCM_Put_Para_8x16(uint8 x,uint8 y,uint8 *ch,int Value,LCM_Color_e LCM_Color_x)
{
  uint8 c=0,i=0,j=0;
  int LCM_Num[5]={0};
  
  LCM_TenThousand = Value / 10000;
  LCM_Thousand = (Value % 10000) / 1000;
  LCM_Hundred = (Value % 1000) / 100;
  LCM_Ten = (Value % 100) / 10;
  LCM_Single = Value % 10;
  
  if(LCM_TenThousand != 0)
  {
    LCM_Num[0] = LCM_TenThousand + 16;
    LCM_Num[1] = LCM_Thousand + 16;
    LCM_Num[2] = LCM_Hundred + 16;
    LCM_Num[3] = LCM_Ten + 16;
    LCM_Num[4] = LCM_Single + 16;
  }
  else if(LCM_Thousand != 0)
  {
    LCM_Num[0] = LCM_Thousand + 16;
    LCM_Num[1] = LCM_Hundred + 16;
    LCM_Num[2] = LCM_Ten + 16;
    LCM_Num[3] = LCM_Single + 16;
    LCM_Num[4] = 0;
  }
  else if(LCM_Hundred != 0)
  {
    LCM_Num[0] = LCM_Hundred + 16;
    LCM_Num[1] = LCM_Ten + 16;
    LCM_Num[2] = LCM_Single + 16;
    LCM_Num[3] = 0;
    LCM_Num[4] = 0;
  }
  else if(LCM_Ten != 0)
  {
    LCM_Num[0] = LCM_Ten + 16;
    LCM_Num[1] = LCM_Single + 16;
    LCM_Num[2] = 0;
    LCM_Num[3] = 0;
    LCM_Num[4] = 0;
  }
  else if(LCM_Single != 0)
  {
    LCM_Num[0] = LCM_Single + 16;
    LCM_Num[1] = 0;
    LCM_Num[2] = 0;
    LCM_Num[3] = 0;
    LCM_Num[4] = 0;
  }
  else
  {
    LCM_Num[0] = 16;
    LCM_Num[1] = 0;
    LCM_Num[2] = 0;
    LCM_Num[3] = 0;
    LCM_Num[4] = 0;
  }
  
  if(LCM_Color_x == LCM_Pure_Color)
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=8,j++)
    {
      c=*(ch+j)-32;
      
      if(x>119)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(Oled_FontLib_8x16[c*16+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(Oled_FontLib_8x16[c*16+i+8]);
      }
    }
    
    for(c=0,i=0,j=0;j<5;x+=8,j++)
    {
      c=LCM_Num[j];
      
      if(x>119)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(Oled_FontLib_8x16[c*16+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(Oled_FontLib_8x16[c*16+i+8]);
      }
    }    
  }
  else
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=8,j++)
    {
      c=*(ch+j)-32;
      
      if(x>119)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_8x16[c*16+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_8x16[c*16+i+8]);
      }
    }
    
    for(c=0,i=0,j=0;j<5;x+=8,j++)
    {
      c=LCM_Num[j];
      
      if(x>119)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_8x16[c*16+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_8x16[c*16+i+8]);
      }
    }
  }
  
}

/*
 *  @brief      LCM draws 8x16 floating-point parameters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0,2,4,6
 *  @param      uint8   *ch     CH array, displaying parameter names
 *  @param      float   Value   Value Floating Point Parameter (Value is a positive number)
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Put_Para_Pot_8x16(0,0,"hello:",Value);
 */
void MAPS_Dock_LCM_Put_Para_Pot_8x16(uint8 x,uint8 y,uint8 *ch,float Value,LCM_Color_e LCM_Color_x)
{
  uint8 c=0,i=0,j=0;
  int LCM_Num[5]={0};
  
  LCM_Ten = ((int)Value % 100) / 10;
  LCM_Single = (int)Value % 10;
  LCM_Pot_Ten = (int)(Value * 10) % 10;
  LCM_Pot_Hundred = (int)(Value * 100) % 10;
  
  if(LCM_Ten != 0)
  {
    LCM_Num[0] = LCM_Ten + 16;
    LCM_Num[1] = LCM_Single + 16;
    LCM_Num[2] = 14;
    LCM_Num[3] = LCM_Pot_Ten + 16;
    LCM_Num[4] = LCM_Pot_Hundred + 16;
  }
  else if(LCM_Single != 0)
  {
    LCM_Num[0] = LCM_Single + 16;
    LCM_Num[1] = 14;
    LCM_Num[2] = LCM_Pot_Ten + 16;
    LCM_Num[3] = LCM_Pot_Hundred + 16;
    LCM_Num[4] = 0;
  }
  else
  {
    LCM_Num[0] = 16;
    LCM_Num[1] = 14;
    LCM_Num[2] = LCM_Pot_Ten + 16;
    LCM_Num[3] = LCM_Pot_Hundred + 16;
    LCM_Num[4] = 0;
  }
  
  if(LCM_Color_x == LCM_Pure_Color)
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=8,j++)
    {
      c=*(ch+j)-32;
      
      if(x>119)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(Oled_FontLib_8x16[c*16+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(Oled_FontLib_8x16[c*16+i+8]);
      }
    }
    
    for(c=0,i=0,j=0;j<5;x+=8,j++)
    {
      c=LCM_Num[j];
      
      if(x>119)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(Oled_FontLib_8x16[c*16+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(Oled_FontLib_8x16[c*16+i+8]);
      }
    }
  }
  else
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=8,j++)
    {
      c=*(ch+j)-32;
      
      if(x>119)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_8x16[c*16+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_8x16[c*16+i+8]);
      }
    }
    
    for(c=0,i=0,j=0;j<5;x+=8,j++)
    {
      c=LCM_Num[j];
      
      if(x>119)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_8x16[c*16+i]);
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y+1);
      
      for(i=0;i<8;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_8x16[c*16+i+8]);
      }
    }
  }
  
}

/*
 *  @brief      LCM draws 6x8 character array parameters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @param      uint8   *ch     CH array, displaying parameter names
 *  @param      uint8   *buff   buff array
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_Put_Para_Char_6x8(0,0,"hello:",Value);
 */
void MAPS_Dock_LCM_Put_Para_Buff_6x8(uint8 x,uint8 y,uint8 *ch,uint8 *buff,LCM_Color_e LCM_Color_x)
{
  uint8 c=0,i=0,j=0;
  
  if(LCM_Color_x == LCM_Pure_Color)
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=6,j++)
    {
      c=*(ch+j)-32;
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(Oled_FontLib_6x8[c][i]);
      }
    }
    
    for(c=0,i=0,j=0;*(buff+j)!='\0';x+=6,j++)
    {
      c=*(buff+j)-32;
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(Oled_FontLib_6x8[c][i]);
      }
    }
  }
  else
  {
    for(c=0,i=0,j=0;*(ch+j)!='\0';x+=6,j++)
    {
      c=*(ch+j)-32;
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_6x8[c][i]);
      }
    }
    
    for(c=0,i=0,j=0;*(buff+j)!='\0';x+=6,j++)
    {
      c=*(buff+j)-32;
      
      if(x>121)
      {
        x=0;
        y++;
      }
      
      MAPS_Dock_LCM_Set_Pos(x,y);
      
      for(i=0;i<6;i++)
      {
        LCM_Write_DATA(~Oled_FontLib_6x8[c][i]);
      }
    }
  }
}

/*
 *  @brief      LCM drawing BMP image (8 * 128 array)
 *  @para       uint8   *Buff
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_LCM_DrawBMP();//LCM draws BMP images
 */
void MAPS_Dock_LCM_DrawBMP(const uint8 *Buff,LCM_Color_e LCM_Color_x)
{
  uint8 i=0,j=0;
  
  if(LCM_Color_x == LCM_Pure_Color)//Orthochromatic
  {
    for(i=0;i<GUI_LCM_PAGE;i++)
    {
      MAPS_Dock_LCM_Set_Pos(0,i);
      
      for(j=0;j<GUI_LCM_XMAX;j++)
      {
        LCM_Write_DATA(*Buff++);
      }
    }
  }
  else//Reverse color
  {
    for(i=0;i<GUI_LCM_PAGE;i++)
    {
      MAPS_Dock_LCM_Set_Pos(0,i);
      
      for(j=0;j<GUI_LCM_XMAX;j++)
      {
        LCM_Write_DATA(~*Buff++);
      }
    }
  }
}