/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       oled_i2c.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "delay.h"
#include "gpio.h"
#include "i2c.h"
#include "misc.h"
#include "oled_i2c.h"
#include "port.h"

//SSD1306
/*
**Oled_Put_Para
*/
//The 5-digit Oled parameter
int Oled_I2C_TenThousand=0;
int Oled_I2C_Thousand=0;
int Oled_I2C_Hundred=0;
int Oled_I2C_Ten=0;
int Oled_I2C_Single=0;
int Oled_I2C_Pot_Ten=0;
int Oled_I2C_Pot_Hundred=0;

/*
**Oled I2C start signal
*/
void Oled_I2C_GPIO_Start(void)
{
  OLED_I2C_DDR_OUT_SDA;      //SDA output
  OLED_I2C_SET_SDA_H;        //SDA high level
  OLED_I2C_SET_SCL_H;        //SCL high level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  OLED_I2C_SET_SDA_L;        //SDA low level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  OLED_I2C_SET_SCL_L;        //SCL low level
}

/*
**Oled I2C stop signal
*/
void Oled_I2C_GPIO_Stop(void)
{
  OLED_I2C_DDR_OUT_SDA;      //SDA output
  OLED_I2C_SET_SDA_L;        //SDA low level
  OLED_I2C_SET_SCL_H;        //SCL high level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  OLED_I2C_SET_SDA_H;        //SDA high level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
}

/*
**Oled I2C sends response signal
*/
void Oled_I2C_GPIO_Send_Ack(int Ack)
{
  OLED_I2C_DDR_OUT_SDA;      //SDA output
  if(Ack == 0)
  {
    OLED_I2C_SET_SDA_L;      //SDA low level
  }
  else
  {
    OLED_I2C_SET_SDA_H;      //SDA high level
  }
  OLED_I2C_SET_SCL_H;        //SCL high level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  OLED_I2C_SET_SCL_L;        //SCL low level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
}

/*
**Oled I2C receiving response signal
*/
uint8 Oled_I2C_GPIO_Recv_Ack(void)
{
  uint8 Recv=0;
  
  OLED_I2C_DDR_IN_SDA;       //SDA input
  OLED_I2C_SET_SCL_H;        //SCL high level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  Recv = OLED_I2C_GET_SDA_IN;//Read SDA signal
  OLED_I2C_SET_SCL_L;        //SCL low level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  return Recv;
}

/*
**Oled I2C sends one byte of data
*/
uint8 Oled_I2C_GPIO_Send_Byte(uint8 I2C_Data)
{
  uint8 i=0;
  uint8 Ack=0;
  
  OLED_I2C_DDR_OUT_SDA;      //SDA output
  for(i=0;i<8;i++)
  {
    if((I2C_Data<<i)&0x80)
    {
      OLED_I2C_SET_SDA_H;    //SDA high level
    }
    else
    {
      OLED_I2C_SET_SDA_L;    //SDA low level
    }
    OLED_I2C_DELAY_TIME;     //I2C delay of 10 Nop
    OLED_I2C_SET_SCL_H;      //SCL high level
    OLED_I2C_DELAY_TIME;     //I2C delay of 10 Nop
    OLED_I2C_SET_SCL_L;      //SCL low level
    OLED_I2C_DELAY_TIME;     //I2C delay of 10 Nop
  }
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  OLED_I2C_SET_SDA_H;        //SDA high level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  OLED_I2C_SET_SCL_H;        //SCL high level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  OLED_I2C_DDR_IN_SDA;       //SDA input
  if(OLED_I2C_GET_SDA_IN == 1)
  {
    Ack = 0;
  }
  else
  {
    Ack = 1;
  }
  OLED_I2C_SET_SCL_L;        //SCL low level
  OLED_I2C_DELAY_TIME;       //I2C delay of 10 Nop
  return Ack;
}

