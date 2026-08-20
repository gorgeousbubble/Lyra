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

/* Global buzzer mute flag (0 = sound on, 1 = muted). Overwritten at boot by
 * Read_Configure_Adjust_Sound_E2PROM_To_Value() and by the Configure menu. */
uint8 Beep_Muted = 0;

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
    // When globally muted, keep the buzzer off regardless of caller.
    if (Beep_Muted)
    {
        BEEP_SET_L;
        return;
    }
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