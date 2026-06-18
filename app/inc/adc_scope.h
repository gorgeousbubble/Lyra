/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       adc_scope.h
 * @brief      Dual-channel ADC oscilloscope (ADC0_DP0 / ADC0_DM0, 12-bit, 100Hz)
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __ADC_SCOPE_H__
#define __ADC_SCOPE_H__

#include "common.h"

/* Ring buffer depth — 128 samples per channel */
#define ADC_SCOPE_BUF  128

/* Decimation factors selectable by KEY2/KEY3 (display every Nth sample) */
#define ADC_SCOPE_DEC_MIN  1   /* ×1 : 128 samples = 1.28s  @100Hz */
#define ADC_SCOPE_DEC_MAX  8   /* ×8 : 128 samples = 10.24s @100Hz */

/* CH layout on 128×64 OLED:
   CH0: y = 0..30  (31px, baseline y=15)
   CH1: y = 33..63 (31px, baseline y=48)
   Separator: y=31..32
*/
#define ADC_CH0_TOP  0
#define ADC_CH0_BOT  30
#define ADC_CH1_TOP  33
#define ADC_CH1_BOT  63
#define ADC_CH0_CY   15   /* centre row of CH0 */
#define ADC_CH1_CY   48   /* centre row of CH1 */
#define ADC_WAVE_H   15   /* half-height: ±15 px from centre */

typedef struct
{
    uint16 ch0_buf[ADC_SCOPE_BUF];  /* ADC0_DP0 ring buffer (12-bit, 0..4095) */
    uint16 ch1_buf[ADC_SCOPE_BUF];  /* ADC0_DM0 ring buffer                    */
    int    head;                    /* Next write index                         */
    int    count;                   /* Samples filled (0..ADC_SCOPE_BUF)        */
    uint8  paused;                  /* 1 = freeze display (KEY0)                */
    uint8  decimation;              /* Display every Nth sample (1/2/4/8)       */
    uint32 sample_tick;             /* Counter for decimation                   */
} AdcScopeState;

extern AdcScopeState AdcScope;

/*
 * Push one ADC sample pair (call every 10ms from main loop, after ADC read).
 * ch0, ch1: raw 12-bit ADC values (0..4095).
 */
extern void AdcScope_Update(uint16 ch0, uint16 ch1);

/* Toggle pause (KEY0) */
extern void AdcScope_Toggle_Pause(void);

/* Increase decimation factor (KEY3: slower / zoom out) */
extern void AdcScope_Dec_Inc(void);

/* Decrease decimation factor (KEY2: faster / zoom in) */
extern void AdcScope_Dec_Dec(void);

/* Render the oscilloscope screen to OLED */
extern void Render_AdcScope(void);

#endif
