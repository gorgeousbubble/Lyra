/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       sdhc.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "dwt.h"
#include "port.h"
#include "sdhc.h"
#include "system_init.h"

SDCARD_t        SDHC_card;


//SD card command
const uint32 ESDHC_COMMAND_XFERTYP[] =
{
    /* CMD0 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD0) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD1) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD2) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD3) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD4) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO),
    /* CMD5 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD5) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD6) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD7) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD8) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD9) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136),
    /* CMD10 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD10) | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD11) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD12) | SDHC_XFERTYP_CMDTYP(ESDHC_XFERTYP_CMDTYP_ABORT) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD13) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    ~0,
    /* CMD15 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD15) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD16) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD17) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD18) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48)| SDHC_XFERTYP_AC12EN_MASK,
    ~0,
    /* CMD20 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD20) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    ~0,
    SDHC_XFERTYP_CMDINX(ESDHC_ACMD22) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_ACMD23) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD24) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    /* CMD25 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD25) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48) | SDHC_XFERTYP_AC12EN_MASK,
    SDHC_XFERTYP_CMDINX(ESDHC_CMD26) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD27) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD28) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD29) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY),
    /* CMD30 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD30) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    ~0,
    SDHC_XFERTYP_CMDINX(ESDHC_CMD32) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD33) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD34) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    /* CMD35 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD35) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD36) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD37) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD38) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD39) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    /* CMD40 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD40) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_ACMD41) | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD42) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY),
    ~0,
    ~0,
    /* CMD45 */
    ~0,
    ~0,
    ~0,
    ~0,
    ~0,
    /* CMD50 */
    ~0,
    SDHC_XFERTYP_CMDINX(ESDHC_ACMD51) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD52) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD53) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    ~0,
    /* CMD55 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD55) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD56) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY),
    ~0,
    ~0,
    ~0,
    /* CMD60 */
    SDHC_XFERTYP_CMDINX(ESDHC_CMD60) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY),
    SDHC_XFERTYP_CMDINX(ESDHC_CMD61) | SDHC_XFERTYP_CICEN_MASK | SDHC_XFERTYP_CCCEN_MASK | SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY),
    ~0,
    ~0
};

#define SDHC_CMD_MAX    ARR_SIZE( ESDHC_COMMAND_XFERTYP )



/*
 *  @brief      SDHC initialization (only module initialization, not SD card initialization)
 *  @since      v1.0
 */
