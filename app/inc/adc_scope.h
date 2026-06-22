/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       adc_scope.h
 * @brief      Dual-channel ADC oscilloscope with auto-measurement (Vpp / Freq / Avg)
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

/* Vref for mV conversion (3300mV, 12-bit: 4095 steps) */
#define ADC_VREF_MV  3300
#define ADC_FULL_SCALE 4095

/* Frequency detection: zero-crossing threshold (midpoint of range) */
#define ADC_ZC_THRESHOLD  2048   /* 12-bit midpoint */

/* CH layout on 128×64 OLED — waveform only (narrower, no right margin):
   CH0: y = 0..25  (26px, centre y=12)
   Separator: y=26..27
   CH1: y = 28..52 (25px, centre y=40)
   Status bar: y=53..63
*/
#define ADC_CH0_TOP  0
#define ADC_CH0_BOT  25
#define ADC_CH1_TOP  28
#define ADC_CH1_BOT  52
#define ADC_CH0_CY   12
#define ADC_CH1_CY   40
#define ADC_WAVE_H   12   /* half-height per channel */

/* Waveform x range (leave right 44px for measurements) */
#define ADC_WAVE_X0   12  /* start (after "C0" label) */
#define ADC_WAVE_X1   82  /* end */
#define ADC_WAVE_W    (ADC_WAVE_X1 - ADC_WAVE_X0 + 1)  /* 71 pixels */

/* -----------------------------------------------------------------------
 * Auto-measurement results for one channel
 * ----------------------------------------------------------------------- */
typedef struct
{
    uint32 vpp_mv;      /* Peak-to-peak voltage in mV           */
    uint32 avg_mv;      /* Average voltage in mV                */
    uint32 freq_hz;     /* Estimated frequency in Hz (0=no sig) */
    uint16 v_min;       /* Min raw ADC value in buffer           */
    uint16 v_max;       /* Max raw ADC value in buffer           */
    uint8  freq_valid;  /* 1 = frequency measurement valid       */
} AdcMeasure;

typedef struct
{
    uint16 ch0_buf[ADC_SCOPE_BUF];
    uint16 ch1_buf[ADC_SCOPE_BUF];
    int    head;
    int    count;
    uint8  paused;
    uint8  decimation;
    uint32 sample_tick;
    AdcMeasure meas[2];   /* [0]=CH0, [1]=CH1 auto-measurements */
} AdcScopeState;

extern AdcScopeState AdcScope;

extern void AdcScope_Update(uint16 ch0, uint16 ch1);
extern void AdcScope_Toggle_Pause(void);
extern void AdcScope_Dec_Inc(void);
extern void AdcScope_Dec_Dec(void);

/* Compute auto-measurements from current buffer contents */
extern void AdcScope_Measure(void);

extern void Render_AdcScope(void);

#endif
