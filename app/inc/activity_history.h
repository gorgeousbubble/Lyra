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
 * Flash storage layout (W25Q80, Page 2, 256 bytes)
 *
 *   Bytes 0..27  : 7 × ActivityDay records (4 bytes each)
 *     [0..3]  = Day 0 (oldest): { uint16 steps_hi, uint16 steps_lo }
 *                               packed as uint32 step count
 *     [4..7]  = Day 1 ...
 *     ...
 *     [24..27]= Day 6 (yesterday's completed day)
 *   Bytes 28..31 : Today's accumulated steps (current day, RAM only — NOT saved)
 *   Byte  32     : Day-of-week index of Day 6 (0=Mon..6=Sun), for display label
 *   Bytes 33..35 : Saved date of Day 6 (year-1970, month, day) for rollover check
 *   Bytes 36..255: Reserved (0xFF)
 *
 * On power-up:  Read all 7 days from Flash.
 * Each day:     Compare RTC date; if new day → shift ring buffer, write to Flash.
 * KEY0:         Manual save of today's partial count into Day 6 slot.
 * ----------------------------------------------------------------------- */

#define ACTIVITY_FLASH_PAGE       2       /* W25Q80 page number            */
#define ACTIVITY_DAYS             7       /* History depth (days)          */
#define ACTIVITY_RECORD_SIZE      36      /* Total bytes written to Flash  */

/* Step goal for progress bar */
#define ACTIVITY_GOAL_STEPS       10000

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
} ActivityHistoryState;

extern ActivityHistoryState Activity;

/* Load history from Flash (call once at startup) */
extern void Activity_Load(void);

/* Check if the date has changed and rotate the ring buffer if needed.
   Call every 100ms from main loop (in RTC_Update_Flag block).
   year/month/day: current RTC calendar values. */
extern void Activity_Tick(int year, int month, int day);

/* Manually save today's step count to Flash (KEY0) */
extern void Activity_Save_Today(void);

/* Toggle view mode (KEY2) */
extern void Activity_Toggle_View(void);

/* Render the activity history screen to OLED */
extern void Render_ActivityHistory(void);

#endif
