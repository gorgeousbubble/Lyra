/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       sdhc.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __SDHC_H__
#define __SDHC_H__

#include "common.h"

#define SDHC_CLOCK      (MK64_Core_KHz * 1000)

// The initial data transfer of SD card needs to operate at a frequency below 400 kb/s
#define SDHC_INIT_BANDRATE      300000                  //baud rate at startup
#define SDHC_BANDRATE           (50*1000*1000)          //baud rate during normal operation (50MBps)
#define SDHC_BUS_WIDTH          ESDHC_BUS_WIDTH_4BIT    //bandwidth, currently the code only supports 1-bit and 4-bit
#define SDCARD_BLOCK_SIZE       512                     //SD block size
#define SEND_OCR_TIME           100                     //number of times CMD55+ACMD41 commands were sent

/* Error code returned by I/O functions */
#define IO_ERROR        (-1)

/*
**SDHC enumerate
*/
typedef enum
{
  ESDHC_IOCTL_INIT,
  ESDHC_IOCTL_SEND_CMD,
  ESDHC_IOCTL_GET_CARD,
  ESDHC_IOCTL_GET_BUS_WIDTH,
  ESDHC_IOCTL_SET_BUS_WIDTH,
}ESDHC_IOCTL_CMD_e;

/* command code used for disk_ioctrl function*/
/* ESDHC error code */
typedef enum
{
  ESDHC_IOCTL_OK,             //OK
  ESDHC_INIT_ERR,             //init error
  ESDHC_CMD_ERR,              //send command error
  ESDHC_CMD_TIMEOUT,          //timeout error
  ESDHC_CMD_INVALID,          //invalidate command
  ESDHC_PARAM_INVALID,        //invalidate parameter
  ESDHC_BUS_WIDTH_INVALID,    //bus bandwidth invalidate
  ESDHC_BUSY,                 //device busy
}ESDHC_IOCTL_ERR_e;


/* ESDHC bus bandwidth */
typedef enum
{
  //values are configured based on registers
  ESDHC_BUS_WIDTH_1BIT = 0,
  ESDHC_BUS_WIDTH_4BIT = 1,
  ESDHC_BUS_WIDTH_8BIT = 2,
}ESDHC_BUS_WIDTH_e;

/* ESDHC card type */
typedef enum
{
  ESDHC_CARD_NONE,//no card
  ESDHC_CARD_UNKNOWN,//unknown card
  ESDHC_CARD_SD,//SD
  ESDHC_CARD_SDHC,//SDHC
  ESDHC_CARD_SDIO,//SDIO
  ESDHC_CARD_SDCOMBO,
  ESDHC_CARD_SDHCCOMBO,
  ESDHC_CARD_MMC,//MMC
  ESDHC_CARD_CEATA,
}ESDHC_CARD_e;

/* ESDHC command type */
typedef enum
{
  ESDHC_TYPE_NORMAL,
  ESDHC_TYPE_SUSPEND,
  ESDHC_TYPE_RESUME,
  ESDHC_TYPE_ABORT,
}ESDHC_TYPE_e;

/* ESDHC instruction */
#define ESDHC_CMD0                           (0)
#define ESDHC_CMD1                           (1)
#define ESDHC_CMD2                           (2)
#define ESDHC_CMD3                           (3)
#define ESDHC_CMD4                           (4)
#define ESDHC_CMD5                           (5)
#define ESDHC_CMD6                           (6)
#define ESDHC_CMD7                           (7)
#define ESDHC_CMD8                           (8)
#define ESDHC_CMD9                           (9)
#define ESDHC_CMD10                          (10)
#define ESDHC_CMD11                          (11)
#define ESDHC_CMD12                          (12)
#define ESDHC_CMD13                          (13)
#define ESDHC_CMD15                          (15)
#define ESDHC_CMD16                          (16)
#define ESDHC_CMD17                          (17)
#define ESDHC_CMD18                          (18)
#define ESDHC_CMD20                          (20)
#define ESDHC_CMD24                          (24)
#define ESDHC_CMD25                          (25)
#define ESDHC_CMD26                          (26)
#define ESDHC_CMD27                          (27)
#define ESDHC_CMD28                          (28)
#define ESDHC_CMD29                          (29)
#define ESDHC_CMD30                          (30)
#define ESDHC_CMD32                          (32)
#define ESDHC_CMD33                          (33)
#define ESDHC_CMD34                          (34)
#define ESDHC_CMD35                          (35)
#define ESDHC_CMD36                          (36)
#define ESDHC_CMD37                          (37)
#define ESDHC_CMD38                          (38)
#define ESDHC_CMD39                          (39)
#define ESDHC_CMD40                          (40)
#define ESDHC_CMD42                          (42)
#define ESDHC_CMD52                          (52)
#define ESDHC_CMD53                          (53)
#define ESDHC_CMD55                          (55)
#define ESDHC_CMD56                          (56)
#define ESDHC_CMD60                          (60)
#define ESDHC_CMD61                          (61)

