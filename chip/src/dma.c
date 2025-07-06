/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       dma.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "dma.h"
#include "gpio.h"
#include "misc.h"

/*
**DMA channel count initialization count value
*/
uint32 DMA_Count_Array[32]={0};

/*
 *  @brief      DMA GPIO input initialization
 *  @param      void    *SADDR  PTX module address
 *  @param      uint8   BYTEs   8Bit data
 *  @since      v1.0
 *  Sample usage:       DMA_GPIO_Input_Init()
 */
static void DMA_GPIO_Input_Init(void *SADDR,uint8 BYTEs)
{
  uint8 n;
  uint8 tmp;
  uint8 ptxn;
  uint8 ptx0=((((uint32)SADDR) & 0x1C0) / 0x40 ) * 32;//covert to PTA0,PTB0,PTC0,PTD0,PTE0
  
  n = (((uint32)SADDR - ((uint32)(&(((Dtype*)(&(PTA_BASE_PTR->PDIR)))->B[0])))) & 0x3f) * 8;//Minimum pin number
  ptxn = ptx0 + n;
  tmp = ptxn + (BYTEs * 8) - 1;//Maximum pin number
  
  while(ptxn <= tmp)
  {
    GPIO_Init((PTXn)ptxn,GPI,0);
    PORT_Init((PTXn)ptxn,ALT1 | PULLDOWN);
    ptxn++;
  }
  
}

/*
 *  @brief      DMA initialization, requesting the transfer of input port data from the IO port to the memory
 *  @param      DMA_CHn         channel number(DMA_CH0~DMA_CH31)
 *  @param      SADDR           source address( (void * )&PTx_Bn_IN »ò (void * )&PTx_Wn_IN »ò (void * )&PTx_IN  )
 *  @param      DADDR           destination address
 *  @param      PTxn            port trigger
 *  @param      DMA_BYTEn       Number of bytes per DMA transfer
 *  @param      count           Number of bytes transmitted in a main loop
 *  @param      cfg             DMA transfer configuration, select from DMA_cfg
 *  @since      v5.0
 *  @note       DMA PTXn trigger source default rising edge trigger transmission. If modification is required, call port_init after initialization to configure DMA trigger mode After initialization, DMA-IN needs to be called to implement
 *  Sample usage:   uint8 buff[10];
                    DMA_PORTX_To_Buff_Init(DMA_CH0, PTB_B0_IN, buff, PTA7, DMA_BYTE1, 10, DADDR_RECOVER);
                    //DMA initialization, source address: PTB_B0-IN, destination address: buff, PTA7 trigger (default rising edge), 1 byte per transfer, a total of 10 transfers, address restored after transfer completion

                    port_init(PTA7,ALT1 | DMA_FALLING);             //The default trigger source is the rising edge, here it is changed to the falling edge trigger

                    DMA_EN(DMA_CH0);                //DMA needs to be enabled before data can be transferred
 */
