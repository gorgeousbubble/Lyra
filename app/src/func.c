/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       func.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "func.h"
#include "it.h"
#include "uart.h"

/*
 * @brief      UART_Send_Parameters
 * @param      uint8* array        OUT   Pointer to the array to store the world clock time coordinates
 * @param      const Coord *city   IN    Pointer to the city coordinate array
 * @param      const int cityLen   IN    Length of the city coordinate array
 * @param      MAPS_WorldClock_Time time IN    World clock time structure
 * @param      int hour            IN    Local hour
 * @param      int minute          IN    Local minute
 * @return     void
 * @since      v1.0
 * Sample usage:       Calc_World_Clock_Time(&array, LCM_WorldClock_City_coordinate, LCM_WorldClock_City_coordinate_length, time, 10, 30);
*/
void UART_Send_Parameters()
{
    signed int temp;
    unsigned char temp1, temp2;
    
    UART_PutChar(UART_UART4, 0xFF);
    UART_PutChar(UART_UART4, 0x00);
    UART_PutChar(UART_UART4, 0x16);
    UART_PutChar(UART_UART4, 0x08);
      
    /*Acc.X(1)*/
    temp = (signed int)FF_X.accAngle;  
    temp1 = (temp) >> 8;
    temp2 = (temp) & 0xFF;
    UART_PutChar(UART_UART4, temp1);
    UART_PutChar(UART_UART4, temp2);
    
    /*Acc.Y(2)*/
    temp = (signed int)FF_X.gyroRate; 
    temp1 = (temp) >> 8;
    temp2 = (temp) & 0xFF;
    UART_PutChar(UART_UART4, temp1);
    UART_PutChar(UART_UART4, temp2);
    
    /*Acc.Z(3)*/
    temp = (signed int)FF_Y.accAngle;        
    temp1 = (temp) >> 8;
    temp2 = (temp) & 0xFF;
    UART_PutChar(UART_UART4, temp1);
    UART_PutChar(UART_UART4, temp2);

    /*Gyro.X(4)*/
    temp = (signed int)FF_Y.gyroRate; // MPU6050.Gyro.X
    temp1 = (temp) >> 8;
    temp2 = (temp) & 0xFF;
    UART_PutChar(UART_UART4, temp1);
    UART_PutChar(UART_UART4, temp2);

    /*Gyro.Y(5)*/
    temp = (signed int)FF_Z.accAngle;  // MPU6050.Gyro.Y
    temp1 = (temp) >> 8;
    temp2 = (temp) & 0xFF;
    UART_PutChar(UART_UART4, temp1);
    UART_PutChar(UART_UART4, temp2);

    /*Gyro.Z(6)*/
    temp = (signed int)FF_Z.gyroRate;  // MPU6050.Gyro.Z
    temp1 = (temp) >> 8;
    temp2 = (temp) & 0xFF;
    UART_PutChar(UART_UART4, temp1);
    UART_PutChar(UART_UART4, temp2);

    UART_PutChar(UART_UART4, 0xAA);
    UART_PutChar(UART_UART4, 0x55);
}