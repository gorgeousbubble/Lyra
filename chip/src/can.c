/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       can.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "can.h"
#include "conf.h"
#include "misc.h"
#include "port.h"
#include "port_cfg.h"
#include "system_init.h"

/*
**CAN base pointer
*/
CAN_MemMapPtr CANN[CAN_CAN_MAX]=
{
  CAN0_BASE_PTR,//CAN0 base pointer
};

/*
**CAN clock source 50MHz
*/
CAN_Baud_CFG_n CAN_Baud_CFG_50000K[CAN_BAUD_MAX]=
{
  //BAND,PRESDIV,PROP_SEG,PSEG1,PSEG2,RJW
  {10,  624,    4,      0,      0,      0},//sample:87.50%
  {20,  124,    12,     2,      2,      2},//sample:85.00%
  {50,  124,    4,      0,      0,      0},//sample:87.50%
  {100, 24,     12,     2,      2,      2},//sample:85.00%
  {125, 24,     10,     1,      1,      1},//sample:87.50%
  {250, 24,     4,      0,      0,      0},//sample:87.50%
  {500, 4,      12,     2,      2,      2},//sample:85.00%
  {1000,1,      9,      6,      6,      3},//sample:72.00%
};

/*
**CAN clock source 60MHz
*/
CAN_Baud_CFG_n CAN_Baud_CFG_60000K[CAN_BAUD_MAX]=
{
  //BAND,PRESDIV,PROP_SEG,PSEG1,PSEG2,RJW
  {10,  249,    16,     2,      2,      2},//sample:87.50%
  {20,  124,    16,     2,      2,      2},//sample:87.50%
  {50,  49,     16,     2,      2,      2},//sample:87.50%
  {100, 24,     16,     2,      2,      2},//sample:87.50%
  {125, 19,     16,     2,      2,      2},//sample:87.50%
  {250, 9,      16,     2,      2,      2},//sample:87.50%
  {500, 4,      16,     2,      2,      2},//sample:87.50%
  {1000,2,      8,      4,      4,      3},//sample:75.00%
};

/*
 *  @brief      set CAN baud rate
 *  @param      CAN_CANn        CAN_CANx        CAN moudule
 *  @param      CAN_BAUD_n      CAN_BAUD_x      baud rate number
 *  @since      v1.0
 *  Sample usage:       CAN_Set_Baud(CAN1,CAN_BAUD_20K)
 */
static void CAN_Set_Baud(CAN_CANn CAN_CANx,CAN_BAUD_n CAN_BAUD_x)
{
  CAN_Baud_CFG_n *PCAN_Baud;
  uint32 CAN_CLK_KHz=0;
  uint8 BFreezeMode=0;
  
  if(CAN_CTRL1_REG(CANN[CAN_CANx]) & CAN_CTRL1_CLKSRC_MASK)
  {
    CAN_CLK_KHz = MK64_Bus_KHz;         //Bus clock
  }
  else
  {
    CAN_CLK_KHz = EXTAL_IN_MHz * 1000;  //Crystal oscillator OSC clock
  }
  
  if(CAN_CLK_KHz == 50000)
  {
    PCAN_Baud = &CAN_Baud_CFG_50000K[CAN_BAUD_x];
  }
  else if(CAN_CLK_KHz == 60000)
  {
    PCAN_Baud = &CAN_Baud_CFG_60000K[CAN_BAUD_x];
  }
  else
  {
    ASSERT(0);
  }
  
  //Enter freeze mode
  if(!(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_HALT_MASK))
  {
    CAN_MCR_REG(CANN[CAN_CANx]) |= CAN_MCR_HALT_MASK;
    
    while(!(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_FRZACK_MASK));
    
    BFreezeMode = 0;
  }
  else
  {
    BFreezeMode = 1;
  }
  
  //Configuration Register
  CAN_CTRL1_REG(CANN[CAN_CANx]) &= ~(0
                                     | CAN_CTRL1_PROPSEG_MASK
                                     | CAN_CTRL1_RJW_MASK
                                     | CAN_CTRL1_PSEG1_MASK
                                     | CAN_CTRL1_PSEG2_MASK
                                     | CAN_CTRL1_PRESDIV_MASK
                                     );
  
  CAN_CTRL1_REG(CANN[CAN_CANx]) |= (0
                                    | CAN_CTRL1_PROPSEG(PCAN_Baud->Prop_Seg)
                                    | CAN_CTRL1_RJW(PCAN_Baud->RJW)
                                    | CAN_CTRL1_PSEG1(PCAN_Baud->PSeg1)
                                    | CAN_CTRL1_PSEG2(PCAN_Baud->PSeg2)
                                    | CAN_CTRL1_PRESDIV(PCAN_Baud->Presdiv)
                                    );
  
  //Restore CAN operation mode
  if(!BFreezeMode)
  {
    CAN_MCR_REG(CANN[CAN_CANx]) &= ~CAN_MCR_HALT_MASK;
    
    while(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_FRZACK_MASK);
    
    while(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_NOTRDY_MASK);
  }
  
}

