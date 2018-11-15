#include "WayPointProvider_Unscouted.h"
#include "XYCoordinate.h"
#include "utils.h"

#include <stdio.h>

enum
{
    NonBlockedCell = 0,
    NonVisitedCell = 0,
    BlockedCell = 1,
    VisitedCell = 1
};

static void PushIndicesOfNonBlockedAdjacents(
    WayPointProvider_Unscouted_t *instance,
    XYCoordinate_t currentCoordinate)
{
    uint8_t i = 0;
    GridMap_FirstQuadrant5cmCell3m2x3m2_GetIndicesOfAdjacents(
        instance->visitedGrid,
        currentCoordinate.x,
        currentCoordinate.y,
        &instance->adjacents[0]);

    for(i = 1; i < 8; i += 2)
    {
        if(instance->adjacents[i-1] > 63 || instance->adjacents[i] > 63)
        {
            continue;
        }

        if(NonBlockedCell ==
           GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(
                   instance->blockedGird,
                   instance->adjacents[i-1],
                   instance->adjacents[i]))
        {
            XYCoordinate_t nonBlockedCell =
                { instance->adjacents[i-1], instance->adjacents[i] };
            Queue_Push(&instance->queue, &nonBlockedCell);
        }
    }
}

static XYCoordinate_t GetOpenCoordinate(
    WayPointProvider_Unscouted_t *instance,
    XYCoordinate_t currentPosition)
{
    /*
     * Do a standard BFS until the first non-blocked, non-visited cell is found
     */
    PushIndicesOfNonBlockedAdjacents(instance, currentPosition);
    XYCoordinate_t possiblyOpen;

    while(!Queue_Empty(&instance->queue))
    {
        Queue_Pop(&instance->queue, &possiblyOpen);

        if(NonVisitedCell ==
           GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(
               instance->visitedGrid,
               possiblyOpen.x,
               possiblyOpen.y))
        {
            Queue_Init(&instance->queue, &instance->queueBuffer[0], QueueBufferSizeBytes, sizeof(XYCoordinate_t));
            break;
        }

        PushIndicesOfNonBlockedAdjacents(instance, possiblyOpen);
    }

    return possiblyOpen;
}

static XYCoordinate_t GetWayPoint(
    I_WayPointProvider_t *_instance,
    XYCoordinate_t currentPosition)
{
    RECAST(instance, _instance, WayPointProvider_Unscouted_t *);
    return GetOpenCoordinate(instance, currentPosition);
}

static const WayPointProviderApi_t api =
    { GetWayPoint };

void WayPointProvider_Unscouted_Init(
    WayPointProvider_Unscouted_t *instance,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visitedGrid,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blockedGird)
{
    instance->interface.api = &api;
    instance->visitedGrid = visitedGrid;
    instance->blockedGird = blockedGird;
    Queue_Init(&instance->queue, &instance->queueBuffer[0], QueueBufferSizeBytes, sizeof(XYCoordinate_t));
}
