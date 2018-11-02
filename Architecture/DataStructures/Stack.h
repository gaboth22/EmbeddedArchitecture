#ifndef STACK_H
#define STACK_H

#include "types.h"

typedef struct
{
    void *dataBuffer;
    uint8_t size;
    uint8_t index;
    size_t sizeOfType;
} Stack_t;

/*
 * Check whether stack is empty
 */
bool Stack_Empty(Stack_t *instance);

/*
 * Get the current size of the stack
 */
uint8_t Stack_Size(Stack_t *instance);

/*
 * Push one element into the stack
 */
void Stack_Push(Stack_t *instance, void *data);

/*
 * Pop an element from the stack, returned in data
 */
void Stack_Pop(Stack_t *instance, void *data);

/*
 * Initialize the stack
 */
void Stack_Init(Stack_t *instance, void *dataBuffer, size_t sizeOfDataType);

#endif
