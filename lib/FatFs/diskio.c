/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       diskio.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "diskio.h"
#include "sdhc.h"


static volatile DSTATUS   Stat             = STA_NOINIT;    /* hard disk status */

/*
 *  @brief      hard disk status
 *  @param      drv                 Device number (currently code only supports 0)
 *  @since      v1.0
 */
DSTATUS disk_initialize (unsigned char drv)
{
    uint32                      param, c_size, c_size_mult, read_bl_len;
    ESDHC_CMD_t                 command;

    if (drv) return STA_NOINIT;         /* currently, the code only supports one device */
    if (Stat & STA_NODISK) return Stat; /* no card inserted */
    if ((Stat & STA_NOINIT) == 0)   return 0;/* not initiated */

    SDHC_card.SD_TIMEOUT = 0;
    SDHC_card.NUM_BLOCKS = 0;
    SDHC_card.ADDRESS = 0;
    SDHC_card.SDHC_SDHC = FALSE;
    SDHC_card.VERSION2 = FALSE;

    /* initialization and detection card */
    if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_INIT, NULL))
    {
        return FALSE;
    }

    /* SDHC check */
    param = 0;
    if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_GET_CARD, &param))
    {
        return FALSE;
    }
    if ((ESDHC_CARD_SD == param) || (ESDHC_CARD_SDHC == param) || (ESDHC_CARD_SDCOMBO == param) || (ESDHC_CARD_SDHCCOMBO == param))
    {
        if ((ESDHC_CARD_SDHC == param) || (ESDHC_CARD_SDHCCOMBO == param))
        {
            SDHC_card.SDHC_SDHC = TRUE;
        }
    }
    else
    {
        return FALSE;
    }

    /* card recognition */
    command.COMMAND = ESDHC_CMD2;
    command.TYPE = ESDHC_TYPE_NORMAL;
    command.ARGUMENT = 0;
    command.READ = FALSE;
    command.BLOCKS = 0;
    if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
    {
        return FALSE;
    }

    /* obtain card address */
    command.COMMAND = ESDHC_CMD3;
    command.TYPE = ESDHC_TYPE_NORMAL;
    command.ARGUMENT = 0;
    command.READ = FALSE;
    command.BLOCKS = 0;
    if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
    {
        return FALSE;
    }
    SDHC_card.ADDRESS = command.RESPONSE[0] & 0xFFFF0000;

    /* obtain card parameters */
    command.COMMAND = ESDHC_CMD9;
    command.TYPE = ESDHC_TYPE_NORMAL;
    command.ARGUMENT = SDHC_card.ADDRESS;
    command.READ = FALSE;
    command.BLOCKS = 0;
    if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
    {
        return FALSE;
    }
    if (0 == (command.RESPONSE[3] & 0x00C00000))
    {
        read_bl_len = (command.RESPONSE[2] >> 8) & 0x0F;
        c_size = command.RESPONSE[2] & 0x03;
        c_size = (c_size << 10) | (command.RESPONSE[1] >> 22);
        c_size_mult = (command.RESPONSE[1] >> 7) & 0x07;
        SDHC_card.NUM_BLOCKS = (c_size + 1) * (1 << (c_size_mult + 2)) * (1 << (read_bl_len - 9));
    }
    else
    {
        SDHC_card.VERSION2 = TRUE;
        c_size = (command.RESPONSE[1] >> 8) & 0x003FFFFF;
        SDHC_card.NUM_BLOCKS = (c_size + 1) << 10;
    }

    /* select card */
    command.COMMAND = ESDHC_CMD7;
    command.TYPE = ESDHC_TYPE_NORMAL;
    command.ARGUMENT = SDHC_card.ADDRESS;
    command.READ = FALSE;
    command.BLOCKS = 0;
    if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
    {
        return FALSE;
    }

    /* set block size */
    command.COMMAND = ESDHC_CMD16;
    command.TYPE = ESDHC_TYPE_NORMAL;
    command.ARGUMENT = SDCARD_BLOCK_SIZE;
    command.READ = FALSE;
    command.BLOCKS = 0;
    if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
    {
        return FALSE;
    }

    if (ESDHC_BUS_WIDTH_4BIT == SDHC_BUS_WIDTH)
    {
        /* application specific commands */
        command.COMMAND = ESDHC_CMD55;
        command.TYPE = ESDHC_TYPE_NORMAL;
        command.ARGUMENT = SDHC_card.ADDRESS;
        command.READ = FALSE;
        command.BLOCKS = 0;
        if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
        {
            return FALSE;
        }

        /* set bandwidth == 4 */
        command.COMMAND = ESDHC_ACMD6;
        command.TYPE = ESDHC_TYPE_NORMAL;
        command.ARGUMENT = 2;
        command.READ = FALSE;
        command.BLOCKS = 0;
        if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
        {
            return FALSE;
        }

        param = ESDHC_BUS_WIDTH_4BIT;
        if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_SET_BUS_WIDTH, &param))
        {
            return FALSE;
        }
    }

    Stat &= ~STA_NOINIT;        /* clear STA_NOINIT */

    return (Stat & 0x03);
}

