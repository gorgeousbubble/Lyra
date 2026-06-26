/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       init.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12 — Hardware and application initialisation
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "adc.h"
#include "animation.h"
#include "beep.h"
#include "ftm.h"
#include "health.h"
#include "i2c.h"
#include "init.h"
#include "it.h"
#include "filter.h"
#include "max30102.h"
#include "maps_dock_key.h"
#include "maps_dock_lcd.h"
#include "maps_dock_lcm.h"
#include "maps_dock_led.h"
#include "maps_dock_rocker.h"
#include "maps_dock_w25q80.h"
#include "misc.h"
#include "mpu6050.h"
#include "oled.h"
#include "oled_i2c.h"
#include "pit.h"
#include "rtc.h"
#include "system_init.h"
#include "uart.h"
#include "watch.h"

/*
 *  @brief      Interrupt_Init — Configure and enable all interrupt sources
 *  @since      v1.0
 *
 *  Interrupt priority assignments:
 *    PIT0  priority 2 — 1ms tick (stopwatch, RTC-update flag)
 *    PIT1  priority 3 — 1ms tick (sensor-read flags at 10ms / 5ms offset)
 *    RTC   priority 4 — RTC overflow / alarm
 *
 *  Note: PTC19 and PTD15 external-interrupt lines are wired on the board
 *  but are not used by this firmware.  The GPIO pins are left as inputs
 *  (default state after reset) and no ISRs are registered for them.
 */
void Interrupt_Init(void)
{
    PIT_Init_ms(PIT0, 1);
    set_irq_priority(PIT0_IRQn, 2);
    Set_Vector_Handler(PIT0_VECTORn, PIT0_IRQHandler);
    enable_irq(PIT0_IRQn);

    PIT_Init_ms(PIT1, 1);
    set_irq_priority(PIT1_IRQn, 3);
    Set_Vector_Handler(PIT1_VECTORn, PIT1_IRQHandler);
    enable_irq(PIT1_IRQn);

    set_irq_priority(RTC_IRQn, 4);
    Set_Vector_Handler(RTC_VECTORn, RTC_IRQHandler);
    enable_irq(RTC_IRQn);
}

/*
 *  @brief      AllInit — Full hardware and application initialisation
 *  @since      v1.0
 *
 *  Sequence (interrupts disabled throughout):
 *    1. Board peripherals  : LEDs, LCM, keys, rocker, W25Q80 Flash
 *    2. LCD display
 *    3. MCU peripherals    : ADC (×2), I2C0, UART4
 *    4. Sensors            : MPU6050, OLED, MAX30102/health, RTC, beeper
 *    5. Sensor fusion      : Kalman/complementary filter init (alpha=0.93, dt=10ms)
 *    6. Interrupts         : PIT0, PIT1, RTC
 *
 *  To set the RTC time at first boot, use the RTC menu under
 *  "Configure Adjust" rather than hard-coding a time here.
 */
void AllInit(void)
{
    DisableInterrupts;

    /* ---- Board peripherals ---- */
    MAPS_Dock_LED_Init();
    MAPS_Dock_LCM_Init();
    MAPS_Dock_KEY_ALL_Init();
    MAPS_Dock_Rocker_Key_Init();
    MAPS_Dock_W25Q80_Init();

    /* ---- LCD ---- */
    MAPS_LCD_Init();

    /* ---- MCU peripherals ---- */
    ADC_Init(ADC0_DP0);
    ADC_Init(ADC0_DM0);
    I2C_Init(I2C_I2C0, 400000);    /* 400 kHz */
    UART_Init(UART_UART4, 115200);  /* 115200 bps telemetry */

    /* ---- Sensors ---- */
    MPU6050_Init();
    Oled_I2C_Init();
    Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Init();
    RTC_Init();
    Beep_Init();

    /* ---- Sensor fusion filter ----
     * Complementary / Kalman filter: alpha = 0.93 (93% gyro, 7% accel),
     * dt = 0.01 s (called at 100 Hz from MPU6050_Read_Flag block). */
    Fusion_Init(&FF, 0.93f, 0.01f);

    /* ---- Interrupts ---- */
    Interrupt_Init();

    EnableInterrupts;
}

/*
 *  @brief      ReadConf — Read persistent configuration from W25Q80 Flash
 *  @since      v1.0
 *
 *  Loads:
 *    - Alarm clock list  (Sector 0, Page 0)
 *    - Time-format tense (Sector 1, Page 1, 12H/24H)
 */
void ReadConf(void)
{
    Read_Alarm_Clock_E2PROM_To_List();
    Read_Configure_Adjust_Tense_E2PROM_To_Value();
}