void SDHC_init()
{

    SDHC_card.CARD = ESDHC_CARD_NONE;

    /* enable SDHC module clock */
    SIM_SCGC3 |= SIM_SCGC3_SDHC_MASK;

    /* reset ESDHC */
    SDHC_SYSCTL = SDHC_SYSCTL_RSTA_MASK | SDHC_SYSCTL_SDCLKFS(0x80);
    while (SDHC_SYSCTL & SDHC_SYSCTL_RSTA_MASK) {};

    /* initialize value */
    SDHC_VENDOR     = 0;
    SDHC_BLKATTR    = SDHC_BLKATTR_BLKCNT(1) | SDHC_BLKATTR_BLKSIZE(512);
    SDHC_PROCTL     = SDHC_PROCTL_EMODE(ESDHC_PROCTL_EMODE_INVARIANT) | SDHC_PROCTL_D3CD_MASK;
    SDHC_WML        = SDHC_WML_RDWML(1) | SDHC_WML_WRWML(1);

    /* configure ESDHC baud rate */
    SDHC_set_baudrate ( SDHC_INIT_BANDRATE );

    /* Poll inhibit bits */
    while (SDHC_PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK)) {};

    /* initialize pin reuse */
    PORT_Init(PTE0, ALT4 | HDS | PULLUP ); /* ESDHC.D1  */
    PORT_Init(PTE1, ALT4 | HDS | PULLUP ); /* ESDHC.D0  */
    PORT_Init(PTE2, ALT4 | HDS          ); /* ESDHC.CLK */
    PORT_Init(PTE3, ALT4 | HDS | PULLUP ); /* ESDHC.CMD */
    PORT_Init(PTE4, ALT4 | HDS | PULLUP ); /* ESDHC.D3  */
    PORT_Init(PTE5, ALT4 | HDS | PULLUP ); /* ESDHC.D2  */

    /* enable request */
    SDHC_IRQSTAT = 0xFFFF;
    SDHC_IRQSTATEN =      SDHC_IRQSTATEN_DEBESEN_MASK | SDHC_IRQSTATEN_DCESEN_MASK | SDHC_IRQSTATEN_DTOESEN_MASK
                          | SDHC_IRQSTATEN_CIESEN_MASK | SDHC_IRQSTATEN_CEBESEN_MASK | SDHC_IRQSTATEN_CCESEN_MASK | SDHC_IRQSTATEN_CTOESEN_MASK
                          | SDHC_IRQSTATEN_BRRSEN_MASK | SDHC_IRQSTATEN_BWRSEN_MASK | SDHC_IRQSTATEN_CRMSEN_MASK
                          | SDHC_IRQSTATEN_TCSEN_MASK | SDHC_IRQSTATEN_CCSEN_MASK;

    /* send 80 initial clock cycles to the card, which are required during the power on period of the card */
    SDHC_SYSCTL |= SDHC_SYSCTL_INITA_MASK;
    while (SDHC_SYSCTL & SDHC_SYSCTL_INITA_MASK) {};            //waiting for 80 SD cycles to complete sending

    /* check if the card is inserted */
    if (SDHC_PRSSTAT & SDHC_PRSSTAT_CINS_MASK)                  //the CINS field changes from 0 to 1 to indicate card insertion, and from 1 to 0 to indicate card removal
    {
        SDHC_card.CARD = ESDHC_CARD_UNKNOWN;                    //Card inserted, unknown type
    }
    SDHC_IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;                      //write 1 clear CRM flag. 0 represents inserting the card, 1 represents removing the card. After writing 1 to 0, if the card is still not inserted, the recovery value is 1
}




/*
 *  @brief      SDHC baud rate configuration
 *  @param      baudrate    baud rate(Hz)
 *  @since      v1.0
 */
void SDHC_set_baudrate(uint32 baudrate)
{
    uint32 pres, div, min, minpres = 0x80, mindiv = 0x0F;
    int32  val;
    uint32 clock = SDHC_CLOCK;

    /* calculate the optimal configuration */
    // SDCLK clock frequency=base clock/(prescaler * divisor)
    // the maximum value of SD clock frequency is 50MHz
    // pre division=2,4,8,16,32,64128256, and SDCLKFS=Pre division>>1;
    // divisor=DVS+1, the value range of DVS is 0~0xF
    min = (uint32) - 1;                         //set min to its maximum value first
    for (pres = 2; pres <= 256; pres <<= 1)     //pres stands for Pre Divider
    {
        for (div = 1; div <= 16; div++)         //div stands for Divisor
        {
            val = pres * div * baudrate - clock;
            if (val >= 0)                       //the baud rate is greater than or equal to the set target value
            {
                if (min > val)                  //choose the one closest to the target value
                {
                    min = val;
                    minpres = pres;
                    mindiv = div;
                }
            }
        }
    }

    /* prohibit ESDHC clock */
    SDHC_SYSCTL &= (~ SDHC_SYSCTL_SDCLKEN_MASK);

    /* change the frequency division coefficient */
    SDHC_SYSCTL = (   (SDHC_SYSCTL & (~ (SDHC_SYSCTL_DTOCV_MASK | SDHC_SYSCTL_SDCLKFS_MASK | SDHC_SYSCTL_DVS_MASK)) ) //first, clear the DTOCV, SDCLKFS, and DVS fields of SDHC_SYSCTL
                      | SDHC_SYSCTL_DTOCV(0x0E)               //data timeout counter value=SDCLK x (DTOCV+213), DTOCV range is 0-0x0E
                      | SDHC_SYSCTL_SDCLKFS(minpres >> 1)     //SDCLK frequency selection=base clock/(1<<SDCLKFS)
                      | SDHC_SYSCTL_DVS(mindiv - 1)

                  );

    /* waiting for SD clock to stabilize  */
    while (0 == (SDHC_PRSSTAT & SDHC_PRSSTAT_SDSTB_MASK));


    /* enable ESDHC clock */
    SDHC_SYSCTL |= SDHC_SYSCTL_SDCLKEN_MASK;

    SDHC_IRQSTAT |= SDHC_IRQSTAT_DTOE_MASK;     //clear data timeout error flag
}


