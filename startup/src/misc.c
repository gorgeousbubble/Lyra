/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       misc.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "common.h"
#include "misc.h"

/*
 *  @brief      Set interrupt vector tables
 *  @param      vtor
 *  @since      v1.0
 *  Sample usage:       write_vtor ((uint32)__VECTOR_RAM);
 */
void write_vtor (int vtor)
{
    ASSERT(vtor % 0x200 == 0);   //Vector Table base offset field. This value must be a multiple of 0x200.

    /* Write the VTOR with the new value */
    SCB->VTOR = vtor;
}

/*
 *  @brief      Set interrupt vector isr functions
 *  @since      v1.0
 *  Sample usage:       Set_Vector_Handler(UART3_RX_TX_VECTORn , uart3_handler);
 */
void Set_Vector_Handler(VECTORn_t vector,void pfunc_handler(void))
{
    extern uint32 __VECTOR_RAM[];

    ASSERT(SCB->VTOR == (uint32)__VECTOR_RAM);

    __VECTOR_RAM[vector] = (uint32)pfunc_handler;
}