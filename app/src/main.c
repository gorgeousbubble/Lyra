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

#include "init.h"
#include "main.h"
#include "maps_dock_lcd.h"
#include "maps_dock_key.h"
#include "maps_dock_rocker.h"
#include "mpu6050.h"

/*
 *  @brief              main
 *  @note               main function
 *  @since              v1.0
 *  Sample usage:       void main(void)
 */
void main(void)
{
  Site_t Site={0,0};
  
  AllInit(); //initialization
  
  MAPS_LCDC_BMP_From_SD("0:/Mitsuha.bmp",Site);//load image
  
  for(;;)
  {
    MAPS_Dock_KEY_Incident();//Independent button serial port transmission
    MAPS_Dock_Rocker_Key_LCM_Control();//LCM display
  }
}