/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       flash.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "dwt.h"
#include "flash.h"

/*
 *  @brief      Flash command
 *  @return     Command execution result (1 successful, 0 failed)
 *  @since      v1.0
 */
__RAMFUNC uint8 Flash_CMD(void)
{
  FTFE_FSTAT = (0
                | FTFE_FSTAT_CCIF_MASK        //Instruction completion flag (write 1 clear 0)
                | FTFE_FSTAT_RDCOLERR_MASK    //Read conflict error flag (write 1 clear 0)
                | FTFE_FSTAT_ACCERR_MASK      //Access error flag (write 1 clear 0)
                | FTFE_FSTAT_FPVIOL_MASK      //Illegal access protection flag (write 1 clear 0)
                );
  while(!(FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK)); //Waiting for the command to complete
  
  if(FTFE_FSTAT & (FTFE_FSTAT_ACCERR_MASK | FTFE_FSTAT_RDCOLERR_MASK | FTFE_FSTAT_FPVIOL_MASK | FTFE_FSTAT_MGSTAT0_MASK))
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

/*
 *  @brief      Initialization flash
 *  @since      v1.0
 */
__RAMFUNC void Flash_Init(void)
{
  // Clear Flash pre read buffer
  FMC_PFB0CR |= FMC_PFB0CR_S_B_INV_MASK;
  
  while(!(FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK));
  
  FTFE_FSTAT = (0
                | FTFE_FSTAT_CCIF_MASK        //Instruction completion flag (write 1 clear 0)
                | FTFE_FSTAT_RDCOLERR_MASK    //Read conflict error flag (write 1 clear 0)
                | FTFE_FSTAT_ACCERR_MASK      //Access error flag (write 1 clear 0)
                | FTFE_FSTAT_FPVIOL_MASK      //Illegal access protection flag (write 1 clear 0)
                );
  DELAY_MS(10);
}

/*
 *  @brief      Erase the specified flash sector
 *  @param      Sector_num sector number (MK66FN2M0 actually uses 0~511, MK66FX1M0 actually uses 0~255)
 *  @return     Execution result (1 successful, 0 failed)
 *  @since      v1.0
 *  Sample usage:       Flash_Erase_Sector(512-1);        //Erase sector 512-1
 */
__RAMFUNC uint8 Flash_Erase_Sector(uint16 sector_num)
{
  uint32 addr = sector_num * FLASH_SECTOR_SIZE;
  
  //Set erase command
  FCMD = ERSSCR;

  //Set target address
  FADDR2 = ((Dtype *)&addr)->B[2];
  FADDR1 = ((Dtype *)&addr)->B[1];
  FADDR0 = ((Dtype *)&addr)->B[0];
  
  if(Flash_CMD() == 0)//Is the Flash_CMD command completed
  {
    return 0;
  }
  
  if(sector_num == 0)//Sector number should not be 0
  {
    return Flash_Write(sector_num,0x000408,0xFFFFFFFFFFFFFFFF);
  }
  
  return 1;
}

/*
 *  @brief      Write long byte data to the specified address in flash
 *  @param      Sector_num sector number (MK66FN2M0 actually uses 0~511, MK66FX1M0 actually uses 0~255)
 *  @param      offset          Write the offset address inside the sector (multiples of 8 from 0 to 4095)
 *  @param      data            Data that needs to be written
 *  @return     Execution result (1 successful, 0 failed)
 *  @since      v1.0
 *  Sample usage:       Flash_Write(511,0,0xAABBCCDDEEFF0011);        //Sector 127, offset address 0, write data: 0xFFFFFFE
 */
__RAMFUNC uint8 Flash_Write(uint16 sector_num, uint16 offset, FLASH_WRITE_TYPE data)
{
  uint32 addr = sector_num * FLASH_SECTOR_SIZE + offset;
  uint32 tmpdata;
  
  //The offset address is not a multiple of FLASH_LIGN_ADDR or the offset address is greater than the sector size, resulting in execution failure
  if(offset % FLASH_ALIGN_ADDR != 0 || offset > FLASH_SECTOR_SIZE)
  {
    return 0;
  }
  
  //Set target address
  FADDR2 = ((Dtype *)&addr)->B[2];
  FADDR1 = ((Dtype *)&addr)->B[1];
  FADDR0 = ((Dtype *)&addr)->B[0];
  
  //Set low 32-bit data
  tmpdata = (uint32)data;
  FDATA0 = ((Dtype *)&tmpdata)->B[3];                  //Set write data
  FDATA1 = ((Dtype *)&tmpdata)->B[2];
  FDATA2 = ((Dtype *)&tmpdata)->B[1];
  FDATA3 = ((Dtype *)&tmpdata)->B[0];
  
  FCMD = PGM8;
  
  tmpdata = (uint32)(data>>32);

  FDATA4 = ((Dtype *)&tmpdata)->B[3];                 //Set write data
  FDATA5 = ((Dtype *)&tmpdata)->B[2];
  FDATA6 = ((Dtype *)&tmpdata)->B[1];
  FDATA7 = ((Dtype *)&tmpdata)->B[0];
  
  if(Flash_CMD() == 0)
  {
    return 0;
  }
  
  return 1;
}

/*
 *  @brief      Write data buffer to specified address in flash
 *  @param      Sector_num sector number (MK66FN2M0 actually uses 0~511, MK66FX1M0 actually uses 0~255)
 *  @param      Offset Write the offset address inside the sector (multiples of 8 from 0 to 4095)
 *  @param      The first address of the data buffer that buf needs to write to
 *  @return     Execution result (1 successful, 0 failed)
 *  @since      v1.0
 *  Sample usage:           uint32 buff[10];
                            Flash_Write_Buff(127,0,sizeof(buff),buff);        //Sector 127, offset address 0, write data source address: buff, number of sizeof (buff)
 */
__RAMFUNC uint8 Flash_Write_Buff(uint16 sector_num, uint16 offset, uint16 cnt, uint8 *buf)
{
  uint32  size;
  uint32  addr;
  uint32  data;
  
  //The offset address is not a multiple of FLASH_LIGN_ADDR or the offset address is greater than the sector size, resulting in execution failure
  if(offset % FLASH_ALIGN_ADDR != 0 || offset > FLASH_SECTOR_SIZE)
  {
    return 0;
  }
  
  addr = sector_num * FLASH_SECTOR_SIZE + offset;
  
  FCMD = PGM8;
  
  for(size = 0; size < cnt; size += FLASH_ALIGN_ADDR)
  {
    //Set target address
    FADDR2 = ((Dtype *)&addr)->B[2];
    FADDR1 = ((Dtype *)&addr)->B[1];
    FADDR0 = ((Dtype *)&addr)->B[0];
    
    //Set low 32-bit data
    data =  *(uint32 *)buf;
    
    FDATA0 = ((Dtype *)&data)->B[3];                    //Set write data
    FDATA1 = ((Dtype *)&data)->B[2];
    FDATA2 = ((Dtype *)&data)->B[1];
    FDATA3 = ((Dtype *)&data)->B[0];
    
    //Set high 32-bit data
    data = *(uint32 *)(buf+4);

    FDATA4 = ((Dtype *)&data)->B[3];                    //Set write data
    FDATA5 = ((Dtype *)&data)->B[2];
    FDATA6 = ((Dtype *)&data)->B[1];
    FDATA7 = ((Dtype *)&data)->B[0];
    
    if(Flash_CMD() == 0)
    {
      return 0;
    }
    
    addr += FLASH_ALIGN_ADDR;
    buf += FLASH_ALIGN_ADDR;
  }
  
  return 1;
}