/*
 *  @brief      Receive data packets from MMC
 *  @param      buff                Receive a data block of SDCARD_BLOCK_SIZE bytes to the data buffer address
 *  @param      btr                 Number of bytes (must be a multiple of 4)
 *  @since      v1.0
 */
static int rcvr_datablock (uint8   *buff, uint32  btr)
{
    uint32  bytes, i, j;
    uint32  *ptr = (uint32 *)buff;


    /* Workaround for random bit polling failures - not suitable for IO cards    */

    while(!(SDHC_IRQSTAT & SDHC_IRQSTAT_BRR_MASK));
    SDHC_IRQSTAT =  SDHC_IRQSTAT_BRR_MASK ;

    /* Read data in 4 byte counts */
    bytes = btr;
    while (bytes)
    {
        i = bytes > SDCARD_BLOCK_SIZE ? SDCARD_BLOCK_SIZE : bytes;
        for (j = (i + 3) >> 2; j != 0; j--)
        {
            if (SDHC_IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
            {
                SDHC_IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BRR_MASK;
                return 0;
            }

            while (0 == (SDHC_PRSSTAT & SDHC_PRSSTAT_BREN_MASK)) {};

            // little port
            *ptr++ = SDHC_DATPORT;
        }
        bytes -= i;
    }

    return 1;                       /* Return with success */
}

/*
 *  @brief      read sector
 *  @param      drv                 driver number (currently code only supports 0)
 *  @param      buff                buffer address
 *  @param      sector              sector number
 *  @param      count               sector number (1~255)
 *  @return     DRESULT             execute result
 *  @since      v1.0
 */
DRESULT disk_read (         //read disk sectors
    uint8  drv,             /* physical driver number (0) */
    uint8  *buff,           /* point to the data buffer to store the read data */
    uint32 sector,          /* start sector number (LBA) */
    uint8  count            /* total sector number(1..255) */
)
{
    ESDHC_CMD_t command;

    if (drv || (!count)) return RES_PARERR;       //drv can only be 0, count must not be equal to 0
    if (Stat & STA_NOINIT) return RES_NOTRDY;     //not ready

    /* detect parameter */
    if ((NULL == buff))
    {
        return RES_PARERR;//paramter invalidate
    }

    if (!SDHC_card.SDHC_SDHC)
    {
        sector *= SDCARD_BLOCK_SIZE;    /* if necessary, convert to byte address */
    }

    if (count == 1) /* single block reading */
    {
        command.COMMAND = ESDHC_CMD17;
        command.TYPE = ESDHC_TYPE_NORMAL;
        command.ARGUMENT = sector;
        command.READ = TRUE;
        command.BLOCKS = count;

        if (ESDHC_IOCTL_OK == SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
        {
            if (rcvr_datablock(buff, SDCARD_BLOCK_SIZE))
            {
                count = 0;
            }
        }
    }
    else
    {
        /* multiple block */
        //
        command.COMMAND = ESDHC_CMD18;
        //command.COMMAND = ESDHC_CMD17;
        command.TYPE = ESDHC_TYPE_NORMAL;
        command.ARGUMENT = sector;
        command.READ = TRUE;
        command.BLOCKS = count;

        if (ESDHC_IOCTL_OK == SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
        {
            if (rcvr_datablock(buff, SDCARD_BLOCK_SIZE * count))
            {
                count = 0;
            }
        }
    }

    return count ? RES_ERROR : RES_OK;
}

/*
 *  @brief      sending data packets to MMC
 *  @param      buff                send the data buffer address of SDCARD_BLOCK_SIZE byte data block
 *  @param      btr                 number of bytes (must be a multiple of 4)
 *  @since      v1.0
 */
static int xmit_datablock (const uint8 *buff, uint32 btr    )
{
    uint32  bytes, i;
    uint32  *ptr = (uint32 *)buff;

    /* Write data in 4 byte counts */
    bytes = btr;
    while (bytes)
    {
        i = bytes > SDCARD_BLOCK_SIZE ? SDCARD_BLOCK_SIZE : bytes;
        bytes -= i;
        for (i = (i + 3) >> 2; i != 0; i--)
        {
            if (SDHC_IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
            {
                SDHC_IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BWR_MASK;
                return IO_ERROR;
            }
            while (0 == (SDHC_PRSSTAT & SDHC_PRSSTAT_BWEN_MASK)) {};

            SDHC_DATPORT = *ptr++;
        }
    }

    return 1;
}



/*
 *  @brief      write sector
 *  @param      drv                 driver number (currently code only supports 0)
 *  @param      buff                buffer address
 *  @param      sector              sector number
 *  @param      count               sector number (1~255)
 *  @return     DRESULT             result
 *  @since      v1.0
 */
DRESULT disk_write (uint8  drv, const uint8  *buff, uint32 sector, uint8  count)
{
    ESDHC_CMD_t command;
    //pSDCARD_t    sdcard_ptr = (pSDCARD_t)&SDHC_card;

    if (drv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    if (Stat & STA_PROTECT) return RES_WRPRT;

    /* Check parameters */
    if ((NULL == buff))
    {
        return RES_PARERR;      // parameter invalidate
    }

    if (!SDHC_card.SDHC_SDHC)
    {
        sector *= SDCARD_BLOCK_SIZE;    /* Convert to byte address if needed */
    }

    if (count == 1) /* Single block write */
    {
        command.COMMAND = ESDHC_CMD24;
        command.TYPE = ESDHC_TYPE_NORMAL;
        command.ARGUMENT = sector;
        command.READ = FALSE;
        command.BLOCKS = count;

        if (ESDHC_IOCTL_OK == SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
        {
            if (xmit_datablock(buff, SDCARD_BLOCK_SIZE))
            {
                count = 0;
            }
        }
    }
    else
    {
        command.COMMAND = ESDHC_CMD25;
        command.TYPE = ESDHC_TYPE_NORMAL;
        command.ARGUMENT = sector;
        command.READ = FALSE;
        command.BLOCKS = count;

        if (ESDHC_IOCTL_OK == SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
        {
            if (xmit_datablock(buff, SDCARD_BLOCK_SIZE * count))
            {
                count = 0;
            }
            while((SDHC_IRQSTAT & SDHC_IRQSTAT_TC_MASK)==0);

            if (SDHC_IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
            {
                SDHC_IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK;
            }
            SDHC_IRQSTAT |= SDHC_IRQSTAT_TC_MASK | SDHC_IRQSTAT_BRR_MASK | SDHC_IRQSTAT_BWR_MASK;


        }
    }

    /* Wait for card ready / transaction state */
    do
    {
        command.COMMAND = ESDHC_CMD13;
        command.TYPE = ESDHC_TYPE_NORMAL;
        command.ARGUMENT = SDHC_card.ADDRESS;
        command.READ = FALSE;
        command.BLOCKS = 0;
        if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
        {
            return RES_ERROR;
        }

        /* Card status error check */
        if (command.RESPONSE[0] & 0xFFD98008)
        {
            return RES_ERROR;
        }
    }
    while (0x000000900 != (command.RESPONSE[0] & 0x00001F00));

    return count ? RES_ERROR : RES_OK;
}


/*
 *  @brief      hard disk control command
 *  @param      drv                 dirver number
 *  @param      ctrl                command
 *  @param      buff                buffer address, used for receiving and sending control data
 *  @since      v1.0
 */
DRESULT disk_ioctl (
    uint8 drv,      /* Physical drive nmuber (0) */
    uint8 ctrl,     /* Control code */
    void  *buff     /* Buffer to send/receive control data */
)
{
    DRESULT              res;
    ESDHC_CMD_t command;
    //pSDCARD_t  sdcard_ptr = (pSDCARD_t)&SDHC_card;

    if (drv) return RES_PARERR;

    res = RES_ERROR;

    if (Stat & STA_NOINIT) return RES_NOTRDY;

    switch (ctrl)
    {
    case CTRL_SYNC :        /* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT : /* Get number of sectors on the disk (uint32) */
        *(unsigned long *)buff = SDHC_card.NUM_BLOCKS;
        res = RES_OK;
        break;

    case GET_SECTOR_SIZE :  /* Get R/W sector size (WORD) */
        *(unsigned short *)buff = SDCARD_BLOCK_SIZE;
        res = RES_OK;
        break;

    case GET_BLOCK_SIZE :           /* Get erase block size in unit of sector (uint32) */
        // Implementar
        command.COMMAND = ESDHC_CMD9;
        command.TYPE = ESDHC_TYPE_NORMAL;
        command.ARGUMENT = SDHC_card.ADDRESS;
        command.READ = FALSE;
        command.BLOCKS = 0;
        if (ESDHC_IOCTL_OK != SDHC_ioctl (ESDHC_IOCTL_SEND_CMD, &command))
        {
            return RES_ERROR;
        }
        if (0 == (command.RESPONSE[3] & 0x00C00000))
        {
            //SD V1
            *(unsigned long *)buff = ((((command.RESPONSE[2] >> 18) & 0x7F) + 1) << (((command.RESPONSE[3] >> 8) & 0x03) - 1));
        }
        else
        {
            //SD V2
            // Implementar
            //*(uint32*)buff = (((command.RESPONSE[2] >> 18) & 0x7F) << (((command.RESPONSE[3] >> 8) & 0x03) - 1));
        }
        res = RES_OK;
        break;

    default:
        res = RES_PARERR;
    }

    return res;
}


/*
 *  @brief      get hard disk status
 *  @return     hard disk status
 *  @since      v1.0
 */
DSTATUS disk_status (
    uint8 drv       /* Physical drive nmuber (0) */
)
{
    if (drv) return STA_NOINIT;     /* Supports only single drive */
    return Stat;
}


/*
 *  @brief      retrieve time (added to meet interface requirements, but not actually used to implement functionality)
 *  @return     result is always 0
 *  @since      v1.0
 */
uint32  get_fattime (void)
{
    return   0;
}


