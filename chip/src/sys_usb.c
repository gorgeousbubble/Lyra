/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       usb.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "misc.h"
#include "sys_usb.h"
#include "usb.h"

/*
 *  @brief      USB module initialization
 *  @since      v1.0
 */
void USB_Init(void)
{
  //software settings 
  Setup_Pkt = (tUSB_Setup *)BufferPointer[bEP0OUT_ODD];
  gu8USB_State = uPOWER;//Mark as powered on state
  
  //MPU configuration
  MPU_CESR = 0;
  
  //SIM configuration
  #ifdef USB_CLOCK_CLKIN
  FLAG_SET(SIM_SCGC5_PORTE_SHIFT, SIM_SCGC5);
  PORTE_PCR26 = (0 | PORT_PCR_MUX(7));//Enable PTE26 as CLK input
  #endif
  
  //CLK configuration
  #ifdef USB_CLOCK_PLL
  SIM_SOPT2 |= (0 | SIM_SOPT2_PLLFLLSEL(1));//Select PLL reference clock
  #endif
  
  #ifndef USB_CLOCK_CLKIN
  SIM_SOPT2 |= (0 | SIM_SOPT2_USBSRC_MASK);//USB Select MCGPLL Reference Clock Input
  #endif
  
  SIM_CLKDIV2 = ((SIM_CLKDIV2 & ~(SIM_CLKDIV2_USBDIV_MASK | SIM_CLKDIV2_USBFRAC_MASK))//Clear SIM_CKDIV2 register
                  | SIM_CLKDIV2_USBDIV(USB_CLK_DIV)                    //USB Frequency division factor
                  | (USB_CLK_FRAC << SIM_CLKDIV2_USBFRAC_SHIFT)        //USB Frequency doubling factor
                   //USB clk = PLL ¡Á [ (USBFRAC+1) / (USBDIV+1) ](80MHz)
                );
  
  SIM_SCGC4 |= SIM_SCGC4_USBOTG_MASK;//USB module clock enable
 
  //Set interrupt vector to enable USB OTG interrupt vector in NVIC
  Set_Vector_Handler(USB0_VECTORn,USB_ISR);
  enable_irq(USB0_IRQn);
  
  //USB module configuration
  USB0_USBTRC0 |= USB_USBTRC0_USBRESET_MASK;      //reset USB module
  while(FLAG_CHK(USB_USBTRC0_USBRESET_SHIFT, USB0_USBTRC0));
  
  //USB set BDT base pointer register
  USB0_BDTPAGE1 = (uint8)((uint32)tBDTtable >> 8); //Configure the current buffer descriptor table BDT
  USB0_BDTPAGE2 = (uint8)((uint32)tBDTtable >> 16);
  USB0_BDTPAGE3 = (uint8)((uint32)tBDTtable >> 24);
  
  //Clear USB reset mark
  FLAG_SET(USB_ISTAT_USBRST_MASK, USB0_ISTAT);
  
  //Enable USB reset interrupt
  FLAG_SET(USB_INTEN_USBRSTEN_SHIFT, USB0_INTEN);
  
  //Enable USB wake-up pull up
  USB0_USBCTRL = 0x40;
  USB0_USBTRC0 |= 0x40;
  USB0_CTL |= 0x01;
  FLAG_SET(USB_CONTROL_DPPULLUPNONOTG_SHIFT, USB0_CONTROL);
  
}

/*
 *  @brief      USB virtual serial port initialization
 *  @since      v1.0
 */
void USB_COM_Init(void)
{
  USB_Init();//USB initialization
  CDC_Init();//CDC initialization
}

/*
 *  @brief      USB waiting for enumeration
 *  @since      v1.0
 */
void USB_Enum_Wait(void)
{
  while(gu8USB_State != uENUMERATED);//Waiting for USB devices to be enumerated
}

/*
 *  @brief      USB virtual serial port received
 *  @param      uint8_t         *Rx_Buf         Data receiving buffer
 *  @return     Received length
 *  @since      v1.0
 */
uint8 USB_COM_Rx(uint8_t *Rx_Buf)
{
  uint8 Len = 0;
  uint8 Temp = 0;
  uint8 *pu8EPBuffer;
  
  //If data is received
  if(FLAG_CHK(EP_OUT, gu8USB_Flags))
  {
    Len = USB_EP_OUT_SizeCheck(EP_OUT);
    Temp = Len;
    pu8EPBuffer = gu8EP3_OUT_ODD_Buffer;
    
    while(Len--)
    {
      *Rx_Buf++ = *pu8EPBuffer++;
    }
    
    usbEP_Reset(EP_OUT);
    usbSIE_CONTROL(EP_OUT);
    FLAG_CLR(EP_OUT, gu8USB_Flags);
    
  }
  
  return Temp;
}

/*
 *  @brief      USB Virtual serial port sending
 *  @param      uint8   *Tx_Buf         Data transmission buffer
 *  @param      uint8   Len             The length of the data to be sent
 *  @since      v1.0
 */
void USB_COM_Tx(uint8 *Tx_Buf,uint8 Len)
{
  EP_IN_Transfer(EP_IN, Tx_Buf, Len);
}
