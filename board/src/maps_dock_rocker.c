/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       maps_dock_rocker.c
 * @brief      MAPS Four Color Board MAPS Dock External Expansion Board
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "dwt.h"
#include "gpio.h"
#include "it.h"
#include "maps_dock_lcm.h"
#include "maps_dock_rocker.h"

//Joystick button controls LCM
int Rocker_Key_Switch_Page=1;           //Current page selection
int Rocker_Key_Switch_Row=1;            //Current selection row
int Rocker_Key_Switch_Row_Old=1;        //Last selection row
int Rocker_Key_Switch_Choice_Stauts=0;  //Parameter selection status
int Rocker_Key_Switch_Choice_Flag=0;    //Button action symbol

/*
**Joystick button port
*/
PTXn Rocker_Key_PTXn[Rocker_MAX]={PTA27,PTA26,PTA25,PTA24,PTA4};//PTA4(NMI)

/*
 *  @brief      Rocker_Key initialization
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_Rocker_Key_Init();//Initialize joystick button
 */
void MAPS_Dock_Rocker_Key_Init(void)
{
  GPIO_Init(Rocker_Key_PTXn[Rocker_Up],GPI,0);//Rocker_Up button initialization
  GPIO_Init(Rocker_Key_PTXn[Rocker_Donw],GPI,0);//Rocker_Donw button initialization
  GPIO_Init(Rocker_Key_PTXn[Rocker_Left],GPI,0);//Rocker_Left button initialization
  GPIO_Init(Rocker_Key_PTXn[Rocker_Right],GPI,0);//Rocker_Right button initialization
  GPIO_Init(Rocker_Key_PTXn[Rocker_Select],GPI,0);//Rocker_Select button initialization
}

/*
 *  @brief      Rocker_Key joystick button controls LCM
 *  @since      v1.0
 *  Sample usage:       MAPS_Dock_Rocker_Key_LCM_Control();//Joystick button controls LCM
 */
void MAPS_Dock_Rocker_Key_LCM_Control(void)
{
  //Enter the interface
  if(Rocker_Key_Switch_Choice_Stauts == 0)
  {
    //Only by pressing the Select button can you enter the parameter interface
    if(GPIO_GET(Rocker_Key_PTXn[Rocker_Select]) == Rocker_Key_On)
    {
      Rocker_Key_Switch_Choice_Stauts = 1;
      MAPS_Dock_LCM_CLS();//Clear screen
      Rocker_Key_Delay(500);//Button delay
    }
  }
  //Key page selection interface
  else if(Rocker_Key_Switch_Choice_Stauts == 1)
  {
    //Press the left joystick button
    if(GPIO_GET(Rocker_Key_PTXn[Rocker_Left]) == Rocker_Key_On)
    {
      Rocker_Key_Switch_Page--;//Reduce the number of pages by one
      if(Rocker_Key_Switch_Page < 1)//Page count less than 1
      {
        Rocker_Key_Switch_Page = LCM_PAGES;//The number of pages is equal to the maximum number of pages
      }
      Rocker_Key_Switch_Row = 1;
      Rocker_Key_Switch_Row_Old = 1;
      Rocker_Key_Switch_Choice_Flag = 0;
      Rocker_Key_Switch_Choice_Stauts = 1;
      MAPS_Dock_LCM_CLS();//Clear screen
      Rocker_Key_Delay(500);//Button delay
    }
    //Press the right button on the joystick
    else if(GPIO_GET(Rocker_Key_PTXn[Rocker_Right]) == Rocker_Key_On)
    {
      Rocker_Key_Switch_Page++;//Page number plus one
      if(Rocker_Key_Switch_Page > LCM_PAGES)//The number of pages is greater than the maximum number of pages
      {
        Rocker_Key_Switch_Page = 1;//The number of pages is equal to 1
      }
      Rocker_Key_Switch_Row = 1;
      Rocker_Key_Switch_Row_Old = 1;
      Rocker_Key_Switch_Choice_Flag = 0;
      Rocker_Key_Switch_Choice_Stauts = 1;
      MAPS_Dock_LCM_CLS();//Clear screen
      Rocker_Key_Delay(500);//Button delay
    }
    //Press the up key on the joystick
    else if(GPIO_GET(Rocker_Key_PTXn[Rocker_Up]) == Rocker_Key_On)
    {
      Rocker_Key_Delay(500);//Button delay
    }
    //Press the down key on the joystick
    else if(GPIO_GET(Rocker_Key_PTXn[Rocker_Donw]) == Rocker_Key_On)
    {
      Rocker_Key_Delay(500);//Button delay
    }
  }
  
  
  //LCM parameter display
  if(Rocker_Key_Switch_Choice_Stauts == 0)
  {
    MAPS_Dock_LCM_DrawBMP(&LCM_Freescale_logo[0],LCM_Invert_Color);
  }
  else
  {
    switch(Rocker_Key_Switch_Page)
    {
      case 1:
              MAPS_Dock_LCM_Put_Str_6x8(37,0,"MAPS_MK64",LCM_Pure_Color);
              MAPS_Dock_LCM_Put_Str_6x8(49,1,"List1",LCM_Pure_Color);
              MAPS_Dock_LCM_Put_Para_6x8(0,2,"PIT_Count:",PIT_Count,LCM_Pure_Color);
              MAPS_Dock_LCM_Put_Para_6x8(0,3,"ADC0_DP0:",ADC_Convert_Result[0],LCM_Pure_Color);
              MAPS_Dock_LCM_Put_Para_6x8(0,4,"ADC0_DM0:",ADC_Convert_Result[1],LCM_Pure_Color);
              break;
      case 2:
              MAPS_Dock_LCM_Put_Str_6x8(37,0,"MAPS_MK64",LCM_Pure_Color);
              MAPS_Dock_LCM_Put_Str_6x8(49,1,"List2",LCM_Pure_Color);
              break;
      case 3:
              MAPS_Dock_LCM_Put_Str_6x8(37,0,"MAPS_MK64",LCM_Pure_Color);
              MAPS_Dock_LCM_Put_Str_6x8(49,1,"List3",LCM_Pure_Color);
              break;
      case 4:
              MAPS_Dock_LCM_Put_Str_6x8(37,0,"MAPS_MK64",LCM_Pure_Color);
              MAPS_Dock_LCM_Put_Str_6x8(49,1,"List4",LCM_Pure_Color);
              break;
      case 5:
              MAPS_Dock_LCM_Put_Str_6x8(37,0,"MAPS_MK64",LCM_Pure_Color);
              MAPS_Dock_LCM_Put_Str_6x8(49,1,"List5",LCM_Pure_Color);
              break;
      default:
              break;
    }
  }
  
}