/*
 *  @brief      CAN Initialization
 *  @param      CAN_CANn        CAN_CANx        CAN module number
 *  @param      CAN_BAUD_n      CAN_BAUD_x      baud rate number
 *  @param      CAN_MODE        CAN_MODE_x      CAN mode(Normal mode, loopback mode)
 *  @param      CAN_CLKSRC_n    CAN_CLKSRC_x    CAN clock source(Bus clock, external crystal oscillator)
 *  @since      v1.0
 *  Sample usage:       CAN_Init(CAN1,CAN_BAUD_20K,CAN_LOOPBACK);   //Initialize CAN1, baud rate 20Kb/s, loopback mode
 */
void CAN_Init(CAN_CANn CAN_CANx,CAN_BAUD_n CAN_BAUD_x,CAN_MODE CAN_MODE_x,CAN_CLKSRC_n CAN_CLKSRC_x)
{
  uint8 i=0;
  
  switch(CAN_CANx)
  {
    case CAN_CAN0:
                  SIM_SCGC6 |= SIM_SCGC6_FLEXCAN0_MASK;//CAN0 clock enable
                  PORT_Init(CAN0_TX_PIN,ALT2);
                  PORT_Init(CAN0_RX_PIN,ALT2);
                  break;
    default:
                  ASSERT(0);
                  break;
  }
  
  if(CAN_CLKSRC_x == CAN_CLKSRC_BUS)
  {
    CAN_CTRL1_REG(CANN[CAN_CANx]) |= CAN_CTRL1_CLKSRC_MASK;//Bus clock
  }
  else
  {
    OSC_CR |= OSC_CR_ERCLKEN_MASK;//External crystal oscillator clock OSC
  }
  
  CAN_MCR_REG(CANN[CAN_CANx]) &= ~CAN_MCR_MDIS_MASK;            //CAN module enable
  CAN_MCR_REG(CANN[CAN_CANx]) |= CAN_MCR_FRZ_MASK;              //CAN freeze module enable
  while(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_LPMACK_MASK);     //Waiting for reset
  CAN_MCR_REG(CANN[CAN_CANx]) |= CAN_MCR_SOFTRST_MASK;          //Software Reset
  while(!(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_FRZACK_MASK));  //Waiting to enter freeze mode
  
  for(i=16;i>0;i--)
  {
    CAN_CS_REG(CANN[CAN_CANx],i) = 0;
    CAN_ID_REG(CANN[CAN_CANx],i) = 0;
    CAN_WORD0_REG(CANN[CAN_CANx],i) = 0;
    CAN_WORD1_REG(CANN[CAN_CANx],i) = 0;
  }
  
  CAN_CTRL2_REG(CANN[CAN_CANx]) = (0 | CAN_CTRL2_TASD(22));
  CAN_MCR_REG(CANN[CAN_CANx]) = (CAN_MCR_REG(CANN[CAN_CANx]) & (~CAN_MCR_IDAM_MASK)) | CAN_MCR_IDAM(0);
  
  //set baud rate
  CAN_Set_Baud(CAN_CANx,CAN_BAUD_x);
  
  if(CAN_MODE_x == CAN_LOOPBACK)
  {
    CAN_CTRL1_REG(CANN[CAN_CANx]) |= CAN_CTRL1_LPB_MASK;//loop mode
  }
  
  //Default independent mask
  CAN_MCR_REG(CANN[CAN_CANx]) |= CAN_MCR_IRMQ_MASK;
  
  for(i=NUMBER_OF_MB;i>0;i--)
  {
    CAN_RXIMR_REG(CANN[CAN_CANx],i) = 0;
  }
  
  CAN_RXMGMASK_REG(CANN[CAN_CANx]) = 0;
  CAN_RX14MASK_REG(CANN[CAN_CANx]) = 0;
  CAN_RX15MASK_REG(CANN[CAN_CANx]) = 0;
  
  //Complete configuration in freeze mode
  CAN_MCR_REG(CANN[CAN_CANx]) &= ~CAN_MCR_HALT_MASK;
  
  //Waiting for synchronization to exit frozen mode
  while(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_FRZACK_MASK);
  
  //Waiting not in freeze mode, sleep mode, stop mode
  while(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_NOTRDY_MASK);
  
}

