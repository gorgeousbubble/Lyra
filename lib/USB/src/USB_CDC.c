/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       usb_cdc.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "usb_cdc.h"

/*
**CDC Global Structures
*/
CDC_Line_Coding com_cfg;
uint8 u8CDCState = WAITING_FOR_ENUMERATION;
uint8 CDC_OUT_Data[CDC_BUFFER_SIZE];

/*
 *  @brief      CDC class initialization function
 *  @since      v1.0
 */
void CDC_Init(void)
{
  //configure serial port features
  com_cfg.DTERate      = 9600;                //baud rate
  com_cfg.CharFormat   = 0;                   //character format
  com_cfg.ParityType   = 0;                   //polarity type
  com_cfg.Databits     = 0x08;                //number of data bits
}

/**********************************************************/
void CDC_Engine(void)
{
  //uint16 u8RecData;
  /* control Stage */
  switch(u8CDCState)
  {
    case WAITING_FOR_ENUMERATION:
    /* Waiting for USB enumeration */
    while(gu8USB_State != uENUMERATED)
    {
      (void)u8CDCState;
    };
    
    u8CDCState = 0;
    break;

    case SET_LINE_CODING:
    if(FLAG_CHK(EP0, gu8USB_Flags))
    {
      FLAG_CLR(EP0, gu8USB_Flags);
      (void)EP_OUT_Transfer(EP0, (uint8 *)&com_cfg);
      EP_IN_Transfer(EP0, 0, 0);
    }
    break;

    case SET_CONTROL_LINE_STATE:
    EP_IN_Transfer(EP0, 0, 0);
    break;
    
  }
}

/*
 *  @brief      The current interface request is an interrupt service function for CDC class requests
 *  @note       Call in USB_SetupHHandler interrupt
 *  @since      v1.0
 */
uint8 CDC_InterfaceReq_Handler(void)
{
  uint8 u8State = uSETUP;

  switch(Setup_Pkt->bRequest)
  {
    case GET_INTERFACE:
        EP_IN_Transfer(EP0, &gu8Interface, 1);
        break;

    case GET_LINE_CODING:
        EP_IN_Transfer(EP0, (uint8 *)&com_cfg, 7);
        break;

    case SET_LINE_CODING:
        u8CDCState = SET_LINE_CODING;
        u8State = uDATA;
        break;

    case SET_CONTROL_LINE_STATE:
        u8CDCState = SET_CONTROL_LINE_STATE;
        u8State = uSETUP;
        break;

    case LOADER_MODE:
        FLAG_SET(LOADER, gu8USB_Flags);
        CDC_OUT_Data[0] = 0xFF;
        break;
    default:
        break;
  }
  return(u8State);
}