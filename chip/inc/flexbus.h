/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       flexbus.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __FLEXBUS_H__
#define __FLEXBUS_H__

#include "common.h"

//The following two parameters are both related to the flexbus_8080uinit function. After modification, it may be necessary to modify the content of the flexbus_8080uinit function
#define FB_BA       0x6000      //Base address=FB_SA * 0x10000, used to distinguish different chip selection signals
                                //According to the system memory map, the range of External Memory is 0x6000_0000~0x9FFF_SFFF
                                //The value range of FB-BA is 0x6000~0x9FFF

#define FB_BAM      0x0800      //Base Address Mask=(FB-BA+FB-BAM) * 0x10000, used to distinguish the addresses of two discontinuous blocks controlled by the same CS

#define FB_8080_CMD     (*(volatile uint16 *)(FB_BA * 0x10000))
#define FB_8080_DATA    (*(volatile uint16 *)((FB_BA + FB_BAM )* 0x10000))

/*
**function declaration
*/
extern void FlexBus_8080_Init(void);


#endif