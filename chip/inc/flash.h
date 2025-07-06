/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       flash.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __FLASH_H__
#define __FLASH_H__

#include "common.h"

#if defined(MK64FN1M0)
#define FLASH_SECTOR_SIZE       (4*1024)        //The sector size is 4KB bytes
#define FLASH_SECTOR_NUM        (256)           //256 sectors (256 * 4KB=1M)
#define FLASH_ALIGN_ADDR        (8)             //Address alignment integer multiples
#elif defined(MK64FX512)
#define FLASH_SECTOR_SIZE       (4*1024)        //The sector size is 4KB bytes
#define FLASH_SECTOR_NUM        (128)           //128 sectors (128 * 4KB=512K)
#define FLASH_ALIGN_ADDR        (8)             //Address alignment integer multiples
#endif
typedef uint64  FLASH_WRITE_TYPE;               //The data type written by the Flash_Lrite function

//Macro defined FCCOB Number
#define     FCMD        FTFE_FCCOB0         //FTFL command
#define     FADDR2      FTFE_FCCOB1         //Flash address [23:16]
#define     FADDR1      FTFE_FCCOB2         //Flash address [15:8]
#define     FADDR0      FTFE_FCCOB3         //Flash address [7:0]
#define     FDATA0      FTFE_FCCOB4         //Data Byte 0       //Note that in 4-byte sorting, FDATA3 is the lowest bit and FDATA0 is the highest bit
#define     FDATA1      FTFE_FCCOB5         //Data Byte 1
#define     FDATA2      FTFE_FCCOB6         //Data Byte 2
#define     FDATA3      FTFE_FCCOB7         //Data Byte 3
#define     FDATA4      FTFE_FCCOB8         //Data Byte 4
#define     FDATA5      FTFE_FCCOB9         //Data Byte 5
#define     FDATA6      FTFE_FCCOBA         //Data Byte 6
#define     FDATA7      FTFE_FCCOBB         //Data Byte 7

//FCMD command
#define     RD1BLK    0x00   //Read the entire Flash
#define     RD1SEC    0x01   //Read the entire sector
#define     PGMCHK    0x02   //Write check
#define     RDRSRC    0x03   //Read target data (8 bytes)
#define     PGM8      0x07   //Write a long word (8 bytes)
#define     ERSBLK    0x08   //Erase the entire Flash
#define     ERSSCR    0x09   //Erase Flash sectors
#define     PGMSEC    0x0B   //Write sector
#define     RD1ALL    0x40   //Read all the blocks
#define     RDONCE    0x41   //Read only once
#define     PGMONCE   0x43   //Only write once
#define     ERSALL    0x44   //Erase all blocks
#define     VFYKEY    0x45   //Verify the backdoor access key
#define     PGMPART   0x80   //Write to partition
#define     SETRAM    0x81   //Set FlexRAM function

/*
**Flash reads sectors
*/
#define Flash_Read(SectorNo,Offset,Type)        (*(Type*)((uint32)((SectorNo)*FLASH_SECTOR_SIZE+Offset)))

/*
**function declaration
*/
__RAMFUNC uint8 Flash_CMD(void);
__RAMFUNC void Flash_Init(void);
__RAMFUNC uint8 Flash_Erase_Sector(uint16 sector_num);
__RAMFUNC uint8 Flash_Write(uint16 sector_num, uint16 offset, FLASH_WRITE_TYPE data);
__RAMFUNC uint8 Flash_Write_Buff(uint16 sector_num, uint16 offset, uint16 cnt, uint8 *buf);

#endif