/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       spi.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "port.h"
#include "port_cfg.h"
#include "spi.h"
#include "system_init.h"

/*
**SPI base pointer
*/
SPI_MemMapPtr SPIN[SPI_SPI_MAX]= 
{
  SPI0_BASE_PTR,//SPI0 base pointer
  SPI1_BASE_PTR,//SPI1 base pointer
  SPI2_BASE_PTR,//SPI2 base pointer
};

/*
 *  @brief      SPI initialization, setting mode
 *  @param      SPI_SPIn        SPI_SPIx        SPI modules (SPI0, SPI1, SPI2)
 *  @param      SPI_PCSn        SPI_PCSx        Pin number for chip selection
 *  @param      SPI_CFG         SPI_cfg         SPI master-slave mode selection
 *  @param      uint32          SPI_Baud        SPI baud rate
 *  @since      v1.0
 *  Sample usage:       uint32 SPI_Real_Baud = SPI_Init(SPI_SPI0,SPIn_PCS0, MASTER,10*1000*1000);              //Initialize SPI, select CS0, host mode, baud rate of 1M, return the true baud rate to the baud variable
 */
uint32 SPI_Init(SPI_SPIn SPI_SPIx,SPI_PCSn SPI_PCSx,SPI_CFG SPI_cfg,uint32 SPI_Baud)
{
  uint8 SPI_Br=0;
  uint8 SPI_Pbr=0;
  uint8 SPI_Pcssck=0;
  uint8 SPI_Cssck=0;
  uint8 SPI_Fit_Pcssck=0;
  uint8 SPI_Fit_Cssck=0;
  uint8 SPI_PreScaler[]={2,3,5,7};
  uint32 SPI_Clk=MK64_Bus_KHz*1000/SPI_Baud;
  uint32 SPI_Scaler[]={2,4,6,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};
  uint32 SPI_Tmp=0;
  uint32 SPI_Fit_Clk=0;
  uint32 SPI_Fit_Br=0;
  uint32 SPI_Fit_Pbr=0;
  uint32 SPI_Min_Diff=~0;
  uint32 SPI_Diff=0;
  
  //SPI pin reuse (select SPI module)
  switch(SPI_SPIx)
  {
    case SPI_SPI0:
                  SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;//SPI0 clock enable
                  PORT_Init(SPI0_SCK_PIN,ALT2);//Initialize SPI_SCK
                  PORT_Init(SPI0_SOUT_PIN,ALT2);//Initialize SPI_SOUT
                  PORT_Init(SPI0_SIN_PIN,ALT2);//Initialize SPI_SIN
                  if(SPI_PCSx == SPI_PCS0)
                  {
                    PORT_Init(SPI0_PCS0_PIN,ALT2);//Initialize CS0
                  }
                  else if(SPI_PCSx == SPI_PCS1)
                  {
                    PORT_Init(SPI0_PCS1_PIN,ALT2);//Initialize CS1
                  }
                  else if(SPI_PCSx == SPI_PCS2)
                  {
                    PORT_Init(SPI0_PCS2_PIN,ALT2);//Initialize CS2
                  }
                  else if(SPI_PCSx == SPI_PCS3)
                  {
                    PORT_Init(SPI0_PCS3_PIN,ALT2);//Initialize CS3
                  }
                  else if(SPI_PCSx == SPI_PCS4)
                  {
                    PORT_Init(SPI0_PCS4_PIN,ALT2);//Initialize CS4
                  }
                  else if(SPI_PCSx == SPI_PCS5)
                  {
                    PORT_Init(SPI0_PCS5_PIN,ALT3);//Initialize CS2
                  }
                  else
                  {
                    ASSERT(0);
                  }
                  break;
    case SPI_SPI1:
                  SIM_SCGC6 |= SIM_SCGC6_SPI1_MASK;//SPI1 clock enable
                  PORT_Init(SPI1_SCK_PIN,ALT2);//Initialize SPI_SCK
                  PORT_Init(SPI1_SOUT_PIN,ALT2);//Initialize SPI_SOUT
                  PORT_Init(SPI1_SIN_PIN,ALT2);//Initialize SPI_SIN
                  if(SPI_PCSx == SPI_PCS0)
                  {
                    PORT_Init(SPI1_PCS0_PIN,ALT2);//Initialize CS0
                  }
                  else if(SPI_PCSx == SPI_PCS1)
                  {
                    PORT_Init(SPI1_PCS1_PIN,ALT2);//Initialize CS1
                  }
                  else if(SPI_PCSx == SPI_PCS2)
                  {
                    PORT_Init(SPI1_PCS2_PIN,ALT2);//Initialize CS2
                  }
                  else if(SPI_PCSx == SPI_PCS3)
                  {
                    PORT_Init(SPI1_PCS3_PIN,ALT2);//Initialize CS3
                  }
                  else
                  {
                    ASSERT(0);
                  }
                  break;
    case SPI_SPI2:
                  SIM_SCGC3 |= SIM_SCGC3_SPI2_MASK;//SPI2 clock enable
                  PORT_Init(SPI2_SCK_PIN,ALT2);//Initialize SPI_SCK
                  PORT_Init(SPI2_SOUT_PIN,ALT2);//Initialize SPI_SOUT
                  PORT_Init(SPI2_SIN_PIN,ALT2);//Initialize SPI_SIN
                  if(SPI_PCSx == SPI_PCS0)
                  {
                    PORT_Init(SPI2_PCS0_PIN,ALT2);//Initialize CS0
                  }
                  else if(SPI_PCSx == SPI_PCS1)
                  {
                    PORT_Init(SPI2_PCS1_PIN,ALT2);//Initialize CS1
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
  
  //SPI register configuration
  SPI_MCR_REG(SPIN[SPI_SPIx]) = (0
                                 | SPI_MCR_CLR_TXF_MASK //Clear Tx FIFO counter
                                 | SPI_MCR_CLR_RXF_MASK //Clear Rx FIFO counter
                                 | SPI_MCR_HALT_MASK    //Stop SPI transmission
                                 );
  
  SPI_SR_REG(SPIN[SPI_SPIx]) = (0
                                | SPI_SR_EOQF_MASK      //The sending queue is empty, sending completed
                                | SPI_SR_TFUF_MASK      //When the transmission FIFO overflow flag is set, SPI is in slave mode, Tx FIFO is empty, and external SPI master mode starts transmission, the flag will be set to 1 and write 1 to clear 0
                                | SPI_SR_TFFF_MASK      //Transfer FIFO full flag bit Write 1 or the DMA controller will clear 0.0 when it detects that the transmit FIFO is full, indicating that the Tx FIFO is full
                                | SPI_SR_RFOF_MASK      //Receive FIFO overflow flag bit
                                | SPI_SR_RFDF_MASK      //Receive FIFO loss flag bit, write 1 or DMA controller will clear 0.0 if transmission FIFO is empty, indicating Rx FIFO is empty
                                );
  
  if(SPI_cfg == MASTER)//Host mode
  {
    SPI_MCR_REG(SPIN[SPI_SPIx]) = (0
                                   | SPI_MCR_MSTR_MASK          //Enable host mode
                                   | SPI_MCR_PCSIS(SPI_PCSx)    //Chip selection signal high level, disable SPI transmission
                                   );
    
    //Set the transmission baud rate and division factor
    for(SPI_Br=0;SPI_Br<16;SPI_Br++)
    {
      for(SPI_Pbr=0;SPI_Pbr<4;SPI_Pbr++)
      {
        SPI_Tmp = SPI_Scaler[SPI_Br] * SPI_PreScaler[SPI_Pbr];
        SPI_Diff = abs(SPI_Tmp - SPI_Clk);
        if(SPI_Min_Diff > SPI_Diff)
        {
          SPI_Min_Diff = SPI_Diff;
          SPI_Fit_Br = SPI_Br;
          SPI_Fit_Pbr = SPI_Pbr;
          
          if(SPI_Min_Diff == 0)
          {
            goto SPI_CLK_EXIT;
          }
        }
      }
    }
    
  SPI_CLK_EXIT:
    SPI_Fit_Clk = MK64_Bus_KHz * 1000 / (SPI_Scaler[SPI_Fit_Br] * SPI_PreScaler[SPI_Fit_Pbr]);
    SPI_Clk = MK64_Bus_KHz * 1000 / SPI_Fit_Clk / 2;
    SPI_Min_Diff = ~0;
    SPI_Fit_Cssck = 15;
    SPI_Fit_Pcssck = 3;
    
    for(SPI_Cssck=0;SPI_Cssck<15;SPI_Cssck++)
    {
      SPI_Tmp = 1 << (SPI_Cssck + 1);
      SPI_Pcssck = (SPI_Clk / SPI_Tmp - 1) / 2;
      if(SPI_Pcssck > 3)
      {
        continue;
      }
      SPI_Tmp = SPI_Tmp * (2 * SPI_Pcssck + 1);
      SPI_Diff = abs(SPI_Tmp - SPI_Clk);
      if(SPI_Min_Diff > SPI_Diff)
      {
        SPI_Min_Diff = SPI_Diff;
        SPI_Fit_Cssck = SPI_Cssck;
        SPI_Fit_Pcssck = SPI_Pcssck;
        
        if(SPI_Min_Diff == 0)
        {
          goto SPI_CSSCK_EXIT;
        }
      } 
    }
    
  SPI_CSSCK_EXIT:
    SPI_CTAR_REG(SPIN[SPI_SPIx],0) = (0
                                      | SPI_CTAR_PBR(SPI_Fit_Pbr)       //Baud rate pre division factor
                                      | SPI_CTAR_BR(SPI_Fit_Br)         //Baud rate division factor
                                      | SPI_CTAR_PDT(0)                 //Transmission delay
                                      | SPI_CTAR_FMSZ(7)                //7-bit transmission
                                      | SPI_CTAR_PCSSCK(SPI_Fit_Pcssck) //Pre division delay of film selection signal
                                      | SPI_CTAR_CSSCK(SPI_Fit_Cssck)   //Selection signal frequency division delay
                                      );
    
  }
  else//Slave mode
  {
    SPI_CTAR_SLAVE_REG(SPIN[SPI_SPIx],0) = (0
                                            | SPI_CTAR_SLAVE_FMSZ(7)
                                            | SPI_CTAR_SLAVE_CPOL_MASK
                                            | SPI_CTAR_SLAVE_CPHA_MASK
                                            );
  }
  
  SPI_MCR_REG(SPIN[SPI_SPIx]) &= ~SPI_MCR_HALT_MASK;
  
  return SPI_Fit_Clk;
}

/*
 *  @brief      SPI Transmission Common SPI Select Other Chip selection
 *  @param      SPI_SPIn        SPI_SPIx        SPI module��SPI0、SPI1、SPI2��
 *  @param      SPI_PCSn        SPI_PCSx        Pin number for chip selection
 *  @since      v1.0
 *  Sample usage:           SPI_Extra_PCS_Init(SPI_SPI2,SPI_PCS1);//Host mode...
 */
void SPI_Extra_PCS_Init(SPI_SPIn SPI_SPIx,SPI_PCSn SPI_PCSx)
{
  //SPI pin reuse (select SPI module)
  switch(SPI_SPIx)
  {
    case SPI_SPI0:
                  SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;//SPI0 clock enable
                  PORT_Init(SPI0_SCK_PIN,ALT2);//Initialize SPI_SCK
                  PORT_Init(SPI0_SOUT_PIN,ALT2);//Initialize SPI_SOUT
                  PORT_Init(SPI0_SIN_PIN,ALT2);//Initialize SPI_SIN
                  if(SPI_PCSx == SPI_PCS0)
                  {
                    PORT_Init(SPI0_PCS0_PIN,ALT2);//Initialize CS0
                  }
                  else if(SPI_PCSx == SPI_PCS1)
                  {
                    PORT_Init(SPI0_PCS1_PIN,ALT2);//Initialize CS1
                  }
                  else if(SPI_PCSx == SPI_PCS2)
                  {
                    PORT_Init(SPI0_PCS2_PIN,ALT2);//Initialize CS2
                  }
                  else if(SPI_PCSx == SPI_PCS3)
                  {
                    PORT_Init(SPI0_PCS3_PIN,ALT2);//Initialize CS3
                  }
                  else if(SPI_PCSx == SPI_PCS4)
                  {
                    PORT_Init(SPI0_PCS4_PIN,ALT2);//Initialize CS4
                  }
                  else if(SPI_PCSx == SPI_PCS5)
                  {
                    PORT_Init(SPI0_PCS5_PIN,ALT3);//Initialize CS5
                  }
                  else
                  {
                    ASSERT(0);
                  }
                  break;
    case SPI_SPI1:
                  SIM_SCGC6 |= SIM_SCGC6_SPI1_MASK;//SPI1 clock enable
                  PORT_Init(SPI1_SCK_PIN,ALT2);//Initialize SPI_SCK
                  PORT_Init(SPI1_SOUT_PIN,ALT2);//Initialize SPI_SOUT
                  PORT_Init(SPI1_SIN_PIN,ALT2);//Initialize SPI_SIN
                  if(SPI_PCSx == SPI_PCS0)
                  {
                    PORT_Init(SPI1_PCS0_PIN,ALT2);//Initialize CS0
                  }
                  else if(SPI_PCSx == SPI_PCS1)
                  {
                    PORT_Init(SPI1_PCS1_PIN,ALT2);//Initialize CS1
                  }
                  else if(SPI_PCSx == SPI_PCS2)
                  {
                    PORT_Init(SPI1_PCS2_PIN,ALT2);//Initialize CS2
                  }
                  else if(SPI_PCSx == SPI_PCS3)
                  {
                    PORT_Init(SPI1_PCS3_PIN,ALT2);//Initialize CS3
                  }
                  else
                  {
                    ASSERT(0);
                  }
                  break;
    case SPI_SPI2:
                  SIM_SCGC3 |= SIM_SCGC3_SPI2_MASK;//SPI2 clock enable
                  PORT_Init(SPI2_SCK_PIN,ALT2);//Initialize SPI_SCK
                  PORT_Init(SPI2_SOUT_PIN,ALT2);//Initialize SPI_SOUT
                  PORT_Init(SPI2_SIN_PIN,ALT2);//Initialize SPI_SIN
                  if(SPI_PCSx == SPI_PCS0)
                  {
                    PORT_Init(SPI2_PCS0_PIN,ALT2);//Initialize CS0
                  }
                  else if(SPI_PCSx == SPI_PCS1)
                  {
                    PORT_Init(SPI2_PCS1_PIN,ALT2);//Initialize CS1
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
  
  SPI_MCR_REG(SPIN[SPI_SPIx]) |= SPI_MCR_PCSIS(SPI_PCSx);//Chip selection signal high level, disable SPI transmission
}

/*
 *  @brief      SPI send receive function
 *  @param      SPI_SPIn        SPI_SPIx        SPI module (SPI0、SPI1、SPI2)
 *  @param      SPI_PCSn        SPI_PCSx        Pin number for chip selection
 *  @param      uint8*          SPI_MO_Data     Send data buffer address (NULL if not needed to receive)
 *  @param      uint8*          SPI_MI_Data     The storage address of the data received when sending data (NULL if not needed)
 *  @param      uint32          SPI_Len         Sending data length
 *  @since      v1.0
 *  Sample usage:           SPI_MOSI(SPI0,SPIn_PCS0,buff,buff,2);    //Send the content of the buff and receive it in the buff, with a length of 2 bytes
 */
void SPI_MOSI(SPI_SPIn SPI_SPIx,SPI_PCSn SPI_PCSx,uint8 *SPI_MO_Data,uint8 *SPI_MI_Data,uint32 SPI_Len)
{
  uint32 i=0;
  
  SPI_MCR_REG(SPIN[SPI_SPIx]) |= SPI_MCR_PCSIS(SPI_PCSx);//Select the film and signal
  
  //Clear the transmission flag bit
  do
  {
    SPI_SR_REG(SPIN[SPI_SPIx]) |= (0
                                   | SPI_SR_EOQF_MASK      //The sending queue is empty, sending completed
                                   | SPI_SR_TFUF_MASK      //When the transmission FIFO overflow flag is set, SPI is in slave mode, Tx FIFO is empty, and external SPI master mode starts transmission, the flag will be set to 1 and write 1 to clear 0
                                   | SPI_SR_TFFF_MASK      //Transfer FIFO full flag bit Write 1 or the DMA controller will clear 0.0 when it detects that the transmit FIFO is full, indicating that the Tx FIFO is full
                                   | SPI_SR_RFOF_MASK      //Receive FIFO overflow flag bit
                                   | SPI_SR_RFDF_MASK      //Receive FIFO loss flag bit, write 1 or DMA controller will clear 0.0 if transmission FIFO is empty, indicating Rx FIFO is empty
                                   );
    
    SPI_MCR_REG(SPIN[SPI_SPIx]) |= (0
                                    | SPI_MCR_CLR_TXF_MASK //Clear Tx FIFO counter
                                    | SPI_MCR_CLR_RXF_MASK //Clear Rx FIFO counter
                                    );
  }while(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK);
  
  if(SPI_MO_Data != NULL)//Sending data is not null
  {
    if(SPI_MI_Data != NULL)//Accept data that is not null
    {
      //Send the first N-1 data
      for(i=0;i<(SPI_Len-1);i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(SPI_MO_Data[i])
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_MI_Data[i] = (uint8)SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
      
      //Send the last data
      SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                       | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                       | SPI_PUSHR_EOQ_MASK       //Transfer the final data of SPI
                                       | SPI_PUSHR_PCS(SPI_PCSx)
                                       | SPI_PUSHR_TXDATA(SPI_MO_Data[i])
                                       );
      SPI_EOQF_WAIT(SPI_SPIx);
      
      while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
      
      SPI_MI_Data[i] = (uint8)SPI_POPR_REG(SPIN[SPI_SPIx]);
        
      SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
    }
    else//Accept data as NULL
    {
      //Send the first N-1 data
      for(i=0;i<(SPI_Len-1);i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(SPI_MO_Data[i])
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
      
      //Send the last data
      SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                       | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                       | SPI_PUSHR_EOQ_MASK       //Transfer the final data of SPI
                                       | SPI_PUSHR_PCS(SPI_PCSx)
                                       | SPI_PUSHR_TXDATA(SPI_MO_Data[i])
                                       );
      SPI_EOQF_WAIT(SPI_SPIx);
      
      while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
      
      SPI_POPR_REG(SPIN[SPI_SPIx]);
        
      SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
    }
  }
  else//Sending data as NULL
  {
    if(SPI_MI_Data != NULL)//Accept data that is not null
    {
      //Send the first N-1 data
      for(i=0;i<(SPI_Len-1);i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(NULL)
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_MI_Data[i] = (uint8)SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
      
      //Send the last data
      SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                       | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                       | SPI_PUSHR_EOQ_MASK       //Transfer the final data of SPI
                                       | SPI_PUSHR_PCS(SPI_PCSx)
                                       | SPI_PUSHR_TXDATA(NULL)
                                       );
      SPI_EOQF_WAIT(SPI_SPIx);
      
      while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
      
      SPI_MI_Data[i] = (uint8)SPI_POPR_REG(SPIN[SPI_SPIx]);
        
      SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
    }
    else//Accept data as NULL
    {
      //Send the first N-1 data
      for(i=0;i<(SPI_Len-1);i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(NULL)
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
      
      //Send the last data
      SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                       | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                       | SPI_PUSHR_EOQ_MASK       //Transfer the final data of SPI
                                       | SPI_PUSHR_PCS(SPI_PCSx)
                                       | SPI_PUSHR_TXDATA(NULL)
                                       );
      SPI_EOQF_WAIT(SPI_SPIx);
      
      while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
      
      SPI_POPR_REG(SPIN[SPI_SPIx]);
        
      SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
    }
  }
  
  SPI_MCR_REG(SPIN[SPI_SPIx]) &= ~SPI_MCR_PCSIS(SPI_PCSx);//Release selection signal
}

/*
 *  @brief      SPI send receive function (with send command CMD)
 *  @param      SPI_SPIn        SPI_SPIx        SPI module (SPI0、SPI1、SPI2)
 *  @param      SPI_PCSn        SPI_PCSx        Pin number for chip selection
 *  @param      uint8*          SPI_MO_CMD      Send command buffer address (NULL if not needed to receive)
 *  @param      uint8*          SPI_MI_CMD      The storage address of the data received when sending the command (NULL if not needed)
 *  @param      uint8*          SPI_MO_Data     Send data buffer address (NULL if not needed to receive)
 *  @param      uint8*          SPI_MI_Data     The storage address of the data received when sending data (NULL if not needed)
 *  @param      uint32          SPI_CMD_Len     Send command length
 *  @param      uint32          SPI_Len         Send data length
 *  @since      v1.0
 *  Sample usage:           SPI_MOSI_CMD(SPI0,SPIn_PCS0,cmd,NULL,buff,buff,1,2);    //Send the content of cmd/buff, do not receive the data sent by cmd, receive the data sent by buff into buff, with lengths of 1 and 2 bytes respectively
 */
void SPI_MOSI_CMD(SPI_SPIn SPI_SPIx,SPI_PCSn SPI_PCSx,uint8 *SPI_MO_CMD,uint8 *SPI_MI_CMD,uint8 *SPI_MO_Data,uint8 *SPI_MI_Data,uint32 SPI_CMD_Len,uint32 SPI_Len)
{
  uint32 i=0;
  
  SPI_MCR_REG(SPIN[SPI_SPIx]) |= SPI_MCR_PCSIS(SPI_PCSx);//Select the film and signal
  
  do
  {
    SPI_SR_REG(SPIN[SPI_SPIx]) |= (0
                                   | SPI_SR_EOQF_MASK      //The sending queue is empty, sending completed
                                   | SPI_SR_TFUF_MASK      //When the transmission FIFO overflow flag is set, SPI is in slave mode, Tx FIFO is empty, and external SPI master mode starts transmission, the flag will be set to 1 and write 1 to clear 0
                                   | SPI_SR_TFFF_MASK      //Transfer FIFO full flag bit Write 1 or the DMA controller will clear 0.0 when it detects that the transmit FIFO is full, indicating that the Tx FIFO is full
                                   | SPI_SR_RFOF_MASK      //Receive FIFO overflow flag bit
                                   | SPI_SR_RFDF_MASK      //Receive FIFO loss flag bit, write 1 or DMA controller will clear 0.0 if transmission FIFO is empty, indicating Rx FIFO is empty
                                   );
    SPI_MCR_REG(SPIN[SPI_SPIx]) |= (0
                                    | SPI_MCR_CLR_TXF_MASK //Clear Tx FIFO counter
                                    | SPI_MCR_CLR_RXF_MASK //Clear Rx FIFO counter
                                    );
  }while(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK);
  
  if(SPI_MO_CMD != NULL)//Send command not null
  {
    if(SPI_MI_CMD != NULL)//Receive command not nullL
    {
      for(i=0;i<SPI_CMD_Len;i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(SPI_MO_CMD[i])
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_MI_CMD[i] = (uint8)SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
    }
    else//Receive command not null
    {
      for(i=0;i<SPI_CMD_Len;i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(SPI_MO_CMD[i])
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
    }
  }
  else//Send command not null
  {
    if(SPI_MI_CMD != NULL)//Receive command not null
    {
      for(i=0;i<SPI_CMD_Len;i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(NULL)
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_MI_CMD[i] = (uint8)SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
    }
    else//Receive command not null
    {
      for(i=0;i<SPI_CMD_Len;i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(NULL)
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
    }
  }
  
  if(SPI_MO_Data != NULL)//Sending data is not null
  {
    if(SPI_MI_Data != NULL)//Accept data that is not null
    {
      //Send the first N-1 data
      for(i=0;i<(SPI_Len-1);i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(SPI_MO_Data[i])
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_MI_Data[i] = (uint8)SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
      
      //Send the last data
      SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                       | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                       | SPI_PUSHR_EOQ_MASK       //Transfer the final data of SPI
                                       | SPI_PUSHR_PCS(SPI_PCSx)
                                       | SPI_PUSHR_TXDATA(SPI_MO_Data[i])
                                       );
      SPI_EOQF_WAIT(SPI_SPIx);
      
      while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
      
      SPI_MI_Data[i] = (uint8)SPI_POPR_REG(SPIN[SPI_SPIx]);
        
      SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
    }
    else//Accept data as NULL
    {
      //Send the first N-1 data
      for(i=0;i<(SPI_Len-1);i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(SPI_MO_Data[i])
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
      
      //Send the last data
      SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                       | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                       | SPI_PUSHR_EOQ_MASK       //Transfer the final data of SPI
                                       | SPI_PUSHR_PCS(SPI_PCSx)
                                       | SPI_PUSHR_TXDATA(SPI_MO_Data[i])
                                       );
      SPI_EOQF_WAIT(SPI_SPIx);
      
      while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
      
      SPI_POPR_REG(SPIN[SPI_SPIx]);
        
      SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
    }
  }
  else//Sending data as NULL
  {
    if(SPI_MI_Data != NULL)//Accept data that is not null
    {
      //Send the first N-1 data
      for(i=0;i<(SPI_Len-1);i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(NULL)
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_MI_Data[i] = (uint8)SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
      
      //Send the last data
      SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                       | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                       | SPI_PUSHR_EOQ_MASK       //Transfer the final data of SPI
                                       | SPI_PUSHR_PCS(SPI_PCSx)
                                       | SPI_PUSHR_TXDATA(NULL)
                                       );
      SPI_EOQF_WAIT(SPI_SPIx);
      
      while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
      
      SPI_MI_Data[i] = (uint8)SPI_POPR_REG(SPIN[SPI_SPIx]);
        
      SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
    }
    else//Accept data as NULL
    {
      //Send the first N-1 data
      for(i=0;i<(SPI_Len-1);i++)
      {
        SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                         | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                         | SPI_PUSHR_CONT_MASK      //Maintain PCSn signal during transmission
                                         | SPI_PUSHR_PCS(SPI_PCSx)
                                         | SPI_PUSHR_TXDATA(NULL)
                                         );
        while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
        
        SPI_POPR_REG(SPIN[SPI_SPIx]);
        
        SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
      }
      
      //Send the last data
      SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                       | SPI_PUSHR_CTAS(0)        //Select CTAR0 register
                                       | SPI_PUSHR_EOQ_MASK       //Transfer the final data of SPI
                                       | SPI_PUSHR_PCS(SPI_PCSx)
                                       | SPI_PUSHR_TXDATA(NULL)
                                       );
      SPI_EOQF_WAIT(SPI_SPIx);
      
      while(!(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK));
      
      SPI_POPR_REG(SPIN[SPI_SPIx]);
        
      SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_RFDF_MASK;
    }
  }
  
  SPI_MCR_REG(SPIN[SPI_SPIx]) &= ~SPI_MCR_PCSIS(SPI_PCSx);//Release selection signal
}

/*
 *  @brief      SPI sends 8Bit data
 *  @param      SPI_SPIn        SPI_SPIx        SPI module (SPI0、SPI1、SPI2)
 *  @param      SPI_PCSn        SPI_PCSx        Pin number for chip selection
 *  @param      uint8           Data            8Bit data
 *  @since      v1.0
 *  Sample usage:           SPI_Send(SPI_SPI2,SPI_PCS1,0x00);
 */
void SPI_Send(SPI_SPIn SPI_SPIx,SPI_PCSn SPI_PCSx,uint8 Data)
{
  SPI_MCR_REG(SPIN[SPI_SPIx]) |= SPI_MCR_PCSIS(SPI_PCSx);//Select the film and signal
  
  do
  {
    SPI_SR_REG(SPIN[SPI_SPIx]) |= (0
                                   | SPI_SR_EOQF_MASK      //The sending queue is empty, sending completed
                                   | SPI_SR_TFUF_MASK      //When the transmission FIFO overflow flag is set, SPI is in slave mode, Tx FIFO is empty, and external SPI master mode starts transmission, the flag will be set to 1 and write 1 to clear 0
                                   | SPI_SR_TFFF_MASK      //Transfer FIFO full flag bit Write 1 or the DMA controller will clear 0.0 when it detects that the transmit FIFO is full, indicating that the Tx FIFO is full
                                   | SPI_SR_RFOF_MASK      //Receive FIFO overflow flag bit
                                   | SPI_SR_RFDF_MASK      //Receive FIFO loss flag bit, write 1 or DMA controller will clear 0.0 if transmission FIFO is empty, indicating Rx FIFO is empty
                                   );
    SPI_MCR_REG(SPIN[SPI_SPIx]) |= (0
                                    | SPI_MCR_CLR_TXF_MASK //Clear Tx FIFO counter
                                    | SPI_MCR_CLR_RXF_MASK //Clear Rx FIFO counter
                                    );
  }while(SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_RFDF_MASK);
  
  SPI_PUSHR_REG(SPIN[SPI_SPIx]) = (0
                                   | SPI_PUSHR_CTAS(0)          //Select CTAR0 register
                                   | SPI_PUSHR_EOQ_MASK         //Transfer the final data of SPI
                                   | SPI_PUSHR_PCS(SPI_PCSx)    //Enable chip selection CS signal, SPI enabled
                                   | SPI_PUSHR_TXDATA(Data)     //Send 8Bit data
                                   );
  
  while((SPI_SR_REG(SPIN[SPI_SPIx]) & SPI_SR_TCF_MASK) != SPI_SR_TCF_MASK);//Waiting for SPI transmission to complete
  
  SPI_SR_REG(SPIN[SPI_SPIx]) |= SPI_SR_TCF_MASK;//Clear TCF transmission completion flag
  
  SPI_MCR_REG(SPIN[SPI_SPIx]) &= ~SPI_MCR_PCSIS(SPI_PCSx);//Release selection signal
}