#include <CoordinateQueue.h>

bool CoordinateQueue_Empty(CoordinateQueue_t *instance)
{
    return instance->size == 0;
}

uint8_t CoordinateQueue_Size(CoordinateQueue_t *instance)
{
    return instance->size;
}

void CoordinateQueue_Push(CoordinateQueue_t *instance, XYCoordinate_t coord)
{
    instance->data[instance->pushIndex].x = coord.x;
    instance->data[instance->pushIndex].y = coord.y;
    instance->size++;
    instance->pushIndex++;
}

XYCoordinate_t CoordinateQueue_Pop(CoordinateQueue_t *instance)
{
    XYCoordinate_t toReturn = instance->data[instance->popIndex];
    instance->size--;

    if(instance->size == 0)
    {
        instance->popIndex = 0;
        instance->pushIndex = 0;
    }
    else
    {
        instance->popIndex++;
    }

    return toReturn;
}

/*
 * Initialize the queue
 */
void CoordinateQueue_Init(CoordinateQueue_t *instance)
{
    instance->size = 0;
    instance->popIndex = 0;
    instance->pushIndex = 0;
}
