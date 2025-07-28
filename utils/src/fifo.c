/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       fifo.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "fifo.h"

#include <stdio.h>
#include <stdlib.h>

QueueArray* createQueueArray(int capacity) {
  QueueArray* queue = (QueueArray*)malloc(sizeof(QueueArray));
  queue->capacity = capacity;
  queue->front = queue->size = 0;
  queue->rear = capacity - 1;
  queue->array = (int*)malloc(queue->capacity * sizeof(int));
  return queue;
}

int isFull(QueueArray* queue) {
  return (queue->size == queue->capacity);
}

int isEmpty(QueueArray* queue) {
  return (queue->size == 0);
}

void enqueue(QueueArray* queue, int item) {
  if (isFull(queue))
    return;
  queue->rear = (queue->rear + 1) % queue->capacity;
  queue->array[queue->rear] = item;
  queue->size = queue->size + 1;
}

int dequeue(QueueArray* queue) {
  if (isEmpty(queue))
    return -1;
  int item = queue->array[queue->front];
  queue->front = (queue->front + 1) % queue->capacity;
  queue->size = queue->size - 1;
  return item;
}
