/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       uart.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "misc.h"
#include "port_cfg.h"
#include "system_init.h"
#include "uart.h"

/*
**UART base pointer
*/
UART_MemMapPtr UARTN[UART_UART_MAX]=
{
  UART0_BASE_PTR,//UART0 base pointer
  UART1_BASE_PTR,//UART1 base pointer
  UART2_BASE_PTR,//UART2 base pointer
  UART3_BASE_PTR,//UART3 base pointer
  UART4_BASE_PTR,//UART4 base pointer
  UART5_BASE_PTR,//UART5 base pointer
};

/*
 *  @brief      Initialize serial UART and set baud rate
 *  @param      UART_UARTn      UART_UARTx      module number(UART0~UART4)
 *  @param      uint32          UART_Baud       baud rate, such as 9600¡¢19200¡¢56000¡¢115200...
 *  @since      v1.0
 *  Sample usage:       UART_Init(UART3,115200);        //Initialize the serial port with a baud rate of 115200
 */
void UART_Init(UART_UARTn UART_UARTx,uint32 UART_Baud)
{
  uint8 UART_Temp=0;
  uint32 UART_SysClk=0;
  register uint16 UART_Sbr=0;
  register uint16 UART_Brfa=0;
  
  //UART pin reuse
  switch(UART_UARTx)
  {
    case UART_UART0:
                    SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;//UART0 clock enable
                    if(UART0_RX_PIN == PTA1)
                    {
                      PORT_Init(UART0_RX_PIN,ALT2);
                    }
                    else if((UART0_RX_PIN == PTA15) || (UART0_RX_PIN == PTB16) || (UART0_RX_PIN == PTD6))
                    {
                      PORT_Init(UART0_RX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    
                    if(UART0_TX_PIN == PTA2)
                    {
                      PORT_Init(UART0_TX_PIN,ALT2);
                    }
                    else if((UART0_TX_PIN == PTA14) || (UART0_TX_PIN == PTB17) || (UART0_TX_PIN == PTD7))
                    {
                      PORT_Init(UART0_TX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    break;
    case UART_UART1:
                    SIM_SCGC4 |= SIM_SCGC4_UART1_MASK;//UART1 clock enable
                    if((UART1_RX_PIN == PTC3) || (UART1_RX_PIN == PTE1))
                    {
                      PORT_Init(UART1_RX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    
                    if((UART1_TX_PIN == PTC4) || (UART1_TX_PIN == PTE0))
                    {
                      PORT_Init(UART1_TX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    break;
    case UART_UART2:
                    SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;//UART2 clock enable
                    if(UART2_RX_PIN == PTD2)
                    {
                      PORT_Init(UART2_RX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    
                    if(UART2_TX_PIN == PTD3)
                    {
                      PORT_Init(UART2_TX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    break;
    case UART_UART3:
                    SIM_SCGC4 |= SIM_SCGC4_UART3_MASK;//UART3 clock enable
                    if((UART3_RX_PIN == PTB10)|| (UART3_RX_PIN == PTC16) || (UART3_RX_PIN == PTE5))
                    {
                      PORT_Init(UART3_RX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    
                    if((UART3_TX_PIN == PTB11) || (UART3_TX_PIN == PTC17) || (UART3_TX_PIN == PTE4))
                    {
                      PORT_Init(UART3_TX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    break;
    case UART_UART4:
                    SIM_SCGC1 |= SIM_SCGC1_UART4_MASK;//UART4 clock enable
                    if((UART4_RX_PIN == PTC14) || (UART4_RX_PIN == PTE25))
                    {
                      PORT_Init(UART4_RX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    
                    if((UART4_TX_PIN == PTC15) || (UART4_TX_PIN == PTE24))
                    {
                      PORT_Init(UART4_TX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    break;
    case UART_UART5:
                    SIM_SCGC1 |= SIM_SCGC1_UART5_MASK;
                    if((UART5_RX_PIN == PTD8) || (UART5_RX_PIN == PTE9))
                    {
                      PORT_Init(UART5_RX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    
                    if((UART5_TX_PIN == PTD9) ||(UART5_TX_PIN == PTE8))
                    {
                      PORT_Init(UART5_TX_PIN,ALT3);
                    }
                    else
                    {
                      ASSERT(0);
                    }
                    break;          
    default:
                    ASSERT(0);
                    break;
  }
  
  //UART register configuration
  UART_C2_REG(UARTN[UART_UARTx]) &= ~(0 | UART_C2_RE_MASK | UART_C2_TE_MASK);//Prohibit accepting and sending
  UART_C1_REG(UARTN[UART_UARTx]) = 0;
  
  //UART clock setting
  if(UART_UARTx == UART_UART0 || UART_UARTx == UART_UART1)//UART0,UART1
  {
    UART_SysClk = MK64_Core_KHz * 1000;//Core clock
  }
  else
  {
    UART_SysClk = MK64_Bus_KHz * 1000;//Bus clock
  }
  
  //UART baud rate UART_Baud = UART_SysClk / (16 * (SBR + BRFD))
  UART_Sbr = (uint16)(UART_SysClk / (UART_Baud * 16));
  
  if(UART_Sbr > 0x1FFF)
  {
    UART_Sbr = 0x1FFF;
  }
  
  //BRFA baud rate adjustment value
  UART_Brfa = (UART_SysClk / UART_Baud) - (UART_Sbr * 16);
  ASSERT(UART_Brfa <= 0x1F);
  
  //UART configuration
  UART_Temp = UART_BDH_REG(UARTN[UART_UARTx]) & (~UART_BDH_SBR_MASK);//Save BDH register and other settings
  UART_BDH_REG(UARTN[UART_UARTx]) = UART_Temp | UART_BDH_SBR(UART_Sbr >> 8);//BDH baud rate high bit write
  UART_BDL_REG(UARTN[UART_UARTx]) = UART_BDL_SBR(UART_Sbr & 0xff);//BDL baud rate low write
  
  UART_Temp = UART_C4_REG(UARTN[UART_UARTx]) & (~UART_C4_BRFA_MASK);//Save other settings of C4 register
  UART_C4_REG(UARTN[UART_UARTx]) = UART_Temp | UART_C4_BRFA(UART_Brfa);//C4 writes BRFA baud rate adjustment value
  
  //UART setting FIFO
  UART_PFIFO_REG(UARTN[UART_UARTx]) = (0 | UART_PFIFO_RXFE_MASK | UART_PFIFO_TXFE_MASK);
  
  //Allow sending and receiving
  UART_C2_REG(UARTN[UART_UARTx]) |= (0 | UART_C2_RE_MASK | UART_C2_TE_MASK);//Allow sending and receiving
  
}

/*
 *  @brief      UART accepts 1 byte
 *  @param      UART_UARTn      UART_UARTx      module number(UART0~UART4)
 *  @param      char            ch              UART accepts bytes
 *  @since      v1.0
 *  Sample usage:
                        UART_GetChar(UART3,&ch);   //Wait to receive 1 byte and save it to ch
 */
void UART_GetChar(UART_UARTn UART_UARTx,char *ch)
{
  while(!(UART_S1_REG(UARTN[UART_UARTx]) & UART_S1_RDRF_MASK));//Waiting for data reception
  
  *ch = UART_D_REG(UARTN[UART_UARTx]);//Read the received value from the buffer register
}

/*
 *  @brief      UART send 1 byte
 *  @param      UART_UARTn      UART_UARTx      module number(UART0~UART4)
 *  @param      char            ch              UART send bytes
 *  @since      v1.0
 *  Sample usage:       UART_PutChar(UART3,'A');  //Send byte 'A'
 */
void UART_PutChar(UART_UARTn UART_UARTx,char ch)
{
  while(!(UART_S1_REG(UARTN[UART_UARTx]) & UART_S1_TDRE_MASK));//Waiting to send buffer
  
  UART_D_REG(UARTN[UART_UARTx]) = (uint8)ch;//Send a byte of data
}

/*
 *  @brief      UART sends string (stops sending when encountering NULL)
 *  @param      UART_UARTn      UART_UARTx      module number(UART0~UART4)
 *  @param      str             String address
 *  @since      v1.0
 *  Sample usage:       UART_PutStr(UART3,"1234567");//Actually sent 7 bytes
 */
void UART_PutStr(UART_UARTn UART_UARTx,const uint8 *str)
{
  for(;*str!='\0';str++)
  {
    while(!(UART_S1_REG(UARTN[UART_UARTx]) & UART_S1_TDRE_MASK));//Waiting to send buffer
  
    UART_D_REG(UARTN[UART_UARTx]) = (uint8)(*str);//Send a byte of data
  }
}

/*
 *  @brief      Enable UART to receive interrupts
 *  @param      UART_UARTn      UART_UARTx      module number(UART0~UART4)
 *  @since      v1.0
 *  Sample usage:       UART_Rx_IRQ_En(UART3);         //Open serial port 3 to receive interrupts
 */
void UART_Rx_IRQ_En(UART_UARTn UART_UARTx)
{
  UART_C2_REG(UARTN[UART_UARTx]) |= UART_C2_RIE_MASK;//UART accepts interrupt enable
  if(UART_UARTx == UART_UART4)
  {
    enable_irq(UART4_RX_TX_IRQn);//IRQ interrupts
  }
  else
  {
    enable_irq((IRQn_Type)((UART_UARTx<<1) + UART0_RX_TX_IRQn));//IRQ interrupts
  }
}

/*
 *  @brief      Enable UART to send interrupts
 *  @param      UART_UARTn      UART_UARTx      module number(UART0~UART4)
 *  @since      v1.0
 *  Sample usage:       UART_Tx_IRQ_En(UART3);         //Open serial port 3 to receive interrupts
 */
void UART_Tx_IRQ_En(UART_UARTn UART_UARTx)
{
  UART_C2_REG(UARTN[UART_UARTx]) |= UART_C2_TIE_MASK;//UART Send Interrupt Enable
  if(UART_UARTx == UART_UART4)
  {
    enable_irq(UART4_RX_TX_IRQn);//IRQ interrupts
  }
  else
  {
    enable_irq((IRQn_Type)((UART_UARTx<<1) + UART0_RX_TX_IRQn));//IRQ interrupts
  }
}

/*
 *  @brief      Turn off UART receive interrupt
 *  @param      UART_UARTn      UART_UARTx      module number(UART0~UART4)
 *  @since      v1.0
 *  Sample usage:       UART_Rx_IRQ_Dis(UART3);         //Close serial port 3 and receive interrupt
 */
void UART_Rx_IRQ_Dis(UART_UARTn UART_UARTx)
{
  UART_C2_REG(UARTN[UART_UARTx]) &= ~UART_C2_RIE_MASK;//UART accepts interrupt and closes
  
  if(!(UART_C2_REG(UARTN[UART_UARTx]) & (UART_C2_TIE_MASK | UART_C2_TCIE_MASK)))
  {
    if(UART_UARTx == UART_UART4)
    {
      disable_irq(UART4_RX_TX_IRQn);//Close Interrupt
    }
    else
    {
      disable_irq((IRQn_Type)((UART_UARTx<<1) + UART0_RX_TX_IRQn));//Close Interrupt
    }
  }
}

/*
 *  @brief      Turn off UART sending interrupt
 *  @param      UART_UARTn      UART_UARTx      module number(UART0~UART4)
 *  @since      v1.0
 *  Sample usage:       UART_Tx_IRQ_Dis(UART3);         //Close serial port 3 and send interrupt
 */
void UART_Tx_IRQ_Dis(UART_UARTn UART_UARTx)
{
  UART_C2_REG(UARTN[UART_UARTx]) &= ~UART_C2_TIE_MASK;//UART send interrupt closed
  
  if(!(UART_C2_REG(UARTN[UART_UARTx]) & UART_C2_RIE_MASK))
  {
    if(UART_UARTx == UART_UART4)
    {
      disable_irq(UART4_RX_TX_IRQn);//Close Interrupt
    }
    else
    {
      disable_irq((IRQn_Type)((UART_UARTx<<1) + UART0_RX_TX_IRQn));//Close Interrupt
    }
  }
}