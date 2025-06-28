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
**晶振频率OSC(50MHz)
*/
#define EXTAL_IN_MHz            50

/*
**PLL锁相环频率设置宏
*/
#define MK64_CORE_PLL_CLK       PLL120          //内核PLL频率(PLL150-PLL180比较稳定,建议PLL150)

#define MK64_CORE_CLK           120             //内核时钟频率
#define MK64_BUS_CLK             60             //总线时钟频率
#define MK64_FLEX_CLK            30             //FLEX时钟频率
#define MK64_FLASH_CLK           30             //FLASH时钟频率
/*
**SIM分频因子设置
*/
#define MK64_CORE_DIV   0
#define MK64_BUS_DIV    1
#define MK64_FLEX_DIV   3
#define MK64_FLASH_DIV  3

/*
**DWT延时
*/
#define DELAY_MS(ms)    DWT_Delay_ms(ms)
#define DELAY_US(us)    DWT_Delay_us(us)

/*
**选择是否输出调试信息，不输出则注释下面的宏定义
*/
#define DEBUG_PRINT

/*
**配置断言和其实现函数
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
**配置调试输出函数
*/
#if( defined(DEBUG) && defined(DEBUG_PRINT))
#define DEBUG_PRINTF(FORMAT,...)        do{printf(FORMAT,##__VA_ARGS__);}while(0)	/*无需打印调试信息时，请将宏内容注释掉*/
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