/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       watch.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "oled_i2c.h"
#include "watch.h"

/*
 *  @brief      Watch_Render_Current_Time_6x8
 *  @param      uint8           x               x scope is 0~127
 *  @param      uint8           y               y scope is 0~7
 *  @param      uint8           ch[]            separator
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @param      int             second          second integer parameter
 *  @param      ClockSystem     clocksystem     clock system (12-hours/24-hours)
 *  @since      v1.0
 *  Sample usage:       Watch_Render_Current_Time_6x8(0,0,":",10,15,30,ClockSystem24);
*/
void Watch_Render_Current_Time_6x8(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second, ClockSystem clock_system) 
{
  int isAM = TRUE; // Assume AM by default
  switch (clock_system) 
  {
    case ClockSystem12:
      if (hour > 12) 
      {
        hour -= 12; // Convert to 12-hour format
        isAM = FALSE; // PM
      }
      else 
      {
        isAM = TRUE; // AM
      }
      Oled_I2C_Put_Time_6x8_12(x, y, ch, hour, minute, second, isAM);
      break;
    case ClockSystem24:
      // No conversion needed for 24-hour format
      Oled_I2C_Put_Time_6x8_24(x, y, ch, hour, minute, second);
      break;
    default:
      ASSERT(0);
      break; // Invalid clock system
  }
}

/*
 *  @brief      Watch_Render_Current_Time_Wo_Sec_6x8
 *  @param      uint8           x               x scope is 0~127
 *  @param      uint8           y               y scope is 0~7
 *  @param      uint8           ch[]            separator
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @param      int             second          second integer parameter
 *  @param      ClockSystem     clocksystem     clock system (12-hours/24-hours)
 *  @since      v1.0
 *  Sample usage:       Watch_Render_Current_Time_Wo_Sec_6x8(0,0,":",10,15,30,ClockSystem24);
*/
void Watch_Render_Current_Time_Wo_Sec_6x8(uint8 x,uint8 y,uint8 ch[],int hour,int minute,ClockSystem clock_system) 
{
  int isAM = TRUE; // Assume AM by default
  switch (clock_system) 
  {
    case ClockSystem12:
      if (hour > 12) 
      {
        hour -= 12; // Convert to 12-hour format
        isAM = FALSE; // PM
      }
      else 
      {
        isAM = TRUE; // AM
      }
      Oled_I2C_Put_Time_Wo_Sec_6x8_12(x, y, ch, hour, minute, isAM);
      break;
    case ClockSystem24:
      // No conversion needed for 24-hour format
      Oled_I2C_Put_Time_Wo_Sec_6x8_24(x, y, ch, hour, minute);
      break;
    default:
      ASSERT(0);
      break; // Invalid clock system
  }
}

/*
 *  @brief      Watch_Render_Current_Time_8x16
 *  @param      uint8           x               x scope is 0~127
 *  @param      uint8           y               y scope is 0~7
 *  @param      uint8           ch[]            separator
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @param      int             second          second integer parameter
 *  @param      ClockSystem     clocksystem     clock system (12-hours/24-hours)
 *  @since      v1.0
 *  Sample usage:       Watch_Render_Current_Time_8x16(0,0,":",10,15,30,ClockSystem24);
*/
void Watch_Render_Current_Time_8x16(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second, ClockSystem clock_system) 
{
  int isAM = TRUE; // Assume AM by default
  switch (clock_system) 
  {
    case ClockSystem12:
      if (hour > 12) 
      {
        hour -= 12; // Convert to 12-hour format
        isAM = FALSE; // PM
      }
      else 
      {
        isAM = TRUE; // AM
      }
      Oled_I2C_Put_Time_8x16_12(x, y, ch, hour, minute, second, isAM);
      break;
    case ClockSystem24:
      // No conversion needed for 24-hour format
      Oled_I2C_Put_Time_8x16_24(x, y, ch, hour, minute, second);
      break;
    default:
      ASSERT(0);
      break; // Invalid clock system
  }
}