/*
**Oled I2C Receive one byte of data
*/
uint8 Oled_I2C_GPIO_Recv_Byte(void)
{
  uint8 i=0;
  uint8 Data=0;
  
  OLED_I2C_DDR_OUT_SDA;      //SDA output
  OLED_I2C_SET_SDA_H;        //SDA high level
  OLED_I2C_DDR_IN_SDA;       //SDA input
  for(i=0;i<8;i++)
  {
    Data<<=1;
    OLED_I2C_SET_SCL_H;      //SCL high level
    OLED_I2C_DELAY_TIME;     //I2C delay of 10 Nop
    Data|=OLED_I2C_GET_SDA_IN;
    OLED_I2C_SET_SCL_L;      //SCL low level
    OLED_I2C_DELAY_TIME;     //I2C delay of 10 Nop
  }
  return Data;
}

/*
 *  @brief      Oled Write data
 *  @param      uint8   data
 *  @since      v1.0
 *  Sample usage:       Oled_I2C_WrDat(0xAA);
 */
void Oled_I2C_WrDat(uint8 data)
{
  Oled_I2C_GPIO_Start();
  Oled_I2C_GPIO_Send_Byte(0x78);
  Oled_I2C_GPIO_Send_Byte(0x40);
  Oled_I2C_GPIO_Send_Byte(data);
  Oled_I2C_GPIO_Stop();
}

/*
 *  @brief      Oled Write command
 *  @param      uint8   cmd
 *  @since      v1.0
 *  Sample usage:       Oled_I2C_WrCmd(0xAA);
 */
void Oled_I2C_WrCmd(uint8 cmd)
{
  Oled_I2C_GPIO_Start();
  Oled_I2C_GPIO_Send_Byte(0x78);
  Oled_I2C_GPIO_Send_Byte(0x00);
  Oled_I2C_GPIO_Send_Byte(cmd);
  Oled_I2C_GPIO_Stop();
}

/*
 *  @brief      Oled Set coordinate points
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~63
 *  @since      v1.0
 *  Sample usage:       Oled_I2C_Set_Pos(64,32);
 */
void Oled_I2C_Set_Pos(uint8 x,uint8 y)
{
  Oled_I2C_WrCmd(0xb0+y);
  Oled_I2C_WrCmd(((x&0xf0)>>4)|0x10);
  Oled_I2C_WrCmd((x&0x0f)|0x01);
}

/*
 *  @brief      Oled full screen display
 *  @param      uint8   data
 *  @since      v1.0
 *  Sample usage:       Oled_I2C_Fill(0xff);
 */
void Oled_I2C_Fill(uint8 data)
{
  uint8 i,j;
  
  for(j=0;j<8;j++)
  {
    Oled_I2C_WrCmd(0xb0+j);
    Oled_I2C_WrCmd(0x01);
    Oled_I2C_WrCmd(0x10);
    for(i=0;i<OLED_I2C_X_WIDTH;i++)
    {
      Oled_I2C_WrDat(data);
    }
  }
}

/*
 *  @brief      Oled screen cleaning
 *  @since      v1.0
 *  Sample usage:       Oled_I2C_Clean();
 */
void Oled_I2C_Clean(void)
{
  uint8 i,j;
  
  for(j=0;j<8;j++)
  {
    Oled_I2C_WrCmd(0xb0+j);
    Oled_I2C_WrCmd(0x01);
    Oled_I2C_WrCmd(0x10);
    for(i=0;i<OLED_I2C_X_WIDTH;i++)
    {
      Oled_I2C_WrDat(0);
    }
  }
}

/*
 *  @brief      Oled reset
 *  @since      v1.0
 *  Sample usage:       Oled_I2C_Reset();
 */
void Oled_I2C_Reset(void)
{
  DisableInterrupts;
  Oled_I2C_Init();
  Oled_I2C_Clean();
  EnableInterrupts;
}

/*
 *  @brief      Oled initialization
 *  @since      v1.0
 *  Sample usage:       Oled_I2C_Init();
 */