/*
 *  @brief      CAN send data
 *  @param      CAN_CANn        CAN_CANx        CAN module number
 *  @param      MB_NUM_n        MB_NUM_x        buffer number
 *  @param      CAN_USR_ID_n    CAN_USR_ID_x    ID number
 *  @param      uint8           Len             data length
 *  @param      void*           Buff            buff address
 *  @since      v1.0
 *  Sample usage:       CAN_Tx(CAN1,CAN_TX_MB,can_tx_id,DATA_LEN, txbuff);  //CAN sends data Buffer CAN_TX_MB, Message ID: tx-ID, Data buffer txbuff, Length DATA_LEN
 */
void CAN_Tx(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x,CAN_USR_ID_n CAN_USR_ID_x,uint8 Len,void *Buff)
{
  uint32 Word=0;
  
  ASSERT(Len <= 8);
  
  CAN_CS_REG(CANN[CAN_CANx],MB_NUM_x) = (0
                                         | CAN_CS_CODE(CAN_CS_CODE_TX_INACTIVE)         //Write inactive code in the buffer
                                         | (CAN_USR_ID_x.IDE << CAN_CS_IDE_SHIFT)       //Buffer Write IDE Bit
                                         | (CAN_USR_ID_x.RTR << CAN_CS_RTR_SHIFT)       //Buffer write RTR bit
                                         | CAN_CS_DLC(Len)                              //Buffer write data length
                                         );
  
  if(CAN_USR_ID_x.IDE)
  {
    CAN_ID_REG(CANN[CAN_CANx],MB_NUM_x) = (0 | CAN_ID_PRIO(1) | CAN_ID_EXT(CAN_USR_ID_x.ID));
  }
  else
  {
    CAN_ID_REG(CANN[CAN_CANx],MB_NUM_x) = (0 | CAN_ID_PRIO(1) | CAN_ID_STD(CAN_USR_ID_x.ID));
  }
  
  Word = *(uint32*)Buff;
  CAN_WORD0_REG(CANN[CAN_CANx],MB_NUM_x) = SWAP32(Word);
  Word = *((uint32*)Buff + 1);
  CAN_WORD1_REG(CANN[CAN_CANx],MB_NUM_x) = SWAP32(Word);
  
  //Start send
  CAN_CS_REG(CANN[CAN_CANx],MB_NUM_x) = (0
                                         | CAN_CS_CODE(CAN_CS_CODE_TX_DATA)
                                         | CAN_CS_DLC(Len)
                                         );
  
  //Time limited waiting for sending completion
  while(!(CAN_IFLAG1_REG(CANN[CAN_CANx]) & (1 << MB_NUM_x)));
  
  //Clear message buffer interrupt flag
  CAN_IFLAG1_REG(CANN[CAN_CANx]) = (1 << MB_NUM_x);
  
}

/*
 *  @brief      Enable CAN receive buffer
 *  @param      CAN_CANn        CAN_CANx        CAN module number
 *  @param      MB_NUM_n        MB_NUM_x        buffer number
 *  @param      CAN_USR_ID_n    CAN_USR_ID_x    ID number
 *  @since      v1.0
 *  Sample usage:      CAN_RxBuff_Enble(CAN1,CAN_RX_MB,can_my_id);//Enable receiving buffer
 */