#define ESDHC_ACMD6                          (6)
#define ESDHC_ACMD13                         (13)
#define ESDHC_ACMD22                         (22)
#define ESDHC_ACMD23                         (23)
#define ESDHC_ACMD41                         (41)
#define ESDHC_ACMD42                         (42)
#define ESDHC_ACMD51                         (51)




#define ESDHC_XFERTYP_RSPTYP_NO              (0x00)
#define ESDHC_XFERTYP_RSPTYP_136             (0x01)
#define ESDHC_XFERTYP_RSPTYP_48              (0x02)
#define ESDHC_XFERTYP_RSPTYP_48BUSY          (0x03)

#define ESDHC_XFERTYP_CMDTYP_ABORT           (0x03)

#define ESDHC_PROCTL_EMODE_INVARIANT         (0x02)

#define ESDHC_PROCTL_DTW_1BIT                (0x00)
#define ESDHC_PROCTL_DTW_4BIT                (0x01)
#define ESDHC_PROCTL_DTW_8BIT                (0x10)


typedef struct
{
  uint8  COMMAND;             //command
  uint8  TYPE;                //command type SDHC_XFERTYP_CMDTYP
  uint8  READ;
  uint8  RES;                 //reseverd
  uint32 ARGUMENT;            //parameter register of command (SDHC_CMDARG)
  uint32 BLOCKS;
  uint32 RESPONSE[4];
}ESDHC_CMD_t, *pESDHC_CMD_t;


/*!
 *  @brief      SD Card structure
 */
typedef struct
{
    /* card status                                  */
    uint32                CARD;

    /* Bottom level response timeout >= 250 ms      */
    uint32               SD_TIMEOUT;

    /* The number of blocks for this device         */
    uint32                NUM_BLOCKS;

    /* card address                                 */
    uint32                ADDRESS;

    /* High capacity=block addressing (SD is byte addressing, SDHC is block addressing) */
    uint8                 SDHC_SDHC;//

    /* Standard 2 or higher version cards=different CSD registers                       */
    uint8                 VERSION2;

    uint8                 RES[2];

} SDCARD_t, *pSDCARD_t;

#define SDHC_is_running()       (0 != (SDHC_PRSSTAT & (SDHC_PRSSTAT_RTA_MASK | SDHC_PRSSTAT_WTA_MASK | SDHC_PRSSTAT_DLA_MASK | SDHC_PRSSTAT_CDIHB_MASK | SDHC_PRSSTAT_CIHB_MASK)))
#define SDHC_STATUS_WAIT(MASK)  while (0 ==  (SDHC_IRQSTAT & (MASK)));
#define SDHC_STATUS_GET(MASK)   (SDHC_IRQSTAT & (MASK))

extern SDCARD_t                 SDHC_card;
extern void                     SDHC_init();                                        // SDHC init
extern ESDHC_IOCTL_ERR_e        SDHC_ioctl(ESDHC_IOCTL_CMD_e  cmd, void *param_ptr); // SDHC control command
extern void                     SDHC_set_baudrate(uint32 baudrate);                 // SDHC baud rate configure
extern uint32                   SDHC_cmd (pESDHC_CMD_t command);                    // SDHC send cmd command


#endif