/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       main.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "adc.h"
#include "activity_history.h"
#include "adc_scope.h"
#include "alarm.h"
#include "attitude3d.h"
#include "freefall.h"
#include "func.h"
#include "gyro_dash.h"
#include "health.h"
#include "health_monitor.h"
#include "health_score.h"
#include "init.h"
#include "it.h"
#include "filter.h"
#include "main.h"
#include "maps_dock_lcd.h"
#include "maps_dock_key.h"
#include "maps_dock_rocker.h"
#include "max30102.h"
#include "misc.h"
#include "mpu6050.h"
#include "pedometer.h"
#include "rtc.h"
#include "sleep_monitor.h"
#include "tilt_alarm.h"

/*
** MAPS Screen Status
*/
MAPS_Screen_Status MAPS_Screen_StatusN[MAPS_Screen_Status_Max] = {
    MAPS_Screen_Saver,
    MAPS_Screen_Menu,
    MAPS_Screen_Normal,
};

/*
** MAPS Menu Selection
*/
MAPS_Menu_Selection MAPS_Menu_SelectionN[MAPS_Menu_Selection_Max] = {
    MAPS_Menu_Clock,
    MAPS_Menu_StopWatch,
    MAPS_Menu_AlarmClock,
    MAPS_Menu_WorldClock,
    MAPS_Menu_SpiritLevel,
    MAPS_Menu_Pedometer,
    MAPS_Menu_Attitude3D,
    MAPS_Menu_TiltAlarm,
    MAPS_Menu_GyroDash,
    MAPS_Menu_FreeFall,
    MAPS_Menu_HealthMonitor,
    MAPS_Menu_ActivityHistory,
    MAPS_Menu_SleepMonitor,
    MAPS_Menu_AdcScope,
    MAPS_Menu_HealthScore,
    MAPS_Menu_Configure_Adjust,
};

/*
** MAPS Clock Style
*/
MAPS_Clock_Style MAPS_Clock_StyleN[MAPS_Clock_Style_Max] = {
    MAPS_Clock_Dial,
    MAPS_Clock_Digit,
};

/*
** MAPS Alarm Clock Mode
*/
MAPS_AlarmClock_Mode MAPS_AlarmClock_ModeN[MAPS_AlarmClock_Mode_Max] = {
    MAPS_AlarmClock_List,
    MAPS_AlarmClock_Edit,
};

/*
** MAPS World Clock Timezone
*/
MAPS_WorldClock_Timezone MAPS_WorldClock_TimezoneN[MAPS_WorldClock_Timezone_Max] = {
    MAPS_WorldClock_Timezone_Beijing,
    MAPS_WorldClock_Timezone_Shanghai,
    MAPS_WorldClock_Timezone_Hongkong,
    MAPS_WorldClock_Timezone_Taipei,
    MAPS_WorldClock_Timezone_Singapore,
    MAPS_WorldClock_Timezone_Seoul,
    MAPS_WorldClock_Timezone_Tokyo,
    MAPS_WorldClock_Timezone_Sydney,
    MAPS_WorldClock_Timezone_SanFrancisco,
    MAPS_WorldClock_Timezone_NewYork,
};

/*
** MAPS World Clock Timezone Array
*/
MAPS_WorldClock_Time MAPS_WorldClock_Timezone_Array[MAPS_WorldClock_Timezone_Max] = {
    {MAPS_WorldClock_Timezone_Beijing, 8, 0},
    {MAPS_WorldClock_Timezone_Shanghai, 8, 0},
    {MAPS_WorldClock_Timezone_Hongkong, 8, 0},
    {MAPS_WorldClock_Timezone_Taipei, 8, 0},
    {MAPS_WorldClock_Timezone_Singapore, 8, 0},
    {MAPS_WorldClock_Timezone_Seoul, 9, 0},
    {MAPS_WorldClock_Timezone_Tokyo, 9, 0},
    {MAPS_WorldClock_Timezone_Sydney, 10, 0},
    {MAPS_WorldClock_Timezone_SanFrancisco, -8, 0},
    {MAPS_WorldClock_Timezone_NewYork, -5, 0},
};