void DMA_PORTX_To_Buff_Init(DMA_CHn DMA_CHx,void *SADDR,void *DADDR,PTXn PTXx,DMA_BYTEn DMA_BYTEx,uint32 Count,DMA_CFG CFG)
{
  uint8 BYTEs=0;
  
  //DMA transfer byte count
  if(DMA_BYTEx == DMA_BYTE1)
  {
    BYTEs = 1;
  }
  else if(DMA_BYTEx == DMA_BYTE2)
  {
    BYTEs = 2;
  }
  else if(DMA_BYTEx == DMA_BYTE4)
  {
    BYTEs = 4;
  }
  else
  {
    BYTEs = 16;
  }
  
  ASSERT(((DMA_BYTEx == DMA_BYTE1) && (SADDR >= &(((Dtype*)(&(PTA_BASE_PTR->PDIR)))->B[0]) && SADDR <= &(((Dtype*)(&(PTE_BASE_PTR->PDIR)))->B[3])))
         ||((DMA_BYTEx == DMA_BYTE2) && (SADDR >= &(((Dtype*)(&(PTA_BASE_PTR->PDIR)))->B[0]) && SADDR <= &(((Dtype*)(&(PTE_BASE_PTR->PDIR)))->W[1])) && (((uint32)SADDR & 0x03) != 0x03))
           ||((DMA_BYTEx == DMA_BYTE4) && (SADDR >= &(((Dtype*)(&(PTA_BASE_PTR->PDIR)))->B[0]) && SADDR <= &(((Dtype*)(&(PTE_BASE_PTR->PDIR)))->B[0])) && (((uint32)SADDR & 0x03) == 0x00)));
  
  if(Count > 0x7FFF)//DMA count maximum 32767
  {
    Count = 0x7FFF;
  }
  
  //DMA register configuration
  SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;//DMA module clock enable
  
  SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;//DMA multiplex clock enable
  
  DMA_SADDR(DMA_CHx) = (uint32)SADDR;   //Set DMA transfer source address
  DMA_DADDR(DMA_CHx) = (uint32)DADDR;   //Set DMA transfer destination address
  DMA_SOFF(DMA_CHx) = 0;                //Source address offset 0x00
  DMA_DOFF(DMA_CHx) = BYTEs;            //Add BYTEs to the destination address after transmission is completed
  DMA_ATTR(DMA_CHx) = (0
                       | DMA_ATTR_SMOD(0)               //Source address modulus prohibition
                       | DMA_ATTR_SSIZE(BYTEs)          //Source data bit width
                       | DMA_ATTR_DMOD(0)               //Target address modulus prohibition
                       | DMA_ATTR_DSIZE(BYTEs)          //Target data bit width
                       );
  
  DMA_CITER_ELINKNO(DMA_CHx)  = DMA_CITER_ELINKNO_CITER(Count);//Current number of main loops
  DMA_BITER_ELINKNO(DMA_CHx)  = DMA_BITER_ELINKNO_BITER(Count);//Starting main loop count
  DMA_CR &= ~DMA_CR_EMLM_MASK;//prohibit
  
  DMA_NBYTES_MLNO(DMA_CHx) = DMA_NBYTES_MLNO_NBYTES(DMA_BYTEx);
  
  DMA_SLAST(DMA_CHx) = 0;
  DMA_DLAST_SGA(DMA_CHx) = (uint32)((CFG & DADDR_KEEPON) == 0 ? (-Count) : 0);
  
  DMA_CSR(DMA_CHx) = (0
                      | DMA_CSR_BWC(3)               //Bandwidth control, the eDMA engine stops for 8 cycles per read (0 does not stop; 1 reserves; 2 stops for 4 cycles; 3 stops for 8 cycles)
                      | DMA_CSR_DREQ_MASK            //Stop hardware requests after the main loop ends
                      | DMA_CSR_INTMAJOR_MASK        //Interrupt generated after the main loop ends
                      );
  
  DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,DMA_CHx) = (0
                                               | DMAMUX_CHCFG_ENBL_MASK                         //DMA channel enable
                                               | DMAMUX_CHCFG_SOURCE(PORT_PTX(PTXx)+DMA_PORTA)  //DMA channel trigger source (DMA-PORTA~DMA-PORTE)
                                               );
  
  PORT_Init(PTXx,ALT1 | DMA_RISING);    //PORT port initialization PTXx, DMA rising edge
  DMA_GPIO_Input_Init(SADDR,BYTEs);     //GPIO initialization settings
  
  DMA_DIS(DMA_CHx);             //DMA channel enable
  DMA_IRQ_CLEAN(DMA_CHx);       //DMAIRQ interrupt enable
  
}

/*
 *  @brief      DMA Pulse count initialization
 *  @param      DMA_CHn DMA_CHx DMA channel number(DMA_CH0~DMA_CH31)
 *  @param      PTXn    PTXx    PTX trigger DMA transfer port
 *  @param      uint32  DMA_Count DMA counter value(0~32767)
 *  @param      DMA_CFG  DMA_Cfg DMA trigger mode configuration
 *  @since      v1.0
 *  Sample usage:       DMA_Count_Init(DMA_CH0,PTA19,32767,DMA_RISING)
 */
