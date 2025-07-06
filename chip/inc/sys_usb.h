/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       usb.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __SYS_USB_H__
#define __SYS_USB_H__

#include "common.h"

//USB configuration related macros
#define USB_CLOCK_PLL                   //USB enable PLL clock
#define USB_CLK_DIV             4       //USB clock source frequency division factor
#define USB_CLK_FRAC            1       //USB clock source multiplication factor

/*
**function declaration
*/
extern void USB_Init(void);
extern void USB_COM_Init(void);
extern void USB_Enum_Wait(void);
extern uint8 USB_COM_Rx(uint8_t *Rx_Buf);
extern void USB_COM_Tx(uint8 *Tx_Buf,uint8 Len);

#endif