void Oled_I2C_Init(void)
{
  OLED_I2C_PORT_INIT_SCL;
  OLED_I2C_PORT_INIT_SDA;
  
  DELAY_us(50);
  
  Oled_I2C_WrCmd(0xae);
  Oled_I2C_WrCmd(0x20);
  Oled_I2C_WrCmd(0x00);
  Oled_I2C_WrCmd(0xb0);
  Oled_I2C_WrCmd(0xc8);
  Oled_I2C_WrCmd(0x00);
  Oled_I2C_WrCmd(0x10);
  Oled_I2C_WrCmd(0x40);
  Oled_I2C_WrCmd(0x81);
  Oled_I2C_WrCmd(0xff);
  Oled_I2C_WrCmd(0xa1);
  Oled_I2C_WrCmd(0xa6);
  Oled_I2C_WrCmd(0xa8);
  Oled_I2C_WrCmd(0x3f);
  Oled_I2C_WrCmd(0xd3);
  Oled_I2C_WrCmd(0x00);
  Oled_I2C_WrCmd(0xd5);
  Oled_I2C_WrCmd(0xf0);
  Oled_I2C_WrCmd(0xd9);
  Oled_I2C_WrCmd(0x22);
  Oled_I2C_WrCmd(0xda);
  Oled_I2C_WrCmd(0x12);
  Oled_I2C_WrCmd(0xdb);
  Oled_I2C_WrCmd(0x20);
  Oled_I2C_WrCmd(0x8d);
  Oled_I2C_WrCmd(0x14);
  Oled_I2C_WrCmd(0xaf);
  Oled_I2C_Clean();
  Oled_I2C_Set_Pos(0,0);
}

/*
 *  @brief      Oled draw a coordinate point
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~63
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Pixel(0,0);
 */
void Oled_I2C_Put_Pixel(uint8 x,uint8 y)
{
  uint8 data;
  
  Oled_I2C_Set_Pos(x,y);
  
  data = 0x01<<(y%8); 	
  Oled_I2C_WrCmd(0xb0+(y>>3));
  Oled_I2C_WrCmd(((x&0xf0)>>4)|0x10);
  Oled_I2C_WrCmd((x&0x0f)|0x00);
  Oled_I2C_WrDat(data);
}

/*
 *  @brief      Oled draws 6x8 characters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @param      uint8   ch[]    ch array
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Str_6x8(0,0,"hello");
 */
void Oled_I2C_Put_Str_6x8(uint8 x,uint8 y,uint8 ch[])
{
  uint8 c=0,i=0,j=0;
  
  for(c=0,i=0,j=0;ch[j]!='\0';x+=6,j++)
  {
    c=ch[j]-32;
    
    if(x>121)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<6;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_6x8[c][i]);
    }
  }
}

/*
 *  @brief      Oled draws 8x16 characters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0,2,4,6
 *  @param      uint8   ch[]    ch array
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Str_8x16(0,0,"hello");
 */
void Oled_I2C_Put_Str_8x16(uint8 x,uint8 y,uint8 ch[])
{
  uint8 c=0,i=0,j=0;
  
  while(ch[j]!='\0')
  {
    c=ch[j]-32;
    
    if(x>119)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<8;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_8x16[c*16+i]);
    }
    
    Oled_I2C_Set_Pos(x,y+1);
    
    for(i=0;i<8;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_8x16[c*16+i+8]);
    }
    
    x+=8;
    j++;
  }
  
}

/*
 *  @brief      Oled draws 12x24 characters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0,4
 *  @param      uint8   ch[]    ch array
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Str_12x24(0,0,"hello");
 */
void Oled_I2C_Put_Str_12x24(uint8 x,uint8 y,uint8 ch[])
{
  uint8 c=0,i=0,j=0;
  
  while(ch[j]!='\0')
  {
    c=ch[j]-32;
    
    if(x>115)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<12;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_12x24[c*36+i]);
    }
    
    Oled_I2C_Set_Pos(x,y+1);
    
    for(i=0;i<12;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_12x24[c*36+i+12]);
    }
    
    Oled_I2C_Set_Pos(x,y+2);
    
    for(i=0;i<12;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_12x24[c*36+i+24]);
    }

    x+=12;
    j++;
  }
}

