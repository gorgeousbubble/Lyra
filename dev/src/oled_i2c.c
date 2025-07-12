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

/*
 *  @brief      Oled draws 6x8 time parameters (12-hour format)
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @param      uint8   ch[]    ch array, displaying parameter names
 *  @param      int     hour     hour integer parameter
 *  @param      int     minute   minute integer parameter
 *  @param      int     second   second integer parameter
 *  @since      v1.0
 *  Sample usage:       Oled_I2C_Put_Time_6x8_12(0,0,":",hour,minute,second);
 */
void Oled_I2C_Put_Time_6x8_12(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second,int isAM)
{
  uint8 c=0,i=0,j=0;
  int Hour_Num[2]={0};
  int Min_Num[2]={0};
  int Sec_Num[2]={0};
  
  // hour
  Oled_I2C_Ten = (hour % 100) / 10;
  Oled_I2C_Single = hour % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Hour_Num[0] = Oled_I2C_Ten + 16;
    Hour_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Hour_Num[0] = Oled_I2C_Ten + 16;
    Hour_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Hour_Num[0] = 16;
    Hour_Num[1] = 16;
  }
  
  // minute
  Oled_I2C_Ten = (minute % 100) / 10;
  Oled_I2C_Single = minute % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Min_Num[0] = Oled_I2C_Ten + 16;
    Min_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Min_Num[0] = Oled_I2C_Ten + 16;
    Min_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Min_Num[0] = 16;
    Min_Num[1] = 16;
  }
  
  // second
  Oled_I2C_Ten = (second % 100) / 10;
  Oled_I2C_Single = second % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Sec_Num[0] = Oled_I2C_Ten + 16;
    Sec_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Sec_Num[0] = Oled_I2C_Ten + 16;
    Sec_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Sec_Num[0] = 16;
    Sec_Num[1] = 16;
  }
  
  // hour
  for(c=0,i=0,j=0;j<2;x+=6,j++)
  {
    c=Hour_Num[j];
    
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
  
  // segment
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
  
  // minute
  for(c=0,i=0,j=0;j<2;x+=6,j++)
  {
    c=Min_Num[j];
    
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
  
  // segment
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
  
  // second
  for(c=0,i=0,j=0;j<2;x+=6,j++)
  {
    c=Sec_Num[j];
    
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

  // meridiem
  char meridiem[3] = {' ', ' ', '\0'};
  if(isAM)
  {
    meridiem[0] = 'A';
    meridiem[1] = 'M';
  }
  else
  {
    meridiem[0] = 'P';
    meridiem[1] = 'M';
  }

  for(c=0,i=0,j=0;meridiem[j]!='\0';x+=6,j++)
  {
    c=meridiem[j]-32;
    
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
 *  @brief      Oled draws 6x8 time parameters (24-hour format)
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @param      uint8   ch[]    ch array, displaying parameter names
 *  @param      int     hour     hour integer parameter
 *  @param      int     minute   minute integer parameter
 *  @param      int     second   second integer parameter
 *  @since      v1.0
 *  Sample usage:       Oled_I2C_Put_Time_6x8_24(0,0,":",hour,minute,second);
 */
void Oled_I2C_Put_Time_6x8_24(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second)
{
  uint8 c=0,i=0,j=0;
  int Hour_Num[2]={0};
  int Min_Num[2]={0};
  int Sec_Num[2]={0};
  
  // hour
  Oled_I2C_Ten = (hour % 100) / 10;
  Oled_I2C_Single = hour % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Hour_Num[0] = Oled_I2C_Ten + 16;
    Hour_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Hour_Num[0] = Oled_I2C_Ten + 16;
    Hour_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Hour_Num[0] = 16;
    Hour_Num[1] = 16;
  }
  
  // minute
  Oled_I2C_Ten = (minute % 100) / 10;
  Oled_I2C_Single = minute % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Min_Num[0] = Oled_I2C_Ten + 16;
    Min_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Min_Num[0] = Oled_I2C_Ten + 16;
    Min_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Min_Num[0] = 16;
    Min_Num[1] = 16;
  }
  
  // second
  Oled_I2C_Ten = (second % 100) / 10;
  Oled_I2C_Single = second % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Sec_Num[0] = Oled_I2C_Ten + 16;
    Sec_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Sec_Num[0] = Oled_I2C_Ten + 16;
    Sec_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Sec_Num[0] = 16;
    Sec_Num[1] = 16;
  }
  
  // hour
  for(c=0,i=0,j=0;j<2;x+=6,j++)
  {
    c=Hour_Num[j];
    
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
  
  // segment
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
  
  // minute
  for(c=0,i=0,j=0;j<2;x+=6,j++)
  {
    c=Min_Num[j];
    
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
  
  // segment
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
  
  // second
  for(c=0,i=0,j=0;j<2;x+=6,j++)
  {
    c=Sec_Num[j];
    
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
 *  @brief      Oled draws 8x16 time parameters (12-hour format)
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0,2,4,6
 *  @param      uint8   ch[]    ch array, displaying parameter names
 *  @param      int     hour     hour integer parameter
 *  @param      int     minute   minute integer parameter
 *  @param      int     second   second integer parameter
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Para_8x16(0,0,"hello:",Value);
 */
void Oled_I2C_Put_Time_8x16_12(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second,int isAM)
{
  uint8 c=0,i=0,j=0;
  int Hour_Num[2]={0};
  int Min_Num[2]={0};
  int Sec_Num[2]={0};
  
  // hour
  Oled_I2C_Ten = (hour % 100) / 10;
  Oled_I2C_Single = hour % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Hour_Num[0] = Oled_I2C_Ten + 16;
    Hour_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Hour_Num[0] = Oled_I2C_Ten + 16;
    Hour_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Hour_Num[0] = 16;
    Hour_Num[1] = 16;
  }

  // minute
  Oled_I2C_Ten = (minute % 100) / 10;
  Oled_I2C_Single = minute % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Min_Num[0] = Oled_I2C_Ten + 16;
    Min_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Min_Num[0] = Oled_I2C_Ten + 16;
    Min_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Min_Num[0] = 16;
    Min_Num[1] = 16;
  }

  // second
  Oled_I2C_Ten = (second % 100) / 10;
  Oled_I2C_Single = second % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Sec_Num[0] = Oled_I2C_Ten + 16;
    Sec_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Sec_Num[0] = Oled_I2C_Ten + 16;
    Sec_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Sec_Num[0] = 16;
    Sec_Num[1] = 16;
  }

  // hour
  for(c=0,i=0,j=0;j<2;x+=8,j++)
  {
    c=Hour_Num[j];
    
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
  
  // segment
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
  
  // minute
  for(c=0,i=0,j=0;j<2;x+=8,j++)
  {
    c=Min_Num[j];
    
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

  // segment
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

  // second
  for(c=0,i=0,j=0;j<2;x+=8,j++)
  {
    c=Sec_Num[j];
    
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

  // meridiem
  char meridiem[3] = {' ', ' ', '\0'};
  if(isAM)
  {
    meridiem[0] = 'A';
    meridiem[1] = 'M';
  }
  else
  {
    meridiem[0] = 'P';
    meridiem[1] = 'M';
  }

  // segment
  for(c=0,i=0,j=0;meridiem[j]!='\0';x+=8,j++)
  {
    c=meridiem[j]-32;
    
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
 *  @brief      Oled draws 8x16 time parameters (24-hour format)
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0,2,4,6
 *  @param      uint8   ch[]    ch array, displaying parameter names
 *  @param      int     hour     hour integer parameter
 *  @param      int     minute   minute integer parameter
 *  @param      int     second   second integer parameter
 *  @since      v1.0
 *  Sample usage:       Oled_Put_Para_8x16(0,0,"hello:",Value);
 */
void Oled_I2C_Put_Time_8x16_24(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second)
{
  uint8 c=0,i=0,j=0;
  int Hour_Num[2]={0};
  int Min_Num[2]={0};
  int Sec_Num[2]={0};
  
  // hour
  Oled_I2C_Ten = (hour % 100) / 10;
  Oled_I2C_Single = hour % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Hour_Num[0] = Oled_I2C_Ten + 16;
    Hour_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Hour_Num[0] = Oled_I2C_Ten + 16;
    Hour_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Hour_Num[0] = 16;
    Hour_Num[1] = 16;
  }

  // minute
  Oled_I2C_Ten = (minute % 100) / 10;
  Oled_I2C_Single = minute % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Min_Num[0] = Oled_I2C_Ten + 16;
    Min_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Min_Num[0] = Oled_I2C_Ten + 16;
    Min_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Min_Num[0] = 16;
    Min_Num[1] = 16;
  }

  // second
  Oled_I2C_Ten = (second % 100) / 10;
  Oled_I2C_Single = second % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Sec_Num[0] = Oled_I2C_Ten + 16;
    Sec_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Sec_Num[0] = Oled_I2C_Ten + 16;
    Sec_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Sec_Num[0] = 16;
    Sec_Num[1] = 16;
  }

  // hour
  for(c=0,i=0,j=0;j<2;x+=8,j++)
  {
    c=Hour_Num[j];
    
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
  
  // segment
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
  
  // minute
  for(c=0,i=0,j=0;j<2;x+=8,j++)
  {
    c=Min_Num[j];
    
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

  // segment
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

  // second
  for(c=0,i=0,j=0;j<2;x+=8,j++)
  {
    c=Sec_Num[j];
    
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
 *  @brief      Oled draws 6x8 data parameters
 *  @param      uint8   x       x scope is 0~127
 *  @param      uint8   y       y scope is 0~7
 *  @param      uint8   ch[]    ch array, displaying parameter names
 *  @param      int     hour     hour integer parameter
 *  @param      int     minute   minute integer parameter
 *  @param      int     second   second integer parameter
 *  @since      v1.0
 *  Sample usage:       Oled_I2C_Put_Date_8x16_12(0,0,":",hour,minute,second);
 */
void Oled_I2C_Put_Date_6x8(uint8 x,uint8 y,uint8 ch[],int year,int month,int day)
{
  uint8 c=0,i=0,j=0;
  int Year_Num[4]={0};
  int Month_Num[2]={0};
  int Day_Num[2]={0};
  
  // year
  Oled_I2C_Thousand = (year % 10000) / 1000;
  Oled_I2C_Hundred = (year % 1000) / 100;
  Oled_I2C_Ten = (year % 100) / 10;
  Oled_I2C_Single = year % 10;
    
  if(Oled_I2C_Thousand != 0)
  {
    Year_Num[0] = Oled_I2C_Thousand + 16;
    Year_Num[1] = Oled_I2C_Hundred + 16;
    Year_Num[2] = Oled_I2C_Ten + 16;
    Year_Num[3] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Hundred != 0)
  {
    Year_Num[0] = Oled_I2C_Thousand + 16;
    Year_Num[1] = Oled_I2C_Hundred + 16;
    Year_Num[2] = Oled_I2C_Ten + 16;
    Year_Num[3] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Ten != 0)
  {
    Year_Num[0] = Oled_I2C_Thousand + 16;
    Year_Num[1] = Oled_I2C_Hundred + 16;
    Year_Num[2] = Oled_I2C_Ten + 16;
    Year_Num[3] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Year_Num[0] = Oled_I2C_Thousand + 16;
    Year_Num[1] = Oled_I2C_Hundred + 16;
    Year_Num[2] = Oled_I2C_Ten + 16;
    Year_Num[3] = Oled_I2C_Single + 16;
  }
  else
  {
    Year_Num[0] = 16;
    Year_Num[1] = 16;
    Year_Num[2] = 16;
    Year_Num[3] = 16;
  }
  
  // month
  Oled_I2C_Ten = (month % 100) / 10;
  Oled_I2C_Single = month % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Month_Num[0] = Oled_I2C_Ten + 16;
    Month_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Month_Num[0] = Oled_I2C_Ten + 16;
    Month_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Month_Num[0] = 16;
    Month_Num[1] = 16;
  }
  
  // day
  Oled_I2C_Ten = (day % 100) / 10;
  Oled_I2C_Single = day % 10;
    
  if(Oled_I2C_Ten != 0)
  {
    Day_Num[0] = Oled_I2C_Ten + 16;
    Day_Num[1] = Oled_I2C_Single + 16;
  }
  else if(Oled_I2C_Single != 0)
  {
    Day_Num[0] = Oled_I2C_Ten + 16;
    Day_Num[1] = Oled_I2C_Single + 16;
  }
  else
  {
    Day_Num[0] = 16;
    Day_Num[1] = 16;
  }
  
  // hour
  for(c=0,i=0,j=0;j<4;x+=6,j++)
  {
    c=Year_Num[j];
    
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
  
  // segment
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
  
  // minute
  for(c=0,i=0,j=0;j<2;x+=6,j++)
  {
    c=Month_Num[j];
    
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
  
  // segment
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
  
  // second
  for(c=0,i=0,j=0;j<2;x+=6,j++)
  {
    c=Day_Num[j];
    
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