/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       pdb.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __PDB_H__
#define __PDB_H__

#include "common.h"

/*
**PDB trigger source
*/
typedef enum
{
  PDB_External_Trigger,         //External trigger source
  PDB_CMP0_Trigger,             //External trigger CMP0
  PDB_CMP1_Trigger,             //External trigger CMP1
  PDB_CMP2_Trigger,             //External trigger CMP2
  PDB_PIT0_Trigger,             //External trigger PIT0
  PDB_PIT1_Trigger,             //External trigger PIT1
  PDB_PIT2_Trigger,             //External trigger PIT2
  PDB_PIT3_Trigger,             //External trigger PIT3
  PDB_FTM0_Trigger,             //External trigger FTM0
  PDB_FTM1_Trigger,             //External trigger FTM1
  PDB_FTM2_Trigger,             //External trigger FTM2
  PDB_FTM3_Trigger,             //External trigger FTM3
  PDB_RTC_Alarm_Trigger,        //External trigger RTC_Alarm
  PDB_RTC_Seconds_Trigger,      //External trigger RTC_Seconds
  PDB_LPTMR_Trigger,            //External trigger LMPTR
  PDB_Software_Trigger,         //Software trigger
}PDB_Trigger_Source;


#endif