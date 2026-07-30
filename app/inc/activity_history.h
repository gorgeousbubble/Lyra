/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       activity_history.h
 * @brief      7-day step count history stored in W25Q80 Flash (Page 2)
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __ACTIVITY_HISTORY_H__
#define __ACTIVITY_HISTORY_H__

#include "common.h"

/* -----------------------------------------------------------------------
 * Flash storage layout (W25Q80, Sector 2 / Page 32, 256 bytes)
 *
 *   Bytes 0..27  : 7 × ActivityDay records (4 bytes each, big-endian uint32)
 *     [0..3]  = Day 0 (oldest) step count
 *     [4..7]  = Day 1 ...
 *     ...
 *     [24..27]= Day 6 (yesterday's completed day)
 *   Bytes 28..31 : Today's accumulated steps (partial count)
 *   Byte  32     : history_len  (0..7)
 *   Byte  33     : last_saved_year  (year - 1970, fits uint8 until 2225)
 *   Byte  34     : last_saved_month (1..12)
 *   Byte  35     : last_saved_day   (1..31)
 *   Byte  36     : CRC-8 of bytes 0..35  ← integrity checksum (NEW)
 *   Bytes 37..255: Reserved (0xFF)
 *
 * CRC-8 polynomial: 0x07 (same as CRC-8/SMBUS).
 * If the stored CRC does not match bytes 0..35, the record is treated as
 * corrupted and the state is reset to zero (same as a fresh Flash).
 *
 * On power-up:  Read record from Flash, verify CRC, populate RAM state.
 * Each day:     Date rollover → rotate ring buffer, write Flash.
 * KEY0:         Manual checkpoint → write Flash (rate-limited, see below).
 * ----------------------------------------------------------------------- */

#define ACTIVITY_FLASH_PAGE       2       /* Legacy name kept for reference */
#define ACTIVITY_DAYS             7       /* History depth (days)           */
#define ACTIVITY_RECORD_SIZE      37      /* 36 data bytes + 1 CRC byte     */

/* Minimum seconds between two consecutive manual Flash saves (KEY0).
 * W25Q80 endurance: ~10,000 erase cycles per sector.
 * At one save per 5 minutes the chip lasts > 95 years of continuous use.
 * Date-rollover saves are NOT rate-limited (at most once per day).      */
#define ACTIVITY_SAVE_THROTTLE_S  300     /* 5 minutes                      */

/* Daily step goal.
 * ACTIVITY_GOAL_STEPS is the factory default. The live value is held in the
 * runtime variable Activity_Step_Goal, which is user-configurable through the
 * Configure-Adjust menu and persisted in Flash (Sector 3 / Page 48).
 * The activity chart and the health score both read Activity_Step_Goal. */
#define ACTIVITY_GOAL_STEPS       10000   /* factory default */
#define STEP_GOAL_MIN             1000    /* configurable lower bound */
#define STEP_GOAL_MAX             50000   /* configurable upper bound (fits uint16) */
#define STEP_GOAL_STEP            500     /* KEY2/KEY3 increment */

extern uint32 Activity_Step_Goal;         /* live, persisted daily step goal */

typedef struct
{
    uint32 steps;       /* Daily step count */
} ActivityDay;

typedef struct
{
    ActivityDay history[ACTIVITY_DAYS]; /* [0]=oldest, [6]=yesterday    */
    int         history_len;            /* Entries actually filled 0..7 */
    uint32      today_steps;            /* Live counter (from Pedometer) */
    uint8       last_saved_year;        /* For date-change detection     */
    uint8       last_saved_month;
    uint8       last_saved_day;
    uint8       loaded;                 /* 1 = Flash data loaded         */
    uint8       view_mode;              /* 0=week bar, 1=day detail      */
    /* Rate-limiting for manual saves (KEY0).
     * Stores the RTC_Count value at the time of the last Flash write.
     * A value of 0 means "never written" (first save always allowed). */
    uint32      last_flash_rtc;         /* RTC seconds of last Flash write */
} ActivityHistoryState;

extern ActivityHistoryState Activity;

/* Load history from Flash (call once at startup) */
extern void Activity_Load(void);

/* Check if the date has changed and rotate the ring buffer if needed.
   Call every 100ms from main loop (in RTC_Update_Flag block).
   year/month/day: current RTC calendar values. */
extern void Activity_Tick(int year, int month, int day);

/* Manually save today's step count to Flash (KEY0).
 * Rate-limited: will not write Flash more than once per ACTIVITY_SAVE_THROTTLE_S
 * seconds to protect W25Q80 endurance (~10,000 erase cycles).
 * Returns 1 if Flash was actually written, 0 if the call was throttled.
 * rtc_now: current RTC_Count (seconds since epoch). */
extern int Activity_Save_Today(uint32 rtc_now);

/* Toggle view mode (KEY2) */
extern void Activity_Toggle_View(void);

/* Render the activity history screen to OLED */
extern void Render_ActivityHistory(void);

#endif