/*
 *  @brief      Oled draws 128x64 static images
 *  @param      uint8   Picture[]    Picture array
 *  @since      v1.0
 *  Sample usage:       Oled_Draw_Picture_128x64(&Oled_Picture_128x64_Star[0][0]);
 */
void Oled_I2C_Draw_Picture_128x64(const uint8 *Picture)
{
  int i=0,j=0,k=0;
  uint32 Pixel=0;
  
  for(i=0;i<8;i++)
  {
    Oled_I2C_Set_Pos(0,i);
    
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
        Oled_I2C_WrDat(Pixel);
      }
    }
  }
}

/*
 *  @brief      Oled draws 6x8 integer parameters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @param      uint8   ch[]    ch array, displaying parameter names
 *  @param      int     Value   Value integer parameter (Value is a positive number)
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Para_6x8(0,0,"hello:",Value);
 */
void Oled_I2C_Put_Para_6x8(uint8 x,uint8 y,uint8 ch[],int Value)
{
  uint8 c=0,i=0,j=0;
  int Oled_Num[5]={0};
  
  Oled_I2C_TenThousand = Value / 10000;
  Oled_I2C_Thousand = (Value % 10000) / 1000;
  Oled_I2C_Hundred = (Value % 1000) / 100;
  Oled_I2C_Ten = (Value % 100) / 10;
  Oled_I2C_Single = Value % 10;
    
  if(Oled_I2C_TenThousand != 0)
  {
    Oled_Num[0] = Oled_I2C_TenThousand + 16;
    Oled_Num[1] = Oled_I2C_Thousand + 16;
    Oled_Num[2] = Oled_I2C_Hundred + 16;
    Oled_Num[3] = Oled_I2C_Ten + 16;
    Oled_Num[4] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Thousand != 0)
  {
    Oled_Num[0] = Oled_I2C_Thousand + 16;
    Oled_Num[1] = Oled_I2C_Hundred + 16;
    Oled_Num[2] = Oled_I2C_Ten + 16;
    Oled_Num[3] = Oled_I2C_Single + 16;
    Oled_Num[4] = 0;
  }
  else if(Oled_I2C_Hundred != 0)
  {
    Oled_Num[0] = Oled_I2C_Hundred + 16;
    Oled_Num[1] = Oled_I2C_Ten + 16;
    Oled_Num[2] = Oled_I2C_Single + 16;
    Oled_Num[3] = 0;
    Oled_Num[4] = 0;
  }
  else if(Oled_I2C_Ten != 0)
  {
    Oled_Num[0] = Oled_I2C_Ten + 16;
    Oled_Num[1] = Oled_I2C_Single + 16;
    Oled_Num[2] = 0;
    Oled_Num[3] = 0;
    Oled_Num[4] = 0;
  }
  else if(Oled_I2C_Single != 0)
  {
    Oled_Num[0] = Oled_I2C_Single + 16;
    Oled_Num[1] = 0;
    Oled_Num[2] = 0;
    Oled_Num[3] = 0;
    Oled_Num[4] = 0;
  }
  else
  {
    Oled_Num[0] = 16;
    Oled_Num[1] = 0;
    Oled_Num[2] = 0;
    Oled_Num[3] = 0;
    Oled_Num[4] = 0;
  }
  
  for(c=0,i=0,j=0;ch[j]!='\0';x+=6,j++)
  {
    c=ch[j]-32;
    
    if(x>121)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<6;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_6x8[c][i]);
    }
  }
  
  for(c=0,i=0,j=0;j<5;x+=6,j++)
  {
    c=Oled_Num[j];
    
    if(x>121)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<6;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_6x8[c][i]);
    }
  }
  
}

/*
 *  @brief      Oled draws 6x8 floating-point parameters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @param      uint8   ch[]    ch array, displaying parameter names
 *  @param      float   Value   Value integer parameter (Value is a positive number)
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Para_Pot_6x8(0,0,"hello:",Value);
 */