/*
 *  @brief      Watch_Render_Current_Time_Wo_Sec_8x16
 *  @param      uint8           x               x scope is 0~127
 *  @param      uint8           y               y scope is 0~7
 *  @param      uint8           ch[]            separator
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @param      int             second          second integer parameter
 *  @param      ClockSystem     clocksystem     clock system (12-hours/24-hours)
 *  @since      v1.0
 *  Sample usage:       Watch_Render_Current_Time_Wo_Sec_8x16(0,0,":",10,15,30,ClockSystem24);
*/
void Watch_Render_Current_Time_Wo_Sec_8x16(uint8 x,uint8 y,uint8 ch[],int hour,int minute,ClockSystem clock_system) 
{
  int isAM = TRUE; // Assume AM by default
  switch (clock_system) 
  {
    case ClockSystem12:
      if (hour > 12) 
      {
        hour -= 12; // Convert to 12-hour format
        isAM = FALSE; // PM
      }
      else 
      {
        isAM = TRUE; // AM
      }
      Oled_I2C_Put_Time_Wo_Sec_8x16_12(x, y, ch, hour, minute, isAM);
      break;
    case ClockSystem24:
      // No conversion needed for 24-hour format
      Oled_I2C_Put_Time_Wo_Sec_8x16_24(x, y, ch, hour, minute);
      break;
    default:
      ASSERT(0);
      break; // Invalid clock system
  }
}

/*
 *  @brief      Watch_Render_Current_Time_12x24
 *  @param      uint8           x               x scope is 0~127
 *  @param      uint8           y               y scope is 0~7
 *  @param      uint8           ch[]            separator
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @param      int             second          second integer parameter
 *  @param      ClockSystem     clocksystem     clock system (12-hours/24-hours)
 *  @since      v1.0
 *  Sample usage:       Watch_Render_Current_Time_12x24(0,0,":",10,15,30,ClockSystem24);
*/
void Watch_Render_Current_Time_12x24(uint8 x,uint8 y,uint8 ch[],int hour,int minute,int second, ClockSystem clock_system) 
{
  int isAM = TRUE; // Assume AM by default
  switch (clock_system) 
  {
    case ClockSystem12:
      if (hour > 12) 
      {
        hour -= 12; // Convert to 12-hour format
        isAM = FALSE; // PM
      }
      else 
      {
        isAM = TRUE; // AM
      }
      Oled_I2C_Put_Time_12x24_12(x, y, ch, hour, minute, second, isAM);
      break;
    case ClockSystem24:
      // No conversion needed for 24-hour format
      Oled_I2C_Put_Time_12x24_24(x, y, ch, hour, minute, second);
      break;
    default:
      ASSERT(0);
      break; // Invalid clock system
  }
}

/*
 *  @brief      Watch_Render_Current_Time_Wo_Sec_12x24
 *  @param      uint8           x               x scope is 0~127
 *  @param      uint8           y               y scope is 0~7
 *  @param      uint8           ch[]            separator
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @param      int             second          second integer parameter
 *  @param      ClockSystem     clocksystem     clock system (12-hours/24-hours)
 *  @since      v1.0
 *  Sample usage:       Watch_Render_Current_Time_Wo_Sec_12x24(0,0,":",10,15,30,ClockSystem24);
*/
void Watch_Render_Current_Time_Wo_Sec_12x24(uint8 x,uint8 y,uint8 ch[],int hour,int minute, ClockSystem clock_system) 
{
  int isAM = TRUE; // Assume AM by default
  switch (clock_system) 
  {
    case ClockSystem12:
      if (hour > 12) 
      {
        hour -= 12; // Convert to 12-hour format
        isAM = FALSE; // PM
      }
      else 
      {
        isAM = TRUE; // AM
      }
      Oled_I2C_Put_Time_Wo_Sec_12x24_12(x, y, ch, hour, minute, isAM);
      break;
    case ClockSystem24:
      // No conversion needed for 24-hour format
      Oled_I2C_Put_Time_Wo_Sec_12x24_24(x, y, ch, hour, minute);
      break;
    default:
      ASSERT(0);
      break; // Invalid clock system
  }
}