void CAN_RxBuff_Enable(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x,CAN_USR_ID_n CAN_USR_ID_x)
{
  //Configure MB to inactive state
  CAN_CS_REG(CANN[CAN_CANx],MB_NUM_x) = CAN_CS_CODE(CAN_CS_CODE_RX_INACTIVE);
  
  if(CAN_USR_ID_x.IDE)
  {
    CAN_ID_REG(CANN[CAN_CANx],MB_NUM_x) = (0 | CAN_ID_EXT(CAN_USR_ID_x.ID));
    
    CAN_CS_REG(CANN[CAN_CANx],MB_NUM_x) = (0
                                           | CAN_CS_IDE_MASK
                                           | (CAN_USR_ID_x.RTR << CAN_CS_IDE_SHIFT)
                                           | CAN_CS_CODE(CAN_CS_CODE_RX_EMPTY)
                                           ); 
  }
  else
  {
    CAN_ID_REG(CANN[CAN_CANx],MB_NUM_x) = (0 | CAN_ID_STD(CAN_USR_ID_x.ID));
    
    CAN_CS_REG(CANN[CAN_CANx],MB_NUM_x) = (0
                                           | (CAN_USR_ID_x.RTR << CAN_CS_IDE_SHIFT)
                                           | CAN_CS_CODE(CAN_CS_CODE_RX_EMPTY)
                                           ); 
  }
  
}

/*
 *  @brief      CAN Receive mask configuration
 *  @param      CAN_CANn        CAN_CANx        CAN number
 *  @param      MB_NUM_n        MB_NUM_x        buffer number
 *  @param      uint32          Mask            mask code
 *  @param      uint8           IsIRMQ          independent mask (0 is the global mask, others are independent masks)
 *  @since      v5.0
 *  Sample usage:      CAN_RxBuff_Mask(CAN1,CAN_RX_MB,0x00FF,1);//The CAN_SX_MB buffer configuration mask for CAN1 is 0x00FF
 */
void CAN_RxBuff_Mask(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x,uint32 Mask,uint8 IsIRMQ)
{
  uint8 BFreezeMode=0;
  
  //Enter freeze mode
  if(!(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_HALT_MASK))
  {
    CAN_MCR_REG(CANN[CAN_CANx]) |= CAN_MCR_HALT_MASK;
    
    //Waiting to enter freeze mode
    while(!(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_FRZACK_MASK));
    
    BFreezeMode = 0;
  }
  else
  {
    BFreezeMode = 1;
  }
  
  if(IsIRMQ != 0)//Independent Mask
  {
    CAN_MCR_REG(CANN[CAN_CANx]) |= CAN_MCR_IRMQ_MASK;
    CAN_RXIMR_REG(CANN[CAN_CANx],MB_NUM_x) = Mask;
  }
  else
  {
    CAN_MCR_REG(CANN[CAN_CANx]) &= ~CAN_MCR_IRMQ_MASK;
    
    if(MB_NUM_x == MB_NUM_14)
    {
      CAN_RX14MASK_REG(CANN[CAN_CANx]) = Mask;
    }
    else if(MB_NUM_x == MB_NUM_15)
    {
      CAN_RX15MASK_REG(CANN[CAN_CANx]) = Mask;
    }
    else
    {
      CAN_RXMGMASK_REG(CANN[CAN_CANx]) = Mask;
    }
  }
  
  if(!BFreezeMode)
  {
    CAN_MCR_REG(CANN[CAN_CANx]) &= ~CAN_MCR_HALT_MASK;
    
    while(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_FRZACK_MASK);
    
    while(CAN_MCR_REG(CANN[CAN_CANx]) & CAN_MCR_NOTRDY_MASK);
  }
  
}

