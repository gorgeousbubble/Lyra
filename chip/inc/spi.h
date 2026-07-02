/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       spi.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __SPI_H__
#define __SPI_H__

#include "common.h"

/*
**SPI module enumeration
*/
typedef enum
{
  SPI_SPI0,
  SPI_SPI1,
  SPI_SPI2,
  SPI_SPI_MAX,
} SPI_SPIn;

/*
**SPI mode enumeration
*/
typedef enum
{
  MASTER,
  SLAVE,
} SPI_CFG;

/*
**SPI module chip selection enumeration
*/
typedef enum
{
  SPI_PCS0 = 1 << 0,
  SPI_PCS1 = 1 << 1,
  SPI_PCS2 = 1 << 2,
  SPI_PCS3 = 1 << 3,
  SPI_PCS4 = 1 << 4,
  SPI_PCS5 = 1 << 5,
} SPI_PCSn;

/*
**SPI macro
*/
/* Upper bound on any SPI status-flag spin-wait. Large enough never to trip
 * during a normal byte transfer (a byte takes microseconds; this is tens of
 * milliseconds of polling), but finite so an unresponsive SPI device (LCD,
 * W25Q80 flash) can never hang the whole system. */
#define SPI_WAIT_TIMEOUT 1000000UL

#define SPI_TX_WAIT(SPIn) while ((SPI_SR_REG(SPIN[SPIn]) & SPI_SR_TXRXS_MASK) == 1)
#define SPI_RX_WAIT(SPIn) while ((SPI_SR_REG(SPIN[SPIn]) & SPI_SR_RFDF_MASK) == 0)
#define SPI_EOQF_WAIT(SPIn)                                                        \
  do {                                                                             \
    volatile uint32 _spi_eoq_to = SPI_WAIT_TIMEOUT;                                \
    while (((SPI_SR_REG(SPIN[SPIn]) & SPI_SR_EOQF_MASK) == 0) && (--_spi_eoq_to))  \
    { }                                                                            \
  } while (0)

/*
**variate declaration
*/
extern SPI_MemMapPtr SPIN[SPI_SPI_MAX];

/*
**function declaration
*/
extern uint32 SPI_Init(SPI_SPIn SPI_SPIx, SPI_PCSn SPI_PCSx, SPI_CFG SPI_cfg, uint32 SPI_Baud);
extern void SPI_Extra_PCS_Init(SPI_SPIn SPI_SPIx, SPI_PCSn SPI_PCSx);
extern void SPI_MOSI(SPI_SPIn SPI_SPIx, SPI_PCSn SPI_PCSx, uint8 *SPI_MO_Data, uint8 *SPI_MI_Data, uint32 SPI_Len);
extern void SPI_MOSI_CMD(SPI_SPIn SPI_SPIx, SPI_PCSn SPI_PCSx, uint8 *SPI_MO_CMD, uint8 *SPI_MI_CMD, uint8 *SPI_MO_Data, uint8 *SPI_MI_Data, uint32 SPI_CMD_Len, uint32 SPI_Len);
extern void SPI_Send(SPI_SPIn SPI_SPIx, SPI_PCSn SPI_PCSx, uint8 Data);

#endif