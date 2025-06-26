/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       misc.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MISC_H__
#define __MISC_H__

#include "common.h"

/*
** interrupts
*/
#define enable_irq(irq)                 NVIC_EnableIRQ(irq)
#define disable_irq(irq)                NVIC_DisableIRQ(irq)
#define set_irq_priority(irq,pri0)      NVIC_SetPriority(irq,pri0)

#define EnableInterrupts                __enable_irq()
#define DisableInterrupts               __disable_irq()

/*
**variate declaration
*/

/*
**function declaration
*/
extern void write_vtor (int vtor);
extern void Set_Vector_Handler(VECTORn_t vector,void pfunc_handler(void));


#endif