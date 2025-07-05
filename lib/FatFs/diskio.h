/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       diskio.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef _DISKIO
#define _DISKIO

#define _READONLY   0   /* 1: Remove write functions */
#define _USE_IOCTL  1   /* 1: Use disk_ioctl fucntion */


#include "common.h"
#include "integer.h"


/* Status of Disk Functions */
typedef uint8   DSTATUS;

/* Results of Disk Functions */
typedef enum
{
    RES_OK = 0,                 /* 0: success               */
    RES_ERROR,                  /* 1: R/W error             */
    RES_WRPRT,                  /* 2: write protect         */
    RES_NOTRDY,                 /* 3: not ready             */
    RES_PARERR                  /* 4: parameter invalidate  */
} DRESULT;


/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT      0x01    /* disk not initialized 1 means not initialized, 0 means initialized   */
#define STA_NODISK      0x02    /* disk not inserted    */
#define STA_PROTECT     0x04    /* write protection     */


/* Generic command (defined for FatFs) */
#define CTRL_SYNC           0   /* flush disk buffer */
#define GET_SECTOR_COUNT    1   /* get disk size */
#define GET_SECTOR_SIZE     2   /* obtain sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE      3   /* get block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR   4   /* forcefully delete a sector (for only _USE_ERASE) */

/* Generic command */
#define CTRL_POWER          5   /* Get/Set power status */
#define CTRL_LOCK           6   /* Lock/Unlock media removal */
#define CTRL_EJECT          7   /* Eject media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE        10  /* Get card type */
#define MMC_GET_CSD         11  /* Get CSD */
#define MMC_GET_CID         12  /* Get CID */
#define MMC_GET_OCR         13  /* Get OCR */
#define MMC_GET_SDSTAT      14  /* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV         20  /* Get F/W revision */
#define ATA_GET_MODEL       21  /* Get model name */
#define ATA_GET_SN          22  /* Get serial number */

/* NAND specific ioctl command */
#define NAND_FORMAT         30  /* Create physical format */


/*
**function declaration
*/
extern DSTATUS disk_initialize (uint8);                         //initialize disk (only supports disk 0)
extern DSTATUS disk_status (uint8);                             //return to disk status
extern DRESULT disk_read (uint8, uint8 *, uint32, uint8);       //read disk sectors
extern DRESULT disk_ioctl (uint8, uint8, void *);               //disk control
extern uint32   get_fattime (void);                             //get the current time

#if _READONLY == 0
DRESULT disk_write (uint8, const uint8 *, uint32, uint8);       //write disk sectors
#endif


#endif