/*
 *  @brief      SDHC sends cmd command
 *  @param      command     command
 *  @return     send command result, 0 indicates success, 1 indicates error, -1 indicates timeout
 *  @since      v1.0
 */
uint32 SDHC_cmd (pESDHC_CMD_t command) /* [IN/OUT] Command specification */
{
    uint32  xfertyp;
    uint32  blkattr;

    //ASSERT(SDHC_CMD_MAX > command->COMMAND  );      //assert that the command cannot exceed SDHC_CMD-MAX

    /*check command*/
    xfertyp = ESDHC_COMMAND_XFERTYP[command->COMMAND ];

    ASSERT (~0 != xfertyp);                             //the assertion is that a valid CMD and xfertyp cannot be~0

    /* prepare to check the card insertion and removal status */
    SDHC_IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;

    /* waiting for CMD line to be idle */
    while (SDHC_PRSSTAT & SDHC_PRSSTAT_CIHB_MASK) {};

    /* set command */
    SDHC_CMDARG = command->ARGUMENT;
    xfertyp &= (~ SDHC_XFERTYP_CMDTYP_MASK);
    xfertyp |= SDHC_XFERTYP_CMDTYP(command->TYPE);
    if (ESDHC_TYPE_RESUME == command->TYPE)     //the recovery command must have DPSEL position bits
    {
        xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
    }

    if (0 != command->BLOCKS)
    {
        if ((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) != SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_48BUSY))
        {
            xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
        }
        if (command->READ)       //write data
        {
           xfertyp |= SDHC_XFERTYP_DTDSEL_MASK;
        }
        if (command->BLOCKS > 1)
        {
            xfertyp |= SDHC_XFERTYP_MSBSEL_MASK;
        }
        if ((uint32) - 1 != command->BLOCKS)
        {
            blkattr  = SDHC_BLKATTR_BLKCNT(command->BLOCKS) | SDHC_BLKATTR_BLKSIZE(SDCARD_BLOCK_SIZE);
            xfertyp |= SDHC_XFERTYP_BCEN_MASK;
        }
        else
        {
            blkattr = SDHC_BLKATTR_BLKCNT(0xFFFF)| SDHC_BLKATTR_BLKSIZE(SDCARD_BLOCK_SIZE) ;
        }
    }
    else
    {
        blkattr = 0;
    }

    SDHC_BLKATTR = blkattr;


    /* send order */
    SDHC_DSADDR = 0;
    SDHC_XFERTYP = xfertyp;

    /* waiting for response */
    SDHC_STATUS_WAIT (SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CCE_MASK | SDHC_IRQSTAT_CC_MASK);
    if (SDHC_STATUS_GET(SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CCE_MASK | SDHC_IRQSTAT_CC_MASK) != SDHC_IRQSTAT_CC_MASK)
    {
        SDHC_IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CCE_MASK | SDHC_IRQSTAT_CC_MASK;
        return ESDHC_CMD_TIMEOUT;
    }

    /* check the status of card insertion and removal */
    if (SDHC_IRQSTAT & SDHC_IRQSTAT_CRM_MASK)
    {
        SDHC_IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CC_MASK;
        return ESDHC_CMD_ERR;
    }

    /* obtain the corresponding if there is */
    if (SDHC_IRQSTAT & SDHC_IRQSTAT_CTOE_MASK)
    {
        SDHC_IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CC_MASK;
        return ESDHC_CMD_TIMEOUT;
    }

    // if there is a corresponding command sent
    if ((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) != SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
    {
        command->RESPONSE[0] = SDHC_CMDRSP(0);

        // if the appearance is received
        if ((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) == SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136))
        {
            command->RESPONSE[1] = SDHC_CMDRSP(1);
            command->RESPONSE[2] = SDHC_CMDRSP(2);
            command->RESPONSE[3] = SDHC_CMDRSP(3);
        }
    }

    //complete command
    SDHC_IRQSTAT |= SDHC_IRQSTAT_CC_MASK;

    return 0;
}



