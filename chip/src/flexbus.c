/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       flexbus.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "flexbus.h"
#include "port.h"

/*
 *  @brief      Flexbus initialized to 8080 protocol
 *  @since      v1.0
 */
void FlexBus_8080_Init(void)
{
  //Flexbus itself does not directly support the 8080 protocol, but supports the 6800 protocol
  //We can configure cmd and data to have different addresses, so as to distinguish commands and data through address lines
  SIM_SOPT2 |= SIM_SOPT2_FBSL(3);             //FlexBus security level: Allow instruction and data access
  SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;        //Enable clock
  
  //Configure data pin reuse
  PORT_Init(PTD6 ,ALT5 | HDS);              //PTD6 reuse as fbaddr [0], configured to output high driving capability
  PORT_Init(PTD5 ,ALT5 | HDS);              //PTD5 reuse as fbaddr [1], configured to output high driving capability
  PORT_Init(PTD4 ,ALT5 | HDS);              //PTD4 reuse as fbaddr [2], configured to output high driving capability
  PORT_Init(PTD3 ,ALT5 | HDS);              //PTD3 reuse as fbaddr [3], configured to output high driving capability
  PORT_Init(PTD2 ,ALT5 | HDS);              //PTD2 reuse as fbaddr [4], configured to output high driving capability
  PORT_Init(PTC10,ALT5 | HDS);              //PTC10 reuse as fbaddr [5], configured to output high driving capability
  PORT_Init(PTC9 ,ALT5 | HDS);              //PTC9 reuse as fbaddr [6], configured to output high driving capability
  PORT_Init(PTC8 ,ALT5 | HDS);              //PTC8 reuse as fbaddr [7], configured to output high driving capability
  PORT_Init(PTC7 ,ALT5 | HDS);              //PTC7 reuse as fbaddr [8], configured to output high driving capability
  PORT_Init(PTC6 ,ALT5 | HDS);              //PTC6 reuse as fbaddr [9], configured to output high driving capability
  PORT_Init(PTC5 ,ALT5 | HDS);              //PTC5 reuse as fbaddr [10], configured to output high driving capability
  PORT_Init(PTC4 ,ALT5 | HDS);              //PTC4 reuse as fbaddr [11], configured to output high driving capability
  PORT_Init(PTC2 ,ALT5 | HDS);              //PTC2 reuse as fbaddr [12], configured to output high driving capability
  PORT_Init(PTC1 ,ALT5 | HDS);              //PTC1 reuse as fbaddr [13], configured to output high driving capability
  PORT_Init(PTC0 ,ALT5 | HDS);              //PTC0 reuse as fbaddr [14], configured to output high driving capability
  PORT_Init(PTB18,ALT5 | HDS);              //PTB18 reuse as fbaddr [15], configured to output high driving capability
  
  //Configure control pin reuse
  //8080 bus, also known as Intel bus, requires four control lines: RD write enable, WR read enable, RS data/instruction selection, CS chip selection
  PORT_Init(PTB19,ALT5 | HDS);              //PTB19 is multiplexed as fb_oe_b and configured to output high driving capability. The timing of fb_oe_b conforms to the RD write enable of 8080 bus
  PORT_Init(PTD1 ,ALT5 | HDS);              //PTD1 is multiplexed as fb_cs0-b and configured to output high driving capability. The timing of fb_cs0-b conforms to the CS chip selection of 8080 bus
  PORT_Init(PTC11,ALT5 | HDS);              //PTC11 is multiplexed as fb_r/w and configured to output high driving capability. The timing of fb_r/w conforms to the WR read enable of 8080 bus
  
  //At present, there is still a lack of RS data/instruction selection lines for the 8080 bus
  //Flexbus can be configured as two blocks with discontinuous addresses (different addresses for the top 16 bits), thereby utilizing address lines to comply with the RS data/instruction selection lines of the 8080 bus
  //The address with 16 bits higher, FB_SA, is configured as 0x6000, so the available address lines are fbaddr [16]~fbaddr [28]
  //FB_SAM is configured as 0x0800, which means selecting fb_ded [27] as the RS. (0x0800 0000 == 1<<27 )
  //When RS==1, data is transmitted, and when RS==0, commands are transmitted
  //Therefore, 0x6000 0000 is the command address and 0x6800 0000 is the data port
  PORT_Init(PTC12 ,ALT5 | HDS);              //PTC12 is multiplexed as fbaddr [27] and configured to output high drive capability. fbaddr [27] serves as the RS data/instruction selection line for the 8080 bus

  FB_CSAR(0) = FB_CSAR_BA(FB_BA);            //Base address
  FB_CSMR(0) = (0
                | FB_CSMR_BAM(FB_BAM)       //BAM = 0x0800 , The base address mask is 0x, 800 FFFF, which means that the valid address for chip selection is the base address~(base address+base address mask). 0x0800 0000 corresponds to FBOAD27
                | FB_CSMR_V_MASK            //Using the selection signal FB_CS0
                );
  FB_CSCR(0) = (FB_CSCR_BLS_MASK            //Right aligned
                | FB_CSCR_PS(2)             //16Byte data
                | FB_CSCR_AA_MASK           //Automatic response
                );
}