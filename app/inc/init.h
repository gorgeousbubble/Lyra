/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       init.h
 * @brief      System and peripheral initialization
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __INIT_H__
#define __INIT_H__

#include "common.h"

extern void Interrupt_Init(void); // Configure and enable PIT0, PIT1, RTC interrupts
extern void AllInit(void);        // Full system initialization (call once at startup)
extern void ReadConf(void);       // Load persistent configuration from E2PROM

#endif