/*
** MAPS Configure Adjust Mode
*/
MAPS_ConfigureAdjust_Mode MAPS_ConfigureAdjust_ModeN[MAPS_ConfigureAdjust_Max] = {
    MAPS_ConfigureAdjust_List,
    MAPS_ConfigureAdjust_Clock,
    MAPS_ConfigureAdjust_Date,
    MAPS_ConfigureAdjust_Tense,
};

/*
** MAPS Configure Adjust Tense Format
*/
MAPS_ConfigureAdjust_Tense_Format MAPS_ConfigureAdjust_Tense_FormatN[2] = {
    MAPS_ConfigureAdjust_Tense_24H,
    MAPS_ConfigureAdjust_Tense_12H,
};

/*
 *  @brief              main
 *  @note               main function
 *  @since              v1.0
 *  Sample usage:       void main(void)
 */
int main(void)
{
        Site_t Site = {0, 0};

        AllInit();  // initialization
        ReadConf(); // read configuration from e2prom
        Activity_Load(); // load 7-day step history from Flash

        MAPS_LCDC_BMP_From_SD("0:/Mitsuha.bmp", Site); // load image

        for (;;)
        {
                // --- 10ms periodic: MPU6050 I2C read + filter (moved from PIT1 ISR) ---
                if (MPU6050_Read_Flag)
                {
                    MPU6050_Read_Flag = 0;

                    // MPU6050 sensor data read (I2C bit-bang, ~200us)
                    MPU6050.Acc.X = MPU_Get_Acc_X();
                    MPU6050.Acc.Y = MPU_Get_Acc_Y();
                    MPU6050.Acc.Z = MPU_Get_Acc_Z();
                    MPU6050.Gyro.X = MPU_Get_Gyro_X();
                    MPU6050.Gyro.Y = MPU_Get_Gyro_Y();
                    MPU6050.Gyro.Z = MPU_Get_Gyro_Z();

                    // Gyro calibration or fusion filter
                    if (!FF.gyro_bias.calibrated)
                    {
                        Fusion_Calibrate(&FF, MPU6050.Gyro.X, MPU6050.Gyro.Y, MPU6050.Gyro.Z);
                    }
                    else
                    {
                        Fusion_Filter(&FF, MPU6050.Acc.X, MPU6050.Acc.Y, MPU6050.Acc.Z,
                                           MPU6050.Gyro.X, MPU6050.Gyro.Y, MPU6050.Gyro.Z);
                    }

                    // Step detection (runs every 10ms regardless of calibration state)
                    Pedometer_Update(MPU6050.Acc.X, MPU6050.Acc.Y, MPU6050.Acc.Z, 10);

                    // Tilt alarm check (runs every 10ms, after filter is updated)
                    TiltAlarm_Update(FF.pitch.angle, FF.roll.angle);

                    // Gyro dashboard: push latest raw gyro sample into ring buffer
                    GyroDash_Update(MPU6050.Gyro.X, MPU6050.Gyro.Y, MPU6050.Gyro.Z);

                    // Free-fall / impact detection
                    FreeFall_Update(MPU6050.Acc.X, MPU6050.Acc.Y, MPU6050.Acc.Z, RTC_Count);

                    // Alarm clock: check hour:minute against alarm list every 10ms
                    // Pass snapshotted values — avoids touching volatile RTC_Time_Now
                    // directly (race-condition fix: RTC_Time_Now is written non-atomically
                    // in the RTC_Update_Flag block; we read from locally snapshotted copies
                    // captured there, but here we just read the individual int fields which
                    // are 32-bit aligned and each updated atomically by the CPU).
                    // Using a dedicated atomic snapshot is safest:
                    {
                        int alarm_h, alarm_m;
                        DisableInterrupts;
                        alarm_h = RTC_Time_Now.Hour;
                        alarm_m = RTC_Time_Now.Minute;
                        EnableInterrupts;
                        Alarm_Tick(alarm_h, alarm_m);
                    }

                    // Sleep monitoring (30-second RMS window, called at 100Hz)
                    SleepMonitor_Update(MPU6050.Acc.X, MPU6050.Acc.Y, MPU6050.Acc.Z, RTC_Count);

                    // ADC scope: sample both channels at 100Hz for oscilloscope display
                    {
                        uint16 adc0 = ADC_Once(ADC0_DP0, ADC_12Bit);
                        uint16 adc1 = ADC_Once(ADC0_DM0, ADC_12Bit);
                        AdcScope_Update(adc0, adc1);
                        /* Keep ADC_Convert_Result in sync for rocker display */
                        ADC_Convert_Result[0] = adc0;
                        ADC_Convert_Result[1] = adc1;
                    }

                    UART_Send_Flag = 1; // Signal UART send after filter update
                }

                // --- 10ms periodic (5ms offset): MAX30102 I2C read (moved from PIT1 ISR) ---
                if (MAX30102_Read_Flag)
                {
                    MAX30102_Read_Flag = 0;

                    uint32 red = 0, ir = 0;
                    MAX30102_ReadFIFO(&red, &ir);
                    MAX30102_RED = red;
                    MAX30102_IR  = ir;
                    Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Collect(red, ir);

                    // Feed IR sample to waveform display (downsampled to ~10Hz inside)
                    HealthMonitor_Update(ir);
                }

                // --- 100ms periodic: ADC + RTC calendar update (moved from PIT0 ISR) ---
                if (RTC_Update_Flag)
                {
                    RTC_Update_Flag = 0;

                    // ADC now sampled at 100Hz in MPU6050 block — just update RTC here
                    uint32 rtc_snap = RTC_Get_Time();

                    // ----------------------------------------------------------------
                    // Compute all calendar fields into LOCAL variables first.
                    // Only then write to RTC_Time_Now under a critical section.
                    // This prevents a torn-read: PIT0 ISR fires every 1ms and UI
                    // code reads RTC_Time_Now; if we wrote fields one-by-one without
                    // protection the ISR could observe a half-updated struct (e.g.
                    // Hour/Minute already flipped to 00:00 but Month/Day still showing
                    // the previous day's values).
                    // ----------------------------------------------------------------

                    // --- Time-of-day ---
                    uint32 t    = rtc_snap;
                    uint32 sec  = t % 60; t /= 60;
                    uint32 min  = t % 60; t /= 60;
                    uint32 hour = t % 24;

                    // --- Date: days since Unix epoch → year/month/day ---
                    uint32 days = rtc_snap / 86400UL;
                    uint32 y    = 1970;
                    while (1) {
                        uint32 diy = ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) ? 366 : 365;
                        if (days < diy) break;
                        days -= diy;
                        y++;
                    }

                    static const uint16 mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
                    uint32 leap = ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) ? 1 : 0;
                    uint32 m = 0;
                    for (m = 0; m < 12; m++) {
                        uint32 dm = mdays[m] + ((m == 1) ? leap : 0);
                        if (days < dm) break;
                        days -= dm;
                    }

                    // --- Atomic commit: disable interrupts while writing the struct ---
                    // Critical section is ~6 STR instructions (~50 ns @ 120 MHz).
                    DisableInterrupts;
                    RTC_Count           = rtc_snap;
                    RTC_Time_Now.Hour   = (int)hour;
                    RTC_Time_Now.Minute = (int)min;
                    RTC_Time_Now.Second = (int)sec;
                    RTC_Time_Now.Year   = (int)y;
                    RTC_Time_Now.Month  = (int)(m + 1);
                    RTC_Time_Now.Day    = (int)(days + 1);
                    EnableInterrupts;

                    // Activity history: check for date rollover every 100ms
                    Activity_Tick((int)y, (int)(m + 1), (int)(days + 1));
                }

                // --- UART telemetry ---
                if (UART_Send_Flag)
                {
                    UART_Send_Flag = 0;
                    UART_Send_Parameters();
                }

                // --- UI handling ---
                MAPS_Dock_KEY_Incident();            // OLED key handling and display
                MAPS_Dock_Rocker_Key_LCM_Control();  // LCM rocker key handling and display

                // --- SpO2/HR algorithm (runs when buffer full) ---
                Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Calculate();
        }
}