void Oled_I2C_Put_Para_Pot_6x8(uint8 x,uint8 y,uint8 ch[],float Value)
{
  uint8 c=0,i=0,j=0;
  int Oled_Num[5]={0};
  
  Oled_I2C_Ten = ((int)Value % 100) / 10;
  Oled_I2C_Single = (int)Value % 10;
  Oled_I2C_Pot_Ten = (int)(Value * 10) % 10;
  Oled_I2C_Pot_Hundred = (int)(Value * 100) % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Oled_Num[0] = Oled_I2C_Ten + 16;
    Oled_Num[1] = Oled_I2C_Single + 16;
    Oled_Num[2] = 14;
    Oled_Num[3] = Oled_I2C_Pot_Ten + 16;
    Oled_Num[4] = Oled_I2C_Pot_Hundred + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Oled_Num[0] = Oled_I2C_Single + 16;
    Oled_Num[1] = 14;
    Oled_Num[2] = Oled_I2C_Pot_Ten + 16;
    Oled_Num[3] = Oled_I2C_Pot_Hundred + 16;
    Oled_Num[4] = 0;
  }
  else
  {
    Oled_Num[0] = 16;
    Oled_Num[1] = 14;
    Oled_Num[2] = Oled_I2C_Pot_Ten + 16;
    Oled_Num[3] = Oled_I2C_Pot_Hundred + 16;
    Oled_Num[4] = 0;
  }
  
  for(c=0,i=0,j=0;ch[j]!='\0';x+=6,j++)
  {
    c=ch[j]-32;
    
    if(x>121)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<6;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_6x8[c][i]);
    }
  }
  
  for(c=0,i=0,j=0;j<5;x+=6,j++)
  {
    c=Oled_Num[j];
    
    if(x>121)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<6;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_6x8[c][i]);
    }
  }
  
}

/*
 *  @brief      Oled draws 8x16 integer parameters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0,2,4,6
 *  @param      uint8   ch[]    ch array, displaying parameter names
 *  @param      int     Value   Value integer parameter (Value is a positive number)
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Para_8x16(0,0,"hello:",Value);
 */
void Oled_I2C_Put_Para_8x16(uint8 x,uint8 y,uint8 ch[],int Value)
{
  uint8 c=0,i=0,j=0;
  int Oled_Num[5]={0};
  
  Oled_I2C_TenThousand = Value / 10000;
  Oled_I2C_Thousand = (Value % 10000) / 1000;
  Oled_I2C_Hundred = (Value % 1000) / 100;
  Oled_I2C_Ten = (Value % 100) / 10;
  Oled_I2C_Single = Value % 10;
    
  if(Oled_I2C_TenThousand != 0)
  {
    Oled_Num[0] = Oled_I2C_TenThousand + 16;
    Oled_Num[1] = Oled_I2C_Thousand + 16;
    Oled_Num[2] = Oled_I2C_Hundred + 16;
    Oled_Num[3] = Oled_I2C_Ten + 16;
    Oled_Num[4] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Thousand != 0)
  {
    Oled_Num[0] = Oled_I2C_Thousand + 16;
    Oled_Num[1] = Oled_I2C_Hundred + 16;
    Oled_Num[2] = Oled_I2C_Ten + 16;
    Oled_Num[3] = Oled_I2C_Single + 16;
    Oled_Num[4] = 0;
  }
  else if(Oled_I2C_Hundred != 0)
  {
    Oled_Num[0] = Oled_I2C_Hundred + 16;
    Oled_Num[1] = Oled_I2C_Ten + 16;
    Oled_Num[2] = Oled_I2C_Single + 16;
    Oled_Num[3] = 0;
    Oled_Num[4] = 0;
  }
  else if(Oled_I2C_Ten != 0)
  {
    Oled_Num[0] = Oled_I2C_Ten + 16;
    Oled_Num[1] = Oled_I2C_Single + 16;
    Oled_Num[2] = 0;
    Oled_Num[3] = 0;
    Oled_Num[4] = 0;
  }
  else if(Oled_I2C_Single != 0)
  {
    Oled_Num[0] = Oled_I2C_Single + 16;
    Oled_Num[1] = 0;
    Oled_Num[2] = 0;
    Oled_Num[3] = 0;
    Oled_Num[4] = 0;
  }
  else
  {
    Oled_Num[0] = 16;
    Oled_Num[1] = 0;
    Oled_Num[2] = 0;
    Oled_Num[3] = 0;
    Oled_Num[4] = 0;
  }
  
  for(c=0,i=0,j=0;ch[j]!='\0';x+=8,j++)
  {
    c=ch[j]-32;
    
    if(x>119)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<8;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_8x16[c*16+i]);
    }
    
    Oled_I2C_Set_Pos(x,y+1);
    
    for(i=0;i<8;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_8x16[c*16+i+8]);
    }
  }
  
  for(c=0,i=0,j=0;j<5;x+=8,j++)
  {
    c=Oled_Num[j];
    
    if(x>119)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<8;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_8x16[c*16+i]);
    }
    
    Oled_I2C_Set_Pos(x,y+1);
    
    for(i=0;i<8;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_8x16[c*16+i+8]);
    }
  }
  
}

