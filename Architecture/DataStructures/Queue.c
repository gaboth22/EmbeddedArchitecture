#include <string.h>
#include "Queue.h"

bool Queue_Empty(Queue_t *instance)
{
    return instance->size == 0;
}

uint8_t Queue_Size(Queue_t *instance)
{
    return instance->size;
}

void Queue_Push(Queue_t *instance, void *data)
{
    if(instance->size < instance->maxSize)
    {
        memcpy(
            ((uint8_t *)instance->dataBuffer + (instance->pushIndex * instance->sizeOfType)),
            data,
            instance->sizeOfType);

        instance->size++;
        instance->pushIndex++;
        instance->pushIndex = instance->pushIndex % instance->maxSize;
    }
}

void Queue_Pop(Queue_t *instance, void *data)
{
    if(instance->size > 0)
    {
        memcpy(
            data,
            ((uint8_t *)instance->dataBuffer + (instance->popIndex * instance->sizeOfType)),
            instance->sizeOfType);

        instance->size--;

        if(instance->size == 0)
        {
            instance->popIndex = 0;
            instance->pushIndex = 0;
        }
        else
        {
            instance->popIndex++;
            instance->popIndex = instance->popIndex % instance->maxSize;
        }
    }
}

void Queue_Init(Queue_t *instance, void *dataBuffer, uint16_t maxSize, size_t sizeOfDataType)
{
    instance->size = 0;
    instance->popIndex = 0;
    instance->pushIndex = 0;
    instance->dataBuffer = dataBuffer;
    instance->sizeOfType = sizeOfDataType;
    instance->maxSize = maxSize;
}
