#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

typedef struct
{
    void *dataBuffer;
    uint8_t size;
    uint8_t popIndex;
    uint8_t pushIndex;
    size_t sizeOfType;
    uint16_t maxSize;
} Queue_t;

/*
 * Check whether queue is empty
 */
bool Queue_Empty(Queue_t *instance);

/*
 * Get the current size of the queue
 */
uint8_t Queue_Size(Queue_t *instance);

/*
 * Push one element into the queue
 */
void Queue_Push(Queue_t *instance, void *data);

/*
 * Pop an element from the queue, returned in data
 */
void Queue_Pop(Queue_t *instance, void *data);

/*
 * Initialize the Queue
 */
void Queue_Init(Queue_t *instance, void *dataBuffer, uint16_t maxSize, size_t sizeOfDataType);

#endif
