/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       wdog.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "misc.h"
#include "wdog.h"

/*
 *  @brief      WDOG unlock
 *  @since      v1.0
 */
void WDOG_Unlock(void)
{
  uint8 WDOG_Temp=0;
  
  WDOG_Temp = __get_BASEPRI();//Return the value of PRIMASK register (1-bit): 1: close interrupt 0: Open interrupt
  
  DisableInterrupts;//close all interrupts
  
  WDOG_UNLOCK = 0xC520;//write 0xC520 and 0xD928 to the WDOG-UNLOCK register to unlock WDOG
  WDOG_UNLOCK = 0xD928;
  
  if(WDOG_Temp == 0)//before unlocking, it was interrupted
  {
    EnableInterrupts;//open all interrupts
  }
  
}

/*
 *  @brief      WDOG feed (counter update)
 *  @since      v1.0
 */
void WDOG_Feed(void)
{
  uint8 WDOG_Temp=0;
  
  WDOG_Temp = __get_BASEPRI();//Return the value of PRIMASK register (1-bit): 1: close interrupt 0: Open interrupt
  
  DisableInterrupts;//close all interrupts
  
  WDOG_REFRESH = 0xA602;//update WDOG counter, Feed Dog
  WDOG_REFRESH = 0xB480;
  
  if(WDOG_Temp == 0)//before unlocking, it was interrupted
  {
    EnableInterrupts;//open all interrupts
  }
}

/*
 *  @brief      WDOG disable
 *  @since      v1.0
 */
void WDOG_Disable(void)
{
  WDOG_Unlock();//unlock WDOG,configure WDOG register
  WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;//WDOGEN clear to 0, disable WDOG
}

/*
 *  @brief      WDOG enable
 *  @since      v1.0
 */
void WDOG_Enable(void)
{
  WDOG_Unlock();//unlock WDOG,configure WDOG register
  WDOG_STCTRLH |= WDOG_STCTRLH_WDOGEN_MASK;//WDOGEN set to 1, enable WDOG
}

/*
 *  @brief      WDOG init
 *  @param      WDOG_Cnt
 *  @since      v1.0
 */
void WDOG_Init(uint32 WDOG_Cnt)
{
  ASSERT(WDOG_Cnt >= 4);//the minimum counting clock is 4 milliseconds
  
  WDOG_Unlock();//unlock WDOG,configure WDOG register
  
  WDOG_PRESC = WDOG_PRESC_PRESCVAL(0);//set the division factor to PRESSCVAL+1 (PRESSCVAL values range from 0 to 7)
  
  WDOG_TOVALH = WDOG_Cnt >> 16;//Set WDOG time
  WDOG_TOVALL = (uint16)(WDOG_Cnt & 0x0000ffff);
  
  WDOG_STCTRLH = (0 | WDOG_STCTRLH_WDOGEN_MASK          //WDOG enable
                    | WDOG_STCTRLH_ALLOWUPDATE_MASK     //WDOG allow update
                    | WDOG_STCTRLH_STOPEN_MASK          //WDOG stop mode
                    | WDOG_STCTRLH_WAITEN_MASK          //WDOG wait mode
                  );
  
}