void DMA_Count_Init(DMA_CHn DMA_CHx,PTXn PTXx,uint32 DMA_Count,PORT_CFG DMA_cfg)
{
  uint8 DMA_Byten=DMA_BYTE1;//8Bit transfer
  uint8 BYTEs=1;
  
  if(DMA_Count > 0x7FFF)//DMA maximum counter 32767
  {
    DMA_Count = 0x7FFF;
  }
  
  DMA_Count_Array[DMA_CHx] = DMA_Count;//DMA channel count initial value
  
  //DMA register configuration
  SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;//DMA module clock enable
  SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;//DMA multiplex clock enable
  
  DMA_SADDR(DMA_CHx) = (uint32)DMA_COUNTSADDR;          //Set DMA transfer source address
  DMA_DADDR(DMA_CHx) = (uint32)DMA_COUNTDADDR;          //Set DMA transfer destination address
  DMA_SOFF(DMA_CHx) = 0;                                //source address offset 0
  DMA_DOFF(DMA_CHx) = 0;                                //destination address offset 0
  DMA_ATTR(DMA_CHx) = (0
                       | DMA_ATTR_SMOD(0)               //source address module address prohibition
                       | DMA_ATTR_SSIZE(DMA_Byten)      //source data bit width,8Bit
                       | DMA_ATTR_DMOD(0)               //target address module address prohibition
                       | DMA_ATTR_DSIZE(DMA_Byten)      //target data bit width,8Bit
                       );
  
  DMA_CITER_ELINKNO(DMA_CHx) = DMA_CITER_ELINKNO_CITER(DMA_Count);//current number of main loops
  DMA_BITER_ELINKNO(DMA_CHx) = DMA_BITER_ELINKNO_BITER(DMA_Count);//starting main loop count
  DMA_CR &= ~DMA_CR_EMLM_MASK;//prohibit transmission
  
  DMA_NBYTES_MLNO(DMA_CHx) = DMA_NBYTES_MLNO_NBYTES(BYTEs);//DMA transfer byte count
  
  DMA_SLAST(DMA_CHx) = -DMA_Count;      //restore source address
  DMA_DLAST_SGA(DMA_CHx) = -DMA_Count;  //restore destination address
  
  DMA_CSR(DMA_CHx) = (0
                      | DMA_CSR_DREQ_MASK       //stop hardware requests after the main loop ends
                      | DMA_CSR_INTMAJOR_MASK   //interrupt generated after the main loop ends
                      );
  
  //DMA trigger source configuration (normal mode)
  DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,DMA_CHx) = (0
                                               | DMAMUX_CHCFG_ENBL_MASK                         //DMA channel enable
                                               | DMAMUX_CHCFG_SOURCE(PORT_PTX(PTXx)+DMA_PORTA)  //DMA channel trigger(DMA_PORTA~DMA_PORTE)
                                               );
  
  GPIO_Init(PTXx,GPI,0);//PTXx set as input
  PORT_Init(PTXx,ALT1 | PULLDOWN | DMA_cfg);//DMA rising edge trigger
  
  DMA_EN(DMA_CHx);      //DMA channel enable
  DMA_IRQ_EN(DMA_CHx);  //DMAIRQ interrupts enable
  
}

/*
 *  @brief      DMA obtain pulse count value
 *  @param      DMA_CHn DMA_CHx DMA channel number(DMA_CH0~DMA_CH31)
 *  @since      v1.0
 *  Sample usage:       DMA_Temp = DMA_Count_Get(DMA_CH0)
 */
uint32 DMA_Count_Get(DMA_CHn DMA_CHx)
{
  uint32 DMA_Temp=0;
  
  DMA_Temp = DMA_Count_Array[DMA_CHx] - DMA_CITER_ELINKNO(DMA_CHx);//read the current DMA pulse count value
  
  return DMA_Temp;
}

/*
 *  @brief      DMA pulse count reset to zero
 *  @param      DMA_CHn DMA_CHx DMA channel number(DMA_CH0~DMA_CH31)
 *  @since      v1.0
 *  Sample usage:       DMA_Count_Reset(DMA_CH0)
 */
void DMA_Count_Reset(DMA_CHn DMA_CHx)
{
  DMA_IRQ_CLEAN(DMA_CHx);//Clear DMA interrupt flag bit
  DMA_CITER_ELINKNO(DMA_CHx) = DMA_Count_Array[DMA_CHx];//DMA count value reset
}