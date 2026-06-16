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
#include "attitude3d.h"
#include "freefall.h"
#include "func.h"
#include "gyro_dash.h"
#include "health.h"
#include "health_monitor.h"
#include "init.h"
#include "it.h"
#include "filter.h"
#include "main.h"
#include "maps_dock_lcd.h"
#include "maps_dock_key.h"
#include "maps_dock_rocker.h"
#include "max30102.h"
#include "mpu6050.h"
#include "pedometer.h"
#include "rtc.h"
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

                    ADC_Convert_Result[0] = ADC_Once(ADC0_DP0, ADC_12Bit);
                    ADC_Convert_Result[1] = ADC_Once(ADC0_DM0, ADC_12Bit);
                    RTC_Count = RTC_Get_Time();

                    // Simple seconds-to-calendar conversion (replaces gmtime in ISR)
                    uint32 t = RTC_Count;
                    uint32 sec = t % 60; t /= 60;
                    uint32 min = t % 60; t /= 60;
                    uint32 hour = t % 24;
                    RTC_Time_Now.Hour   = (int)hour;
                    RTC_Time_Now.Minute = (int)min;
                    RTC_Time_Now.Second = (int)sec;

                    // Full date calculation using days since epoch
                    uint32 days = RTC_Count / 86400;
                    uint32 y = 1970;
                    while (1) {
                        uint32 diy = ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) ? 366 : 365;
                        if (days < diy) break;
                        days -= diy;
                        y++;
                    }
                    RTC_Time_Now.Year = (int)y;

                    static const uint16 mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
                    uint32 leap = ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) ? 1 : 0;
                    uint32 m = 0;
                    for (m = 0; m < 12; m++) {
                        uint32 dm = mdays[m] + ((m == 1) ? leap : 0);
                        if (days < dm) break;
                        days -= dm;
                    }
                    RTC_Time_Now.Month = (int)(m + 1);
                    RTC_Time_Now.Day   = (int)(days + 1);
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