/*
 *  @brief      SDHC control command
 *  @param      ESDHC_IOCTL_CMD_e       command
 *  @return     ESDHC_IOCTL_ERR_e       operate result
 *  @since      v1.0
 */
ESDHC_IOCTL_ERR_e SDHC_ioctl(ESDHC_IOCTL_CMD_e cmd, void *param_ptr )
{
    ESDHC_CMD_t             command;
    uint8                   mem, io, mmc, ceata, mp, hc;
    int32                   val;
    ESDHC_IOCTL_ERR_e                  result ;
    uint32                  *param32_ptr = param_ptr;

    result = ESDHC_IOCTL_OK;

    switch (cmd)
    {
    case ESDHC_IOCTL_INIT:

        SDHC_init();        //Init SD card

        mem = FALSE;
        io  = FALSE;
        mmc = FALSE;
        ceata = FALSE;
        hc = FALSE;
        mp = FALSE;

        /* CMD0 - Go to idle - reset card */
        command.COMMAND = ESDHC_CMD0;
        command.TYPE = ESDHC_TYPE_NORMAL;
        command.ARGUMENT = 0;
        command.READ = FALSE;
        command.BLOCKS = 0;
        if (SDHC_cmd (&command))
        {
            result = ESDHC_INIT_ERR;
            break;
        }

        DELAY_MS(1100);//

        /* CMD8 - Send interface condition - check HC support */
        command.COMMAND = ESDHC_CMD8;
        command.TYPE = ESDHC_TYPE_NORMAL;
        command.ARGUMENT = 0x000001AA;
        command.READ = FALSE;
        command.BLOCKS = 0;
        val = SDHC_cmd (&command);

        if (val == 0)
        {
            // SDHC Card
            if (command.RESPONSE[0] != command.ARGUMENT)
            {
                result = ESDHC_INIT_ERR;
                break;
            }
            hc = TRUE;
        }

        mp = TRUE;

        if (mp)
        {
            /* CMD55 - Application specific command - check MMC */
            command.COMMAND = ESDHC_CMD55;
            command.TYPE = ESDHC_TYPE_NORMAL;
            command.ARGUMENT = 0;
            command.READ = FALSE;
            command.BLOCKS = 0;
            val = SDHC_cmd (&command);
            if (val > 0)
            {
                result = ESDHC_INIT_ERR;
                break;
            }
            if (val < 0)
            {
                /* MMC or CE-ATA */
                io = FALSE;
                mem = FALSE;
                hc = FALSE;

                /* CMD1 - Send operating conditions - check HC */
                command.COMMAND = ESDHC_CMD1;
                command.TYPE = ESDHC_TYPE_NORMAL;
                command.ARGUMENT = 0x40300000;
                command.READ = FALSE;
                command.BLOCKS = 0;
                if (SDHC_cmd (&command))
                {
                    result = ESDHC_INIT_ERR;
                    break;
                }
                if (0x20000000 == (command.RESPONSE[0] & 0x60000000))
                {
                    hc = TRUE;
                }
                mmc = TRUE;

                /* CMD39 - Fast IO - check CE-ATA signature CE */
                command.COMMAND = ESDHC_CMD39;
                command.TYPE = ESDHC_TYPE_NORMAL;
                command.ARGUMENT = 0x0C00;
                command.READ = FALSE;
                command.BLOCKS = 0;
                if (SDHC_cmd (&command))
                {
                    result = ESDHC_INIT_ERR;
                    break;
                }
                if (0xCE == (command.RESPONSE[0] >> 8) & 0xFF)
                {
                    /* CMD39 - Fast IO - check CE-ATA signature AA */
                    command.COMMAND = ESDHC_CMD39;
                    command.TYPE = ESDHC_TYPE_NORMAL;
                    command.ARGUMENT = 0x0D00;
                    command.READ = FALSE;
                    command.BLOCKS = 0;
                    if (SDHC_cmd (&command))
                    {
                        result = ESDHC_INIT_ERR;
                        break;
                    }
                    if (0xAA == (command.RESPONSE[0] >> 8) & 0xFF)
                    {
                        mmc = FALSE;
                        ceata = TRUE;
                    }
                }
            }
            else
            {
                /* SD */
                /* ACMD41 - Send Operating Conditions */
                command.COMMAND = ESDHC_ACMD41;
                command.TYPE = ESDHC_TYPE_NORMAL;
                command.ARGUMENT = 0;
                command.READ = FALSE;
                command.BLOCKS = 0;
                if (SDHC_cmd (&command))
                {
                    result = ESDHC_INIT_ERR;
                    break;
                }
                if (command.RESPONSE[0] & 0x300000)
                {
                    val = 0;
                    do
                    {
                        DELAY_MS(10);
                        val++;

                        /* CMD55 + ACMD41 - Send OCR */
                        //MMC card does not respond to CMD55+ACMD41 command, so this command can be used during initialization to distinguish between SD card and MMC card
                        command.COMMAND = ESDHC_CMD55;
                        command.TYPE = ESDHC_TYPE_NORMAL;
                        command.ARGUMENT = 0;
                        command.READ = FALSE;
                        command.BLOCKS = 0;
                        if (SDHC_cmd (&command))
                        {
                            result = ESDHC_INIT_ERR;
                            break;
                        }

                        command.COMMAND = ESDHC_ACMD41;
                        command.TYPE = ESDHC_TYPE_NORMAL;
                        if (hc)
                        {
                            command.ARGUMENT = 0x40300000;
                        }
                        else
                        {
                            command.ARGUMENT = 0x00300000;
                        }
                        command.READ = FALSE;
                        command.BLOCKS = 0;
                        if (SDHC_cmd (&command))
                        {
                            result = ESDHC_INIT_ERR;
                            break;
                        }
                    }
                    while ((0 == (command.RESPONSE[0] & 0x80000000)) && (val < SEND_OCR_TIME));
                    if (ESDHC_IOCTL_OK != result)
                    {
                        break;
                    }
                    if (val >= SEND_OCR_TIME)
                    {
                        hc = FALSE;
                    }
                    else
                    {
                        mem = TRUE;
                        if (hc)
                        {
                            hc = FALSE;
                            if (command.RESPONSE[0] & 0x40000000)
                            {
                                hc = TRUE;
                            }
                        }
                    }
                }
            }
        }


        if (mmc)
        {
            SDHC_card.CARD = ESDHC_CARD_MMC;
        }
        if (ceata)
        {
            SDHC_card.CARD = ESDHC_CARD_CEATA;
        }
        if (io)
        {
            SDHC_card.CARD = ESDHC_CARD_SDIO;
        }
        if (mem)
        {
            SDHC_card.CARD = ESDHC_CARD_SD;
            if (hc)
            {
                SDHC_card.CARD = ESDHC_CARD_SDHC;
            }
        }
        if (io && mem)
        {
            SDHC_card.CARD = ESDHC_CARD_SDCOMBO;
            if (hc)
            {
                SDHC_card.CARD = ESDHC_CARD_SDHCCOMBO;
            }
        }

        /* set ESDHC default baud rate */
        SDHC_set_baudrate (SDHC_BANDRATE);

        /* enable SDHC module clock */
        SIM_SCGC3 |= SIM_SCGC3_SDHC_MASK;
        
        break;
    case ESDHC_IOCTL_SEND_CMD:
        val = SDHC_cmd ((pESDHC_CMD_t)param32_ptr);
        if (val > 0)
        {
            result = ESDHC_CMD_ERR;
        }
        if (val < 0)
        {
            result = ESDHC_CMD_TIMEOUT;
        }
        break;
    case ESDHC_IOCTL_GET_BUS_WIDTH:
        if (NULL == param32_ptr)
        {
            result = ESDHC_PARAM_INVALID;
        }
        else
        {
            /* obtain ESDHC bus width */
            val = (SDHC_PROCTL & SDHC_PROCTL_DTW_MASK) >> SDHC_PROCTL_DTW_SHIFT;
            if (ESDHC_PROCTL_DTW_1BIT == val)
            {
                *param32_ptr = ESDHC_BUS_WIDTH_1BIT;
            }
            else if (ESDHC_PROCTL_DTW_4BIT == val)
            {
                *param32_ptr = ESDHC_BUS_WIDTH_4BIT;
            }
            else if (ESDHC_PROCTL_DTW_8BIT == val)
            {
                *param32_ptr = ESDHC_BUS_WIDTH_8BIT;
            }
            else
            {
                result = ESDHC_BUS_WIDTH_INVALID;
            }
        }
        break;
    case ESDHC_IOCTL_SET_BUS_WIDTH:
        if (NULL == param32_ptr)
        {
            result = ESDHC_PARAM_INVALID;
        }
        else
        {
            /* set ESDHC bus width */
            if (! SDHC_is_running())
            {
                if (ESDHC_BUS_WIDTH_1BIT == *param32_ptr)
                {
                    SDHC_PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
                    SDHC_PROCTL |= SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_1BIT);
                }
                else if (ESDHC_BUS_WIDTH_4BIT == *param32_ptr)
                {
                    SDHC_PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
                    SDHC_PROCTL |= SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_4BIT);
                }
                else if (ESDHC_BUS_WIDTH_8BIT == *param32_ptr)
                {
                    SDHC_PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
                    SDHC_PROCTL |= SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_8BIT);
                }
                else
                {
                    result = ESDHC_BUS_WIDTH_INVALID;
                }
            }
            else
            {
                result = ESDHC_BUSY;
            }
        }
        break;
    case ESDHC_IOCTL_GET_CARD:
        if (NULL == param32_ptr)
        {
            result = ESDHC_PARAM_INVALID;
        }
        else
        {
            /* 80 clocks to update levels */
            SDHC_SYSCTL |= SDHC_SYSCTL_INITA_MASK;
            while (SDHC_SYSCTL & SDHC_SYSCTL_INITA_MASK)
                { };

            /* Update and return actual card status */
            if (SDHC_IRQSTAT & SDHC_IRQSTAT_CRM_MASK)
            {
                SDHC_IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;
                SDHC_card.CARD = ESDHC_CARD_NONE;
            }
            if (SDHC_PRSSTAT & SDHC_PRSSTAT_CINS_MASK)
            {
                if (ESDHC_CARD_NONE == SDHC_card.CARD)
                {
                    SDHC_card.CARD = ESDHC_CARD_UNKNOWN;
                }
            }
            else
            {
                SDHC_card.CARD = ESDHC_CARD_NONE;
            }
            *param32_ptr = SDHC_card.CARD;
        }
        break;
    default:
        result = ESDHC_CMD_INVALID;
        break;
    }
    return result;
}