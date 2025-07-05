/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       usb_cdc.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __USB_CDC_H__
#define __USB_CDC_H__

#include "usb.h"
#include "fsl_types.h"

#define CDC_INPointer   gu8EP2_IN_ODD_Buffer
#define CDC_OUTPointer  gu8EP3_OUT_ODD_Buffer

/* Defines */
#define CDC_BUFFER_SIZE 128
#define EP_OUT          EP3
#define EP_IN           EP2

/* USB_CDC Definitions */
#define WAITING_FOR_ENUMERATION 0x00
#define SET_LINE_CODING         0x20
#define GET_LINE_CODING         0x21
#define SET_CONTROL_LINE_STATE  0x22
#define LOADER_MODE             0xAA
#define GET_INTERFACE           0x0A
#define GET_STATUS              0x00
#define CLEAR_FEATURE           0x01
#define SET_FEATURE             0x03

/*
**Structure Types
*/
typedef struct
{
  uint32 DTERate;
  uint8  CharFormat;
  uint8  ParityType;
  uint8  Databits;
}CDC_Line_Coding;

/*
**variate declaration
*/
extern uint8 u8CDCState;
extern uint8 CDC_OUT_Data[];
extern CDC_Line_Coding com_cfg;

/*
**function declaration
*/
extern void CDC_Init(void);
extern void CDC_Engine(void);
extern uint8 CDC_InterfaceReq_Handler(void);

#endif