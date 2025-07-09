/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       rtc.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __RTC_H__
#define __RTC_H__

#include "common.h"
#include <time.h>

/*
**RTC macros
*/
#define RTC_ALARM_IRQ_EN()      RTC_IER |= RTC_IER_TAIE_MASK      //Enable alarm clock interruption
#define RTC_ALARM_IRQ_DIS()     RTC_IER &= ~RTC_IER_TAIE_MASK     //Disable alarm clock interruption

#define RTC_OVERFLOW_IRQ_EN()   RTC_IER |= RTC_IER_TOIE_MASK      //Enable overflow interrupt
#define RTC_OVERFLOW_IRQ_DIS()  RTC_IER &= ~RTC_IER_TOIE_MASK     //Disable overflow interrupt

#define RTC_INVALID_IRQ_EN()    RTC_IER |= RTC_IER_TIIE_MASK      //Enable invalid interrupt settings
#define RTC_INVALID_IRQ_DIS()   RTC_IER &= ~RTC_IER_TIIE_MASK     //Disable invalid interrupt settings

/*
**function declaration
*/
extern void RTC_Init(void);
extern void RTC_Set_Time(uint32 Seconds);
extern void RTC_Set_Time_Format(struct tm *timeinfo);
extern uint32 RTC_Get_Time(void);
extern struct tm RTC_Get_Time_Format(void);
extern uint8 RTC_Set_Alarm(uint32 Alarm);
extern void RTC_Close_Alarm(void);
extern void RTC_Test_Handler(void);

#endif