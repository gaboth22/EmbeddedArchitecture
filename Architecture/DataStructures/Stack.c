#include <string.h>
#include "Stack.h"

bool Stack_Empty(Stack_t *instance)
{
    return instance->size == 0;
}

uint8_t Stack_Size(Stack_t *instance)
{
    return instance->size;
}

void Stack_Push(Stack_t *instance, void *data)
{
    memcpy(
        ((uint8_t *)instance->dataBuffer + (instance->index * instance->sizeOfType)),
        data,
        instance->sizeOfType);

    instance->size++;
    instance->index++;
}

void Stack_Pop(Stack_t *instance, void *data)
{
    instance->size--;
    instance->index--;

    memcpy(
        data,
        ((uint8_t *)instance->dataBuffer + (instance->index * instance->sizeOfType)),
        instance->sizeOfType);
}

void Stack_Init(Stack_t *instance, void *dataBuffer, size_t sizeOfDataType)
{
    instance->size = 0;
    instance->index = 0;
    instance->dataBuffer = dataBuffer;
    instance->sizeOfType = sizeOfDataType;
}
