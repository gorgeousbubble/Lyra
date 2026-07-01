/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       animation.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "common.h"

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

// Define a structure to hold coordinates
struct CoordNode
{
    uint8 x;
    uint8 y;
    struct CoordNode *next;
};
typedef struct CoordNode CoordNode;

struct Coord
{
    uint8 x;
    uint8 y;
};
typedef struct Coord Coord;

struct CoordCache
{
    Coord *coord;  // Points to static storage, do NOT free
    int length;
};
typedef struct CoordCache CoordCache;

// Maximum pixels in a 128x64 frame
#define COORD_CACHE_MAX_SIZE 8192

/*
**variate declaration
*/
extern const Coord LCM_Watch_icon_coordinate[];
extern const Coord LCM_Stop_Watch_icon_coordinate[];
extern const Coord LCM_Alarm_Clock_icon_coordinate[];
extern const Coord LCM_World_Clock_icon_coordinate[];
extern const Coord LCM_Spirit_Level_icon_coordinate[];
extern const Coord LCM_Configure_Adjust_icon_coordinate[];

extern const int LCM_Watch_icon_coordinate_length;
extern const int LCM_Stop_Watch_icon_coordinate_length;
extern const int LCM_Alarm_Clock_icon_coordinate_length;
extern const int LCM_World_Clock_icon_coordinate_length;
extern const int LCM_Spirit_Level_icon_coordinate_length;
extern const int LCM_Configure_Adjust_icon_coordinate_length;

/*
**function declaration
*/
extern void Animation_Screen_Switch_Horizontal_Scroll_Array(const Coord *src, const int srcLen, const Coord *dst, const int dstLen, uint8 direction, uint8 speed, uint8 acc);
extern void Animation_Screen_Switch_Vertical_Scroll_Array(const Coord *src, const int srcLen, const Coord *dst, const int dstLen, uint8 direction, uint8 speed, uint8 acc);

#endif