/*
 *  @brief      Oled draws 8x16 floating-point parameters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0,2,4,6
 *  @param      uint8   ch[]    ch array, displaying parameter names
 *  @param      float   Value   Value integer parameter (Value is a positive number)
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Para_Pot_8x16(0,0,"hello:",Value);
 */
void Oled_I2C_Put_Para_Pot_8x16(uint8 x,uint8 y,uint8 ch[],float Value)
{
  uint8 c=0,i=0,j=0;
  int Oled_Num[5]={0};
  
  Oled_I2C_Ten = ((int)Value % 100) / 10;
  Oled_I2C_Single = (int)Value % 10;
  Oled_I2C_Pot_Ten = (int)(Value * 10) % 10;
  Oled_I2C_Pot_Hundred = (int)(Value * 100) % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Oled_Num[0] = Oled_I2C_Ten + 16;
    Oled_Num[1] = Oled_I2C_Single + 16;
    Oled_Num[2] = 14;
    Oled_Num[3] = Oled_I2C_Pot_Ten + 16;
    Oled_Num[4] = Oled_I2C_Pot_Hundred + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Oled_Num[0] = Oled_I2C_Single + 16;
    Oled_Num[1] = 14;
    Oled_Num[2] = Oled_I2C_Pot_Ten + 16;
    Oled_Num[3] = Oled_I2C_Pot_Hundred + 16;
    Oled_Num[4] = 0;
  }
  else
  {
    Oled_Num[0] = 16;
    Oled_Num[1] = 14;
    Oled_Num[2] = Oled_I2C_Pot_Ten + 16;
    Oled_Num[3] = Oled_I2C_Pot_Hundred + 16;
    Oled_Num[4] = 0;
  }
  
  for(c=0,i=0,j=0;ch[j]!='\0';x+=8,j++)
  {
    c=ch[j]-32;
    
    if(x>119)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<8;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_8x16[c*16+i]);
    }
    
    Oled_I2C_Set_Pos(x,y+1);
    
    for(i=0;i<8;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_8x16[c*16+i+8]);
    }
  }
  
  for(c=0,i=0,j=0;j<5;x+=8,j++)
  {
    c=Oled_Num[j];
    
    if(x>119)
    {
      x=0;
      y++;
    }
    
    Oled_I2C_Set_Pos(x,y);
    
    for(i=0;i<8;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_8x16[c*16+i]);
    }
    
    Oled_I2C_Set_Pos(x,y+1);
    
    for(i=0;i<8;i++)
    {
      Oled_I2C_WrDat(Oled_FontLib_8x16[c*16+i+8]);
    }
  }
}