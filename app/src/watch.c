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

#include "animation.h"
#include "conf.h"
#include "oled_i2c.h"
#include "watch.h"

const Coord LCM_Clock_Dial_coordinate[] = {{57,2},{58,2},{59,2},{60,2},{61,2},{62,2},{63,2},{64,2},{65,2},{66,2},{67,2},{68,2},{69,2},{70,2},{71,2},{53,3},{54,3},{55,3},{56,3},{71,3},{72,3},{73,3},{74,3},{51,4},{52,4},{75,4},{76,4},{49,5},{50,5},{77,5},{78,5},{47,6},{48,6},{79,6},{80,6},{46,7},{47,7},{57,7},{58,7},{59,7},{60,7},{61,7},{62,7},{63,7},{64,7},{65,7},{66,7},{67,7},{68,7},{69,7},{70,7},{80,7},{81,7},{45,8},{54,8},{55,8},{56,8},{71,8},{72,8},{73,8},{82,8},{44,9},{52,9},{53,9},{64,9},{74,9},{75,9},{83,9},{84,9},{42,10},{43,10},{50,10},{51,10},{64,10},{76,10},{77,10},{84,10},{85,10},{42,11},{49,11},{50,11},{52,11},{64,11},{77,11},{78,11},{85,11},{86,11},{41,12},{48,12},{52,12},{75,12},{79,12},{86,12},{40,13},{47,13},{53,13},{74,13},{80,13},{81,13},{87,13},{39,14},{46,14},{53,14},{74,14},{81,14},{82,14},{88,14},{38,15},{39,15},{45,15},{82,15},{88,15},{89,15},{38,16},{44,16},{83,16},{89,16},{37,17},{43,17},{84,17},{90,17},{37,18},{42,18},{43,18},{84,18},{85,18},{90,18},{36,19},{42,19},{85,19},{91,19},{36,20},{41,20},{44,20},{83,20},{84,20},{86,20},{91,20},{35,21},{41,21},{45,21},{46,21},{81,21},{82,21},{86,21},{92,21},{35,22},{40,22},{41,22},{87,22},{92,22},{35,23},{40,23},{87,23},{92,23},{35,24},{40,24},{87,24},{93,24},{34,25},{39,25},{88,25},{93,25},{34,26},{39,26},{88,26},{93,26},{34,27},{39,27},{88,27},{93,27},{34,28},{39,28},{88,28},{93,28},{34,29},{39,29},{88,29},{93,29},{34,30},{39,30},{62,30},{63,30},{64,30},{65,30},{88,30},{93,30},{34,31},{39,31},{62,31},{65,31},{88,31},{93,31},{34,32},{39,32},{41,32},{42,32},{43,32},{62,32},{65,32},{84,32},{85,32},{86,32},{88,32},{93,32},{34,33},{39,33},{62,33},{63,33},{64,33},{65,33},{88,33},{93,33},{34,34},{39,34},{88,34},{93,34},{34,35},{39,35},{88,35},{93,35},{34,36},{39,36},{88,36},{93,36},{34,37},{39,37},{88,37},{93,37},{34,38},{39,38},{88,38},{93,38},{34,39},{35,39},{40,39},{87,39},{93,39},{35,40},{40,40},{87,40},{92,40},{35,41},{40,41},{41,41},{87,41},{92,41},{35,42},{41,42},{45,42},{46,42},{81,42},{82,42},{86,42},{92,42},{36,43},{41,43},{44,43},{83,43},{84,43},{86,43},{91,43},{36,44},{42,44},{85,44},{91,44},{37,45},{42,45},{43,45},{84,45},{85,45},{90,45},{37,46},{43,46},{84,46},{90,46},{38,47},{44,47},{83,47},{89,47},{38,48},{39,48},{45,48},{82,48},{88,48},{89,48},{39,49},{46,49},{53,49},{74,49},{81,49},{82,49},{88,49},{40,50},{47,50},{53,50},{74,50},{80,50},{81,50},{87,50},{41,51},{48,51},{52,51},{75,51},{79,51},{86,51},{41,52},{42,52},{49,52},{50,52},{52,52},{64,52},{75,52},{77,52},{78,52},{85,52},{86,52},{42,53},{43,53},{50,53},{51,53},{64,53},{76,53},{77,53},{84,53},{85,53},{43,54},{44,54},{52,54},{53,54},{64,54},{74,54},{75,54},{83,54},{84,54},{45,55},{54,55},{55,55},{56,55},{71,55},{72,55},{73,55},{82,55},{46,56},{47,56},{57,56},{58,56},{59,56},{60,56},{61,56},{62,56},{63,56},{64,56},{65,56},{66,56},{67,56},{68,56},{69,56},{70,56},{80,56},{81,56},{47,57},{48,57},{79,57},{80,57},{49,58},{50,58},{77,58},{78,58},{51,59},{52,59},{75,59},{76,59},{53,60},{54,60},{55,60},{72,60},{73,60},{74,60},{56,61},{57,61},{58,61},{59,61},{60,61},{61,61},{62,61},{63,61},{64,61},{65,61},{66,61},{67,61},{68,61},{69,61},{70,61},{71,61},};

