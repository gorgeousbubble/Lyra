/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       health_monitor.h
 * @brief      Heart rate and SpO2 display module (MAX30102)
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __HEALTH_MONITOR_H__
#define __HEALTH_MONITOR_H__

#include "common.h"

/*
 * IR waveform ring buffer for real-time pleth display.
 * Stores the most recent HEALTH_WAVE_BUF IR samples (downsampled to ~10Hz).
 * At 10Hz, 64 samples cover 6.4 seconds.
 */
#define HEALTH_WAVE_BUF  64     /* columns available on left side */
#define HEALTH_WAVE_H    20     /* pixel height of waveform area  */

typedef struct
{
    uint32 ir_wave[HEALTH_WAVE_BUF]; /* raw IR values */
    int    head;                     /* next write position */
    int    count;                    /* samples filled */
    uint32 min_val;                  /* min in buffer (for scaling) */
    uint32 max_val;                  /* max in buffer (for scaling) */
    uint32 downsample_cnt;           /* counter for downsampling */
} HealthWave;

extern HealthWave HWave;

/*
 * Push one new IR sample (call every 10ms from MAX30102 read block).
 * Downsamples by factor 10 → effectively ~10Hz waveform.
 */
extern void HealthMonitor_Update(uint32 ir_raw);

/* Render the health monitor screen to OLED */
extern void Render_HealthMonitor(void);

#endif
