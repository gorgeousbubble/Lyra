/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       conf.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __CONF_H__
#define __CONF_H__

#include "common.h"

/*
**OSC crystal oscillator(50MHz)
*/
#define EXTAL_IN_MHz            50

/*
**PLL phase locked loop frequency
*/
#define MK64_CORE_PLL_CLK       PLL100          //PLL core frequency (PLL150-PLL180 is stable, suggest use PLL150)

#define MK64_CORE_CLK           100             //core clock frequency
#define MK64_BUS_CLK             50             //bus clock frequency
#define MK64_FLEX_CLK            25             //flex clock frequency
#define MK64_FLASH_CLK           25             //flash clock frequency
/*
**SIM division factors
*/
#define MK64_CORE_DIV   0
#define MK64_BUS_DIV    1
#define MK64_FLEX_DIV   3
#define MK64_FLASH_DIV  3

/*
**DWT delay
*/
#define DELAY_MS(ms)    DWT_Delay_ms(ms)
#define DELAY_US(us)    DWT_Delay_us(us)

/*
**select whether printout debug information
*/
#define DEBUG_PRINT

/*
**configure assert and implement function
*/
void assert_failed(char *, int);

#if defined( DEBUG )
#define ASSERT(expr) \
    if (!(expr)) \
        assert_failed(__FILE__, __LINE__)
#else
#define ASSERT(expr)
#endif
          
/*
**configure debug printout function
*/
#if( defined(DEBUG) && defined(DEBUG_PRINT))
#define DEBUG_PRINTF(FORMAT,...)        do{printf(FORMAT,##__VA_ARGS__);}while(0)	/*please comment when don't need to printout debug information*/
#else
#define DEBUG_PRINTF(FORMAT,...)
#endif

/*
**variate declaration
*/
extern uint8 Vector_Str[][15];

/*
**function declaration
*/
extern void default_isr(void);


#endif