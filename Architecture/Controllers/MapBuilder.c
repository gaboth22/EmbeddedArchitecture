#include "MapBuilder.h"

enum
{
    CurrentHeading_N = 0,
    CurrentHeading_S = 1,
    CurrentHeading_W = 2,
    CurrentHeading_E = 3,
    DistanceForOneCellCm = 5,

    DistanceToCountAWallInThatDirectionCm = 30
};

static XYCoordinate_t PopulateGridsAndUpdatePosition(
    MapBuilder_t *instance,
    DistanceInCm_t distanceMoved,
    uint8_t xpos,
    uint8_t ypos,
    uint8_t currentHeading,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visitedAreasGrid,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blockedAreasGrid)
{
    bool increaseX = false;
    bool decreaseX = false;
    bool increaseY = false;
    bool decreaseY = false;

    uint8_t numberOfGridCellsToOccupy = distanceMoved / DistanceForOneCellCm;

    switch(currentHeading)
    {
        case CurrentHeading_N:
            increaseY = true;
            break;

        case CurrentHeading_S:
            decreaseY = true;
            break;

        case CurrentHeading_W:
            decreaseX = true;
            break;

        case CurrentHeading_E:
            increaseX = true;
            break;
    }

    uint8_t i = 0;

    if(increaseX)
    {
        uint8_t newXPosEnd = xpos + numberOfGridCellsToOccupy;
        for(i = xpos; i <= newXPosEnd; i++)
        {
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                visitedAreasGrid,
                i,
                ypos);

            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) < DistanceToCountAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    blockedAreasGrid,
                    i,
                    ypos + 1);
            }

            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) < DistanceToCountAWallInThatDirectionCm)
           {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    blockedAreasGrid,
                    i,
                    ypos - 1);
           }
        }

        xpos += numberOfGridCellsToOccupy;
    }

    else if(decreaseX)
    {
        uint8_t newXPosEnd = xpos - numberOfGridCellsToOccupy;
        for(i = xpos; i >= newXPosEnd; i--)
        {
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                visitedAreasGrid,
                i,
                ypos);

            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) < DistanceToCountAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    blockedAreasGrid,
                    i,
                    ypos - 1);
            }

            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) < DistanceToCountAWallInThatDirectionCm)
           {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    blockedAreasGrid,
                    i,
                    ypos + 1);
           }
        }

        xpos -= numberOfGridCellsToOccupy;
    }

    else if(increaseY)
    {
        uint8_t newYPosEnd = ypos + numberOfGridCellsToOccupy;
        for(i = ypos; i <= newYPosEnd; i++)
        {
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                visitedAreasGrid,
                xpos,
                i);

            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) < DistanceToCountAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    blockedAreasGrid,
                    xpos - 1,
                    i);
            }

            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) < DistanceToCountAWallInThatDirectionCm)
           {
               GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                   blockedAreasGrid,
                   xpos + 1,
                   i);
           }
        }

        ypos += numberOfGridCellsToOccupy;
    }

    else if(decreaseY)
    {
        uint8_t newYPosEnd = ypos - numberOfGridCellsToOccupy;
        for(i = ypos; i >= newYPosEnd; i--)
        {
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                visitedAreasGrid,
                xpos,
                i);

            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) < DistanceToCountAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    blockedAreasGrid,
                    xpos + 1,
                    i);
            }

            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) < DistanceToCountAWallInThatDirectionCm)
           {
               GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                   blockedAreasGrid,
                   xpos - 1,
                   i);
           }
        }

        ypos -= numberOfGridCellsToOccupy;
    }

    XYCoordinate_t currentPos =
        { xpos, ypos };

    return currentPos;
}

XYCoordinate_t MapBuilder_Run(
    MapBuilder_t *instance,
    uint8_t currentHeading,
    uint8_t xpos,
    uint8_t ypos,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visitedAreasGrid,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blockedAreasGrid)
{
    XYCoordinate_t currentPos =
        { xpos, ypos };
    DistanceInCm_t distanceMovedSoFar =
        DistanceProviderCm_GetDistance(instance->distanceProvider);

    currentPos = PopulateGridsAndUpdatePosition(
        instance,
        distanceMovedSoFar,
        xpos,
        ypos,
        currentHeading,
        visitedAreasGrid,
        blockedAreasGrid);

    if(distanceMovedSoFar > 0)
    {
        DistanceProviderCm_ClearDistance(instance->distanceProvider);
    }

    return currentPos;
}

void MapBuilder_Init(
    MapBuilder_t *instance,
    DistanceProviderCm_t *distanceProvider,
    I_DistanceSensor_t *leftSensor,
    I_DistanceSensor_t *rightSensor)
{
    instance->distanceProvider = distanceProvider;
    instance->leftSensor = leftSensor;
    instance->rightSensor = rightSensor;
}