const int LCM_Clock_Dial_coordinate_length = ARRAY_LENGTH(LCM_Clock_Dial_coordinate);

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

/*
 *  @brief      Render_Clock_Current_Time
 *  @param      int             hour            hour integer parameter
 *  @param      int             minute          minute integer parameter
 *  @param      int             second          second integer parameter
 *  @since      v1.0
 *  Sample usage:       Render_Clock_Current_Time(10,15,30);
*/
void Render_Clock_Current_Time(const Coord *dial, const int dialLen, int hour,int minute,int second)
{
  uint8 clock[64][16] = {0x00}; // 64 rows, 128 columns
  CoordNode* head = NULL;
  CoordNode* current = NULL;

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
              char_x -= 9;
              char_y += 9;
            } else if (i == 3)
            {
              char_x -= 25;
              char_y -= 3;
            } else if (i == 6)
            {
              char_x -= 6;
              char_y -= 15;
            } else if (i == 9)
            {
              char_x += 7;
              char_y -= 3;
            }
            // create a new coordinate node for the character pixel
            CoordNode* charNode = (CoordNode*)malloc(sizeof(CoordNode));
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
      CoordNode* hourNode = (CoordNode*)malloc(sizeof(CoordNode));
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
      CoordNode* hourArrowNode = (CoordNode*)malloc(sizeof(CoordNode));
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
      CoordNode* hourArrowNode = (CoordNode*)malloc(sizeof(CoordNode));
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
      CoordNode* minuteNode = (CoordNode*)malloc(sizeof(CoordNode));
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
      CoordNode* minuteArrowNode = (CoordNode*)malloc(sizeof(CoordNode));
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
      CoordNode* minuteArrowNode = (CoordNode*)malloc(sizeof(CoordNode));
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
      CoordNode* secondNode = (CoordNode*)malloc(sizeof(CoordNode));
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
      CoordNode* secondArrowNode = (CoordNode*)malloc(sizeof(CoordNode));
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
      CoordNode* secondArrowNode = (CoordNode*)malloc(sizeof(CoordNode));
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
  // render the clock dial
  for (int i = 0; i < dialLen; i++)
  {
    const Coord *coord = &dial[i];
    clock[coord->y][coord->x >> 3] |= (0x01 << (7 - (coord->x & 0x07)));
  }
  // render the clock numbers and hands
  for (current = head; current != NULL; current = current->next) 
  {
    clock[current->y][current->x >> 3] |= (0x01 << (7 - (current->x & 0x07)));
  }
  // free the linked list
  while (head != NULL) 
  {
    CoordNode* temp = head;
    head = head->next;
    free(temp);
  }
  Oled_I2C_Draw_Picture_128x64((const uint8*)clock);
}