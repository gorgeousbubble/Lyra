/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_w25q80.c
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "dwt.h"
#include "maps_dock_w25q80.h"
#include "spi.h"

/*
 *  @brief      W25Q80 SPI transfer
 *  @since      v1.0
 *  Sample usage:       W25Q80_Transfer()
 */
static inline void W25Q80_Transfer(uint8 *SPI_MO_Data,uint8 *SPI_MI_Data,uint32 SPI_Len)
{
  SPI_MOSI(SPI_SPI2,SPI_PCS0,&SPI_MO_Data[0],&SPI_MI_Data[0],SPI_Len);
}

/*
 *  @brief      W25Q80 SPI transfer command
 *  @since      v1.0
 *  Sample usage:       W25Q80_TransferCMD()
 */
static inline void W25Q80_TransferCMD(uint8 *SPI_MO_CMD,uint8 *SPI_MI_CMD,uint8 *SPI_MO_Data,uint8 *SPI_MI_Data,uint32 SPI_CMD_Len,uint32 SPI_Len)
{
  SPI_MOSI_CMD(SPI_SPI2,SPI_PCS0,&SPI_MO_CMD[0],&SPI_MI_CMD[0],&SPI_MO_Data[0],&SPI_MI_Data[0],SPI_CMD_Len,SPI_Len);
}

/*
 *  @brief      W25Q80 SPI write enable
 *  @since      v1.0
 *  Sample usage:       W25Q80_Write_Enable()
 */
static inline void W25Q80_Write_Enable(void)
{
  uint8 CMD=W25Q80_CMD_WRITE_ENABLE;
  
  W25Q80_Transfer(&CMD,NULL,1);
}

/*
 *  @brief      W25Q80 check chip Busy
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_W25Q80_Check();//W25Q80 check chip Busy
 */
static inline void MAPS_Dock_W25Q80_Check(uint32 TimeOut,uint8 CMD)
{
  uint32 i=0;
  uint8 Status=0;
  
  W25Q80_Transfer(&CMD,NULL,1);
  
  for(i=0;i<TimeOut;i++)
  {
    W25Q80_Transfer(NULL,&Status,1);
    if((Status & STATUS_BUSY) == 0)
    {
      break;
    }
  }
}

/*
 *  @brief      W25Q80 initialization SPI chip selection signal (shared with LCM SPI2)
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_W25Q80_Init();//W25Q80 initialization
 */
void MAPS_Dock_W25Q80_Init(void)
{
  SPI_Extra_PCS_Init(SPI_SPI2,SPI_PCS0);
  //SPI_Init(SPI_SPI2,SPI_PCS0,MASTER,10000000);
}

/*
 *  @brief      W25Q80 read chip ID
 *  @para       uint8   Vender_ID       Manufacturing ID number
 *  @para       uint8   Drive_ID[]      Device ID Number (2)
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_W25Q80_Read_ID();//W25Q80 read chip ID
 */
void MAPS_Dock_W25Q80_Read_ID(uint8 *Vender_ID,uint8 *Drive_ID)
{
  uint8 Send[4]={W25Q80_CMD_DEVICE_ID,0,0,0};
  uint8 Receive[4]={0};
  
  W25Q80_TransferCMD(&Send[0],&Receive[0],NULL,NULL,4,1);
  
  *Vender_ID = Receive[1];
  *Drive_ID = Receive[2];
  *(Drive_ID+1) = Receive[3];
}

/*
 *  @brief      W25Q80 chip erase
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_W25Q80_Erase_Chip();//W25Q80 chip erase
 */
void MAPS_Dock_W25Q80_Erase_Chip(void)
{
  uint8 CMD=W25Q80_CMD_CHIP_ERASE;
  
  W25Q80_Write_Enable();
  W25Q80_Transfer(&CMD,NULL,1);
  MAPS_Dock_W25Q80_Check(SPI_FLASH_TIMEOUT,W25Q80_CMD_READ_STATUS_REG);
  MAPS_Dock_W25Q80_Delay(SPI_FLASH_TIMEOUT);
}

/*
 *  @brief      W25Q80 chip sector erase
 *  @para       uint32  Address        Offset Address
 *  @para       uint8   Block_Size     Erase the size of the sector
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_W25Q80_Erase_Block();//W25Q80 chip sector erase
 */
void MAPS_Dock_W25Q80_Erase_Block(uint32 Address,uint8 Block_Size)
{
  uint8 CMD[4]={0};
  
  ASSERT((Address % Block_Size) == 0);
  
  switch(Block_Size)
  {
    case ERASE_SECTOR_SIZE:
                            CMD[0] = W25Q80_CMD_SECTOR_ERASE;//Erase the size of the sector 4KB
                            break;
    case ERASE_CLUSTER_SIZE:
                            CMD[0] = W25Q80_CMD_CLUSTER_ERASE;//Cluster erase 32KB
                            break;
    case ERASE_BLOCK_SIZE:
                            CMD[0] = W25Q80_CMD_BLOCK_ERASE;//Block erase 64KB
                            break;
    default:
                            ASSERT(0);
                            break;
  }
  
  CMD[1] = (Address >> 16) & 0xff;
  CMD[2] = (Address >> 8) & 0xff;
  CMD[3] = (Address >> 0) & 0xff;
  
  W25Q80_Write_Enable();
  W25Q80_Transfer(&CMD[0],NULL,4);
  MAPS_Dock_W25Q80_Check(SPI_FLASH_TIMEOUT,W25Q80_CMD_READ_STATUS_REG);
  MAPS_Dock_W25Q80_Delay(SPI_FLASH_TIMEOUT);
}

/*
 *  @brief      W25Q80 writes one page of data
 *  @para       uint16  Page_Number     Page address written(0x0000~0x0FFF)
 *  @para       uint8   Byte_Offset     Write address offset(0x00~0xFF)
 *  @para       uint8   *Page_Buff      Written array
 *  @para       uint8   Page_Buff_Len   The length of the written array
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_W25Q80_Write_Page();//W25Q80 writes one page of data
 */
void MAPS_Dock_W25Q80_Write_Page(uint16 Page_Number,uint8 Byte_Offset,uint8 *Page_Buff,uint8 Page_Buff_Len)
{
  uint8 Send[4]={W25Q80_CMD_PAGE_PROGRAM,(Page_Number>>8),(Page_Number&0xff),Byte_Offset};
  
  W25Q80_Write_Enable();
  W25Q80_TransferCMD(&Send[0],NULL,&Page_Buff[0],NULL,4,Page_Buff_Len);
  MAPS_Dock_W25Q80_Delay(SPI_FLASH_TIMEOUT);
}

/*
 *  @brief      W25Q80 writes one page of data
 *  @para       uint16  Page_Number     Page address written(0x0000~0x0FFF)
 *  @para       uint8   Byte_Offset     Write address offset(0x00~0xFF)
 *  @para       uint8   *Page_Buff      Written array
 *  @para       uint8   Page_Buff_Len   The length of the written array
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_W25Q80_Read_Page();//W25Q80 reads in one page of data
 */
void MAPS_Dock_W25Q80_Read_Page(uint16 Page_Number,uint8 Byte_Offset,uint8 *Page_Buff,uint8 Page_Buff_Len)
{
  uint8 Send[4]={W25Q80_CMD_READ_ARRAY_SLOW,(Page_Number>>8),(Page_Number&0xff),Byte_Offset};
  
  W25Q80_TransferCMD(&Send[0],NULL,NULL,&Page_Buff[0],4,Page_Buff_Len);
}
