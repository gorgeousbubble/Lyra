/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       beep.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "beep.h"
#include "gpio.h"

/*
** @brief      Beep Initialization
** @since      v1.0
*/
void Beep_Init(void)
{
    // Initialize the beep GPIO pin
    BEEP_PORT_INIT;
}

/*
** @brief      Beep On
** @since      v1.0
*/
void Beep_On(void)
{
    // Set the beep GPIO pin high
    BEEP_SET_H;
}

/*
** @brief      Beep Off
** @since      v1.0
*/
void Beep_Off(void)
{
    // Set the beep GPIO pin low
    BEEP_SET_L;
}