/*
 *  @brief      CAN recv data
 *  @param      CAN_CANn        CAN_CANx        CAN module number
 *  @param      MB_NUM_n        MB_NUM_x        buffer number
 *  @param      CAN_USR_ID_n    CAN_USR_ID_x    ID number
 *  @param      uint8           Len             data length
 *  @param      void*           Buff            buffer address
 *  @since      v1.0
 *  Sample usage:       CAN_Rx(CAN1,CAN_RX_MB,&can_rx_id,&can_rx_len,can_rx_data);  //CAN receives data from CAN_SX_MB, and the received ID is saved in can_rx_id, the length is saved in can_rx_len, and the data is saved in can_rx_data
 */
void CAN_Rx(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x,CAN_USR_ID_n *CAN_USR_ID_x,uint8 *Len,void *Buff)
{
  uint8 Lenth=0;
  uint32 Word=0;
  
  *(uint32*)CAN_USR_ID_x = 0;
  
  if((CAN_CS_REG(CANN[CAN_CANx],MB_NUM_x) & CAN_CS_CODE_MASK) != CAN_CS_CODE(CAN_CS_CODE_RX_FULL))
  {
    *Len = 0;
    return;
  }
  
  Lenth = (CAN_CS_REG(CANN[CAN_CANx],MB_NUM_x) & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT;
  
  if(Lenth < 1)
  {
    *Len = 0;
    return;
  }
  
  if(!(CAN_CS_REG(CANN[CAN_CANx],MB_NUM_x) & CAN_CS_IDE_MASK))
  {
    CAN_USR_ID_x->ID = (CAN_ID_REG(CANN[CAN_CANx],MB_NUM_x) & CAN_ID_STD_MASK) >> CAN_ID_STD_SHIFT;
  }
  else
  {
    CAN_USR_ID_x->ID = (CAN_ID_REG(CANN[CAN_CANx],MB_NUM_x) & CAN_ID_EXT_MASK) >> CAN_ID_EXT_SHIFT;
    CAN_USR_ID_x->IDE = 1;
  }
  
  if(CAN_CS_REG(CANN[CAN_CANx],MB_NUM_x) & CAN_CS_RTR_MASK)
  {
    CAN_USR_ID_x->RTR = 1;
  }
  
  Word = CAN_WORD0_REG(CANN[CAN_CANx],MB_NUM_x);
  *((uint32 *)Buff) = SWAP32(Word);
  
  Word = CAN_WORD1_REG(CANN[CAN_CANx],MB_NUM_x);
  *((uint32 *)Buff + 1) = SWAP32(Word);
  
  *Len = Lenth;
  
  CAN_TIMER_REG(CANN[CAN_CANx]);
}

/*
 *  @brief      Enable CAN buffer receive and send interrupts
 *  @param      CAN_CANn        CAN_CANx        CAN module number
 *  @param      MB_NUM_n        MB_NUM_x        buffer number
 *  @since      v1.0
 *  Sample usage:       CAN_Irq_En(CAN1,MB_NUM_5);
 */
void CAN_Irq_En(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x)
{
  CAN_IMASK1_REG(CANN[CAN_CANx]) = (1 << MB_NUM_x);
  
  if(CAN_CANx == CAN_CAN0)
  {
    enable_irq(CAN0_ORed_Message_buffer_IRQn);//CANO IRQ interrupts
  }
  else
  {
    ASSERT(0);
  }
  
}

/*
 *  @brief      Clear CAN buffer interrupt flag
 *  @param      CAN_CANn        CAN_CANx        CAN module number
 *  @param      MB_NUM_n        MB_NUM_x        buffer number
 *  @since      v1.0
 *  Sample usage:       CAN_Clear_Flag(CAN1,MB_NUM_5);
 */
void CAN_Clear_Flag(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x)
{
  CAN_IFLAG1_REG(CANN[CAN_CANx]) = (1 << MB_NUM_x);
}

/*
 *  @brief      Obtain CAN buffer interrupt flag
 *  @param      CAN_CANn        CAN_CANx        CAN module number
 *  @param      MB_NUM_n        MB_NUM_x        buffer number
 *  @since      v1.0
 *  Sample usage:       CAN_Get_Flag(CAN1,MB_NUM_5);
 */
uint32 CAN_Get_Flag(CAN_CANn CAN_CANx,MB_NUM_n MB_NUM_x)
{
  return (CAN_IFLAG1_REG(CANN[CAN_CANx]) & (1 << MB_NUM_x));
}