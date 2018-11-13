#ifndef WAYPOINTPROVIDER_UNSCOUTED_H
#define WAYPOINTPROVIDER_UNSCOUTED_H

#include "Queue.h"
#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"
#include "I_WayPointProvider.h"

enum
{
    QueueBufferSizeBytes = 250
};

typedef struct
{
    I_WayPointProvider_t interface;
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visitedGrid;
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blockedGird;
    Queue_t queue;
    XYCoordinate_t queueBuffer[QueueBufferSizeBytes];
    uint8_t adjacents[8];
} WayPointProvider_Unscouted_t;

/*
 * Initialize the module
 * @param instance the object
 * @param visitedGrid Grid Map with visited nodes flagged
 * @param blockedGrid Gird Map with blocked nodes flagged
 */
void WayPointProvider_Unscouted_Init(
    WayPointProvider_Unscouted_t *instance,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visitedGrid,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blockedGird);

#endif
