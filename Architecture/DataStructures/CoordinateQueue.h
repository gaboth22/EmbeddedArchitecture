#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"
#include "XYCoordinate.h"

typedef struct
{
    uint8_t size;
    uint8_t popIndex;
    uint8_t pushIndex;
    XYCoordinate_t data[255];
} CoordinateQueue_t;

/*
 * Check if the queue is empty
 */
bool CoordinateQueue_Empty(CoordinateQueue_t *instance);

/*
 * Get the size of the queue
 */
uint8_t CoordinateQueue_Size(CoordinateQueue_t *instance);

/*
 * Push an item into the queue
 */
void CoordinateQueue_Push(CoordinateQueue_t *instance, XYCoordinate_t coord);

/*
 * Pop an element from the queue
 */
XYCoordinate_t CoordinateQueue_Pop(CoordinateQueue_t *instance);

/*
 * Initialize the queue
 */
void CoordinateQueue_Init(CoordinateQueue_t *instance);

#endif
