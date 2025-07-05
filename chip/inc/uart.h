/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       uart.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __UART_H__
#define __UART_H__

#include "common.h"

/*
**UART port enumeration (UART0~UART4)
*/
typedef enum
{
  UART_UART0,
  UART_UART1,
  UART_UART2,
  UART_UART3,
  UART_UART4,
  UART_UART5,
  UART_UART_MAX,
}UART_UARTn;


/*
**variate declaration
*/
extern UART_MemMapPtr UARTN[UART_UART_MAX];

/*
**function declaration
*/
extern void UART_Init(UART_UARTn UART_UARTx,uint32 UART_Baud);
extern void UART_GetChar(UART_UARTn UART_UARTx,char *ch);
extern void UART_PutChar(UART_UARTn UART_UARTx,char ch);
extern void UART_PutStr(UART_UARTn UART_UARTx,const uint8 *str);
extern void UART_Rx_IRQ_En(UART_UARTn UART_UARTx);
extern void UART_Tx_IRQ_En(UART_UARTn UART_UARTx);
extern void UART_Rx_IRQ_Dis(UART_UARTn UART_UARTx);
extern void UART_Tx_IRQ_Dis(UART_UARTn UART_UARTx);


#endif