/*
 *  @brief      Watch_Render_Current_Time_Clock
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @param      int             second          second integer parameter
 *  @since      v1.0
 *  Sample usage:       Watch_Render_Current_Time_Clock(10,15,30);
*/
void Watch_Render_Current_Time_Clock(int hour,int minute,int second)
{
    // Define a structure to hold coordinates
    struct Coordinate {
        uint8 x;
        uint8 y;
        struct Coordinate* next;
    };
    typedef struct Coordinate Coord;

    Coord* head = NULL;
    Coord* current = NULL;

    uint8 clock[64][16] = {0x00}; // 64 rows, 128 columns

    // draw clock circle
    for (int i = 0; i < 64; i ++) 
    {
        uint8 x1 = 0;
        uint8 x2 = 0;
        uint8 y = i;
        float32_t* temp = (float32_t*)malloc(sizeof(float32_t));
        // calculate the x1 and x2 coordinates
        arm_sqrt_f32(32 * 32 - (i - 32) * (i - 32), temp);
        x1 = 64 - (uint8)(*temp);
        x2 = 64 + (uint8)(*temp);
        free(temp);
        if (x1 == x2)
        {
            // create a new coordinate node
            Coord* newNode = (Coord*)malloc(sizeof(Coord));
            newNode->x = x1;
            newNode->y = y;
            newNode->next = NULL;

            // if head is NULL, set head to the new node
            if (head == NULL) {
                head = newNode;
                current = head;
            } else {
                current->next = newNode; // link the new node to the list
                current = newNode; // move current to the new node
            }
        } else {
            // create two coordinate nodes for x1 and x2
            Coord* newNode1 = (Coord*)malloc(sizeof(Coord));
            Coord* newNode2 = (Coord*)malloc(sizeof(Coord));
            newNode1->x = x1;
            newNode1->y = y;
            newNode2->x = x2;
            newNode2->y = y;
            newNode1->next = newNode2;
            newNode2->next = NULL;

            // if head is NULL, set head to the first node
            if (head == NULL) {
                head = newNode1;
                current = newNode2; // move current to the second node
            } else {
                current->next = newNode1; // link the first node to the list
                current = newNode2; // move current to the second node
            }
        }
    }
    // create the bottom coordinate
    Coord* bottomNode = (Coord*)malloc(sizeof(Coord));
    bottomNode->x = 64;
    bottomNode->y = 63;
    bottomNode->next = NULL;
    // link the bottom node to the head
    current->next = bottomNode;
    current = bottomNode; // move current to the bottom node
    // draw center point
    Coord* centerNode = (Coord*)malloc(sizeof(Coord));
    centerNode->x = 64; // center x coordinate
    centerNode->y = 32; // center y coordinate
    centerNode->next = NULL;
    // link the center node to the list
    if (head == NULL) {
        head = centerNode; // if head is NULL, set head to the center node
        current = head; // move current to the center node
    } else {
        current->next = centerNode; // link the center node to the list
        current = centerNode; // move current to the center node
    }
    // draw clock center
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // calculate the coordinates for the center point
            uint8 x = 64 + i - 1; // center x coordinate
            uint8 y = 32 + j - 1; // center y coordinate
            // create a new coordinate node for the center point
            Coord* centerPointNode = (Coord*)malloc(sizeof(Coord));
            centerPointNode->x = x;
            centerPointNode->y = y;
            centerPointNode->next = NULL;
            // link the center point node to the list
            if (head == NULL) {
                head = centerPointNode; // if head is NULL, set head to the center point node
                current = head; // move current to the center point node
            } else {
                current->next = centerPointNode; // link the center point node to the list
                current = centerPointNode; // move current to the center point node
            }
        }
    }
    // draw clock scale
    for (int i = 0; i < 12; i++)
    {
      for (int j = 0; j < 5; j++)
      {
        // calculate the angle for the number
        float32_t angle = i * 30.0; // 360 degrees / 12 numbers = 30 degrees per number
        // calculate the coordinates for the number
        uint8 x = (uint8)(64 + (31 - j) * arm_sin_f32(angle * (3.14159265358979323846 / 180.0)));
        uint8 y = (uint8)(32 - (31 - j) * arm_cos_f32(angle * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the number
        Coord* numberNode = (Coord*)malloc(sizeof(Coord));
        numberNode->x = x;
        numberNode->y = y;
        numberNode->next = NULL;
        // link the number node to the list
        if (head == NULL) {
            head = numberNode; // if head is NULL, set head to the number node
            current = head; // move current to the number node
        } else {
            current->next = numberNode; // link the number node to the list
            current = numberNode; // move current to the number node
        }
      }
    }
    // draw clock sub scale
    for (int i = 0; i < 60; i++)
    {
      if (i % 5 == 0) continue;
      for (int j = 0; j < 2; j++)
      {
        // calculate the angle for the number
        float32_t angle = i * 6.0; // 360 degrees / 60 numbers = 6 degrees per number
        // calculate the coordinates for the number
        uint8 x = (uint8)(64 + (31 - j) * arm_sin_f32(angle * (3.14159265358979323846 / 180.0)));
        uint8 y = (uint8)(32 - (31 - j) * arm_cos_f32(angle * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the number
        Coord* numberNode = (Coord*)malloc(sizeof(Coord));
        numberNode->x = x;
        numberNode->y = y;
        numberNode->next = NULL;
        // link the number node to the list
        if (head == NULL) {
            head = numberNode; // if head is NULL, set head to the number node
            current = head; // move current to the number node
        } else {
            current->next = numberNode; // link the number node to the list
            current = numberNode; // move current to the number node
        }
      }
    }
    // draw clock numbers
    for (int i = 0; i < 12; i++) 
    {
      // calculate the angle for the number
      float32_t angle = i * 30.0; // 360 degrees / 12 numbers = 30 degrees per number
      // calculate the coordinates for the number
      uint8 x = (uint8)(64 + 28 * arm_sin_f32(angle * (3.14159265358979323846 / 180.0)));
      uint8 y = (uint8)(32 - 28 * arm_cos_f32(angle * (3.14159265358979323846 / 180.0)));
      // draw the number (6x8)
      //char ch[12][5] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"};
      //char ch[12][3] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};
      char ch[12][5] = {"XII", "", "", "III", "", "", "VI", "", "", "IX", "", ""};
      for (int j = 0; ch[i][j] != '\0'; j++) 
      {
        uint8 c = ch[i][j] - 32; // convert character to ASCII value
        for (int k = 0; k < 6; k++) 
        {
          for (int l = 0; l < 8; l++) 
          {
            if (Oled_FontLib_6x8[c][k] & (0x01 << l)) 
            {
              // if the pixel is set, draw it
              // calculate the x and y coordinates for the character
              uint8 char_x = x + j * 6 + k;
              uint8 char_y = y + l;
              if (i == 0)
              {
                char_x -= 8;
                char_y += 3;
              } else if (i == 3)
              {
                char_x -= 20;
                char_y -= 3;
              } else if (i == 6)
              {
                char_x -= 6;
                char_y -= 10;
              } else if (i == 9)
              {
                char_x += 2;
                char_y -= 3;
              }
              // create a new coordinate node for the character pixel
              Coord* charNode = (Coord*)malloc(sizeof(Coord));
              charNode->x = char_x;
              charNode->y = char_y;
              charNode->next = NULL;
              // link the character node to the list
              if (head == NULL) {
                  head = charNode; // if head is NULL, set head to the character node
                  current = head; // move current to the character node
              } else {
                  current->next = charNode; // link the character node to the list
                  current = charNode; // move current to the character node
              }
            }
          }
        }
      }
    }
    // draw clock hands
    // calculate the angle for the hour hand
    float32_t hour_angle = (hour % 12 + minute / 60.0) * 30.0; // 360 degrees / 12 hours = 30 degrees per hour
    // calculate the angle for the minute hand
    float32_t minute_angle = (minute + second / 60.0) * 6.0; // 360 degrees / 60 minutes = 6 degrees per minute
    // calculate the angle for the second hand
    float32_t second_angle = second * 6.0; // 360 degrees / 60 seconds = 6 degrees per second
    // calculate the coordinates for the hour hand
    // the hour hand is 16 pixels long, starting from the center of the clock
    for (int i = 0; i < 16; i++) 
    {
        uint8 hour_x = (uint8)(64 + (i + 1) * arm_sin_f32(hour_angle * (3.14159265358979323846 / 180.0)));
        uint8 hour_y = (uint8)(32 - (i + 1) * arm_cos_f32(hour_angle * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the hour hand
        Coord* hourNode = (Coord*)malloc(sizeof(Coord));
        hourNode->x = hour_x;
        hourNode->y = hour_y;
        hourNode->next = NULL;
        // link the hour hand node to the list
        if (head == NULL) {
            head = hourNode; // if head is NULL, set head to the hour hand node
            current = head; // move current to the hour hand node
        } else {
            current->next = hourNode; // link the hour hand node to the list
            current = hourNode; // move current to the hour hand node
        }
    }
    // add an arrow for the hour hand
    uint8 bottomhour_x = 64 + (int)(16 * arm_sin_f32(hour_angle * (3.14159265358979323846 / 180.0)));
    uint8 bottomhour_y = 32 - (int)(16 * arm_cos_f32(hour_angle * (3.14159265358979323846 / 180.0)));
    // the arrow is 3 pixels long, starting from the end of the hour hand
    for (int i = 0; i < 3; i++) 
    {
        uint8 hour_arrow_x = (uint8)(bottomhour_x - (i + 1) * arm_sin_f32((hour_angle-30.0) * (3.14159265358979323846 / 180.0)));
        uint8 hour_arrow_y = (uint8)(bottomhour_y + (i + 1) * arm_cos_f32((hour_angle-30.0) * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the hour hand arrow
        Coord* hourArrowNode = (Coord*)malloc(sizeof(Coord));
        hourArrowNode->x = hour_arrow_x;
        hourArrowNode->y = hour_arrow_y;
        hourArrowNode->next = NULL;
        // link the hour hand arrow node to the list
        if (head == NULL) {
            head = hourArrowNode; // if head is NULL, set head to the hour hand arrow node
            current = head; // move current to the hour hand arrow node
        } else {
            current->next = hourArrowNode; // link the hour hand arrow node to the list
            current = hourArrowNode; // move current to the hour hand arrow node
        }
    }
    for (int i = 0; i < 3; i++) 
    {
        uint8 hour_arrow_x = (uint8)(bottomhour_x - (i + 1) * arm_sin_f32((hour_angle+30.0) * (3.14159265358979323846 / 180.0)));
        uint8 hour_arrow_y = (uint8)(bottomhour_y + (i + 1) * arm_cos_f32((hour_angle+30.0) * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the hour hand arrow
        Coord* hourArrowNode = (Coord*)malloc(sizeof(Coord));
        hourArrowNode->x = hour_arrow_x;
        hourArrowNode->y = hour_arrow_y;
        hourArrowNode->next = NULL;
        // link the hour hand arrow node to the list
        if (head == NULL) {
            head = hourArrowNode; // if head is NULL, set head to the hour hand arrow node
            current = head; // move current to the hour hand arrow node
        } else {
            current->next = hourArrowNode; // link the hour hand arrow node to the list
            current = hourArrowNode; // move current to the hour hand arrow node
        }
    }
    // calculate the coordinates for the minute hand
    // the minute hand is 20 pixels long, starting from the center of the clock
    for (int i = 0; i < 20; i++) 
    {
        uint8 minute_x = (uint8)(64 + (i + 1) * arm_sin_f32(minute_angle * (3.14159265358979323846 / 180.0)));
        uint8 minute_y = (uint8)(32 - (i + 1) * arm_cos_f32(minute_angle * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the minute hand
        Coord* minuteNode = (Coord*)malloc(sizeof(Coord));
        minuteNode->x = minute_x;
        minuteNode->y = minute_y;
        minuteNode->next = NULL;
        // link the minute hand node to the list
        if (head == NULL) {
            head = minuteNode; // if head is NULL, set head to the minute hand node
            current = head; // move current to the minute hand node
        } else {
            current->next = minuteNode; // link the minute hand node to the list
            current = minuteNode; // move current to the minute hand node
        }
    }
    // add an arrow for the minute hand
    uint8 bottomminute_x = 64 + (int)(20 * arm_sin_f32(minute_angle * (3.14159265358979323846 / 180.0)));
    uint8 bottomminute_y = 32 - (int)(20 * arm_cos_f32(minute_angle * (3.14159265358979323846 / 180.0)));
    // the arrow is 4 pixels long, starting from the end of the minute hand
    for (int i = 0; i < 4; i++)
    {
        uint8 minute_arrow_x = (uint8)(bottomminute_x - (i + 1) * arm_sin_f32((minute_angle-30.0) * (3.14159265358979323846 / 180.0)));
        uint8 minute_arrow_y = (uint8)(bottomminute_y + (i + 1) * arm_cos_f32((minute_angle-30.0) * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the minute hand arrow
        Coord* minuteArrowNode = (Coord*)malloc(sizeof(Coord));
        minuteArrowNode->x = minute_arrow_x;
        minuteArrowNode->y = minute_arrow_y;
        minuteArrowNode->next = NULL;
        // link the minute hand arrow node to the list
        if (head == NULL) {
            head = minuteArrowNode; // if head is NULL, set head to the minute hand arrow node
            current = head; // move current to the minute hand arrow node
        } else {
            current->next = minuteArrowNode; // link the minute hand arrow node to the list
            current = minuteArrowNode; // move current to the minute hand arrow node
        }
    }
    for (int i = 0; i < 4; i++) 
    {
        uint8 minute_arrow_x = (uint8)(bottomminute_x - (i + 1) * arm_sin_f32((minute_angle+30.0) * (3.14159265358979323846 / 180.0)));
        uint8 minute_arrow_y = (uint8)(bottomminute_y + (i + 1) * arm_cos_f32((minute_angle+30.0) * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the minute hand arrow
        Coord* minuteArrowNode = (Coord*)malloc(sizeof(Coord));
        minuteArrowNode->x = minute_arrow_x;
        minuteArrowNode->y = minute_arrow_y;
        minuteArrowNode->next = NULL;
        // link the minute hand arrow node to the list
        if (head == NULL) {
            head = minuteArrowNode; // if head is NULL, set head to the minute hand arrow node
            current = head; // move current to the minute hand arrow node
        } else {
            current->next = minuteArrowNode; // link the minute hand arrow node to the list
            current = minuteArrowNode; // move current to the minute hand arrow node
        }
    }
    // calculate the coordinates for the second hand
    // the second hand is 24 pixels long, starting from the center of the clock
    for (int i = 0; i < 24; i+=2) 
    {
        uint8 second_x = (uint8)(64 + (i + 1) * arm_sin_f32(second_angle * (3.14159265358979323846 / 180.0)));
        uint8 second_y = (uint8)(32 - (i + 1) * arm_cos_f32(second_angle * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the second hand
        Coord* secondNode = (Coord*)malloc(sizeof(Coord));
        secondNode->x = second_x;
        secondNode->y = second_y;
        secondNode->next = NULL;
        // link the second hand node to the list
        if (head == NULL) {
            head = secondNode; // if head is NULL, set head to the second hand node
            current = head; // move current to the second hand node
        } else {
            current->next = secondNode; // link the second hand node to the list
            current = secondNode; // move current to the second hand node
        }
    }
    // add an arrow for the second hand
    uint8 bottomsecond_x = 64 + (int)(24 * arm_sin_f32(second_angle * (3.14159265358979323846 / 180.0)));
    uint8 bottomsecond_y = 32 - (int)(24 * arm_cos_f32(second_angle * (3.14159265358979323846 / 180.0)));
    // the arrow is 5 pixels long, starting from the end of the second hand
    for (int i = 0; i < 5; i++) 
    {
        uint8 second_arrow_x = (uint8)(bottomsecond_x - (i + 1) * arm_sin_f32((second_angle-30.0) * (3.14159265358979323846 / 180.0)));
        uint8 second_arrow_y = (uint8)(bottomsecond_y + (i + 1) * arm_cos_f32((second_angle-30.0) * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the second hand arrow
        Coord* secondArrowNode = (Coord*)malloc(sizeof(Coord));
        secondArrowNode->x = second_arrow_x;
        secondArrowNode->y = second_arrow_y;
        secondArrowNode->next = NULL;
        // link the second hand arrow node to the list
        if (head == NULL) {
            head = secondArrowNode; // if head is NULL, set head to the second hand arrow node
            current = head; // move current to the second hand arrow node
        } else {
            current->next = secondArrowNode; // link the second hand arrow node to the list
            current = secondArrowNode; // move current to the second hand arrow node
        }
    }
    for (int i = 0; i < 5; i++) 
    {
        uint8 second_arrow_x = (uint8)(bottomsecond_x - (i + 1) * arm_sin_f32((second_angle+30.0) * (3.14159265358979323846 / 180.0)));
        uint8 second_arrow_y = (uint8)(bottomsecond_y + (i + 1) * arm_cos_f32((second_angle+30.0) * (3.14159265358979323846 / 180.0)));
        // create a new coordinate node for the second hand arrow
        Coord* secondArrowNode = (Coord*)malloc(sizeof(Coord));
        secondArrowNode->x = second_arrow_x;
        secondArrowNode->y = second_arrow_y;
        secondArrowNode->next = NULL;
        // link the second hand arrow node to the list
        if (head == NULL) {
            head = secondArrowNode; // if head is NULL, set head to the second hand arrow node
            current = head; // move current to the second hand arrow node
        } else {
            current->next = secondArrowNode; // link the second hand arrow node to the list
            current = secondArrowNode; // move current to the second hand arrow node
        }
    }
    // draw clock dial
    for (current = head; current != NULL; current = current->next) 
    {
        clock[current->y][current->x >> 3] |= (0x01 << (7 - (current->x & 0x07)));
    }
    // free the linked list
    while (head != NULL) 
    {
        Coord* temp = head;
        head = head->next;
        free(temp);
    }
    Oled_I2C_Draw_Picture_128x64((const uint8*)clock);
}