/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       can.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __CAN_H__
#define __CAN_H__

#include "common.h"

/*
**CAN module enumeration
*/
typedef enum
{
  CAN_CAN0,
  CAN_CAN_MAX,
}CAN_CANn;

/*
**MB buffer zone number
*/
typedef enum
{
  MB_NUM_0,
  MB_NUM_1,
  MB_NUM_2,
  MB_NUM_3,
  MB_NUM_4,
  MB_NUM_5,
  MB_NUM_6,
  MB_NUM_7,
  MB_NUM_8,
  MB_NUM_9,
  MB_NUM_10,
  MB_NUM_11,
  MB_NUM_12,
  MB_NUM_13,
  MB_NUM_14,
  MB_NUM_15,
  NUMBER_OF_MB,
}MB_NUM_n;

/*
**CAN work mode enumerate
*/
typedef enum
{
  CAN_NORMAL,   //normal mode
  CAN_LOOPBACK, //loop-back mode
}CAN_MODE;

/*
**CAN baud rate enumerate
*/
typedef enum
{
  CAN_BAUD_10K,
  CAN_BAUD_20K,
  CAN_BAUD_50K,
  CAN_BAUD_100K,
  CAN_BAUD_125K,
  CAN_BAUD_250K,
  CAN_BAUD_500K,
  CAN_BAUD_1M,
  CAN_BAUD_MAX,
}CAN_BAUD_n;

/*
**CAN bus clock source selection
*/
typedef enum
{
  CAN_CLKSRC_BUS,//Bus clock
  CAN_CLKSRC_OSC,//External OSC crystal oscillator
}CAN_CLKSRC_n;

/*
**CAN baud rate register configuration structure
*/
typedef struct
{
  uint32_t Baud;        //baud rate(KB/S)
  uint16_t Presdiv;     //division factor
  uint8_t  Prop_Seg;    //spread time period
  uint8_t  PSeg1;       //phase buffer segment 1
  uint8_t  PSeg2;       //phase buffer segment 2
  uint8_t  RJW;         //synchronous jump width
  uint8_t  Res[2];      //reversed
}CAN_Baud_CFG_n;

/*
**CAN User ID
*/
typedef struct
{
  uint32 ID:29;         //ID
  uint32 IDE:1;         //IDE   1: Extended ID, 0: Standard ID
  uint32 RTR:1;         //RTR   1: Remote frame, 0: Data frame
}CAN_USR_ID_n;

//CAN ID The lower the value, the higher the priority of the message
//CAN Mask is used for shielding

//Message buffer code word used for sending buffer
#define CAN_CS_CODE_TX_INACTIVE         B8(1000)            //MB is in an inactive state
#define CAN_CS_CODE_TX_ABORT            B8(1001)            //MB discarded
#define CAN_CS_CODE_TX_DATA             B8(1100)            //MB is a transmission data frame (MB's RTR bit is 0)
#define CAN_CS_CODE_TX_REMOTE           B8(1100)            //MB is a remote request frame for sending (with an RTR bit of 1)
#define CAN_CS_CODE_TX_TANSWER          B8(1110)            //MB is a sending response frame for remote request frames

//Message buffer code word used for receiving buffer
#define CAN_CS_CODE_RX_INACTIVE         B8(0000)            //MB is in an inactive state
#define CAN_CS_CODE_RX_EMPTY            B8(0100)            //MB activated and empty
#define CAN_CS_CODE_RX_FULL             B8(0010)            //MB is full
#define CAN_CS_CODE_RX_OVERRUN          B8(0110)            //Writing to a full buffer causes MB to be overwritten
#define CAN_CS_CODE_RX_RANSWER          B8(1010)            //A new frame is configured to confirm a remote request frame and send a reply frame
#define CAN_CS_CODE_RX_BUSY             B8(0001)            //Updating the content of MB CPU does not allow access to MB

/*
**variate declaration
*/
extern CAN_MemMapPtr CANN[CAN_CAN_MAX];

/*
**function declaration
*/
extern void CAN_Init(CAN_CANn CAN_CANx,CAN_BAUD_n CAN_BAUD_x,CAN_MODE CAN_MODE_x,CAN_CLKSRC_n CAN_CLKSRC_x);
extern void CAN_Tx(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x,CAN_USR_ID_n CAN_USR_ID_x,uint8 Len,void *Buff);
extern void CAN_RxBuff_Enable(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x,CAN_USR_ID_n CAN_USR_ID_x);
extern void CAN_RxBuff_Mask(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x,uint32 Mask,uint8 IsIRMQ);
extern void CAN_Rx(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x,CAN_USR_ID_n *CAN_USR_ID_x,uint8 *Len,void *Buff);
extern void CAN_Irq_En(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x);
extern void CAN_Clear_Flag(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x);
extern uint32 CAN_Get_Flag(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x);

#endif