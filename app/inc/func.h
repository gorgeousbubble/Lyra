/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       func.h
 * @brief      UART data transmission for host-side visualization
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __FUNC_H__
#define __FUNC_H__

#include "common.h"

// Send one frame of MPU6050 filter data over UART4 to the host PC
extern void UART_Send_Parameters(void);

#endif
