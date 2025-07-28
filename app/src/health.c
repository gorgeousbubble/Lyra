/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       health.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "ftm.h"
#include "gpio.h"
#include "health.h"
#include "max30102.h"
#include "max30102_algo.h"

LinkedList* IR_Buff = NULL;  //IR LED sensor data
LinkedList* RED_Buff = NULL; //Red LED sensor data

int32 SPO2 = 0;             //SPO2 value
int32 Heart_Rate = 0;       //Heart Rate value

uint32 RD_Duty = 0;

LinkedList* createLinkedList(int capacity) {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->capacity = capacity;
    return list;
}

void addNode(LinkedList* list, int data) {
    if (list->size >= list->capacity) {
        // If the list is full, remove the head node
        Node* temp = list->head;
        list->head = list->head->next;
        free(temp);
        list->size--;
    }
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (list->tail == NULL) {
        // If the list is empty, set head and tail to the new node
        list->head = newNode;
        list->tail = newNode;
    } else {
        // Link the new node to the end of the list
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->size++;
}

void freeLinkedList(LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    free(list);
}

/*
**heart rate and oxygen saturation sensor initialization
*/
void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Init(void)
{
  // Initialize the GPIO ports for MAX30102
  //MAX30102_PORT_INIT_RD;
  //MAX30102_PORT_INIT_IRD;
  //MAX30102_PORT_INIT_INT;

  // Initialize the linked lists for IR and RED data
  IR_Buff = createLinkedList(100);  // Example capacity
  RED_Buff = createLinkedList(100); // Example capacity

  // Initialize the MAX30102 sensor
  MAX30102_Init();
}

/*
**heart rate and oxygen saturation sensor cleanup
*/
void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Clean(void)
{
  // Free the linked lists
  freeLinkedList(IR_Buff);
  freeLinkedList(RED_Buff);
  
  // Reset the SPO2 and Heart Rate values
  SPO2 = 0;
  Heart_Rate = 0;
}

/*
**heart rate and oxygen saturation sensor calculation once per second
*/
void Health_Heart_Rate_And_Oxygen_Saturation_Sensor_Calculate(void)
{
  //uint32 min = 0x3FFFF;
  //uint32 max = 0;
  //uint32 pre_data = 0;
  //uint32 cur_data = 0;
  uint32 ir_data = 0;
  uint32 red_data = 0;
  int32 spo2 = 0;
  int8 spo2_valid = 0;
  int32 heart_rate = 0;
  int8 heart_rate_valid = 0;
  //int32 brightness = 0;
  //float temp = 0.0f;

  // Check if the linked lists are initialized
  //pre_data = RED_Buff->tail->data;

  // read RED & IR LED sensor data
  MAX30102_ReadFIFO(&red_data, &ir_data);

  // add the data to the linked lists
  addNode(IR_Buff, ir_data);
  addNode(RED_Buff, red_data);

  // find the minimum and maximum values in the RED data
  /*Node* current = RED_Buff->head;
  while (current != NULL) {
    if (current->data < min) {
      min = current->data;
    }
    if (current->data > max) {
      max = current->data;
    }
    current = current->next;
  }*/

  // calculate the current data as the difference from the minimum
  /*cur_data = RED_Buff->tail->data;
  if(cur_data > pre_data)//just to determine the brightness of LED according to the deviation of adjacent two AD data
  {
    temp = cur_data - pre_data;
    temp /= (max - min);
    temp *= MAX_BRIGHTNESS;
    brightness -= (int)temp;
    if(brightness < 0)
      brightness = 0;
  }
  else
  {
    temp = pre_data - cur_data;
    temp /= (max - min);
    temp *= MAX_BRIGHTNESS;
    brightness += (int)temp;
    if(brightness > MAX_BRIGHTNESS)
      brightness = MAX_BRIGHTNESS;
  }

  RD_Duty = (uint32)((1 - (float)brightness / 256) * 10000); // Convert brightness to duty cycle
  if(RD_Duty > 10000)
  {
    RD_Duty = 10000;
  }
  else if(RD_Duty < 0)
  {
    RD_Duty = 0;
  }
  FTM_PWM_Duty(FTM_FTM0, FTM_CH4, RD_Duty); // Update PWM duty cycle

  if(brightness < 120)
    MAX30102_SET_IRD_H; // Set IRD port high level
  else
    MAX30102_SET_IRD_L; // Set IRD port low level*/

  // calculate SPO2 using the formula
  maxim_heart_rate_and_oxygen_saturation(
      (uint32*)IR_Buff->head, 
      IR_Buff->size, 
      (uint32*)RED_Buff->head, 
      &spo2, 
      &spo2_valid, 
      &heart_rate, 
      &heart_rate_valid
  );
  // update the global SPO2 and Heart Rate values
  if(spo2_valid) {
    SPO2 = spo2;
  }
  if(heart_rate_valid) {
    Heart_Rate = heart_rate;
  }
}