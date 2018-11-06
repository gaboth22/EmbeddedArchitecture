#include <math.h>
#include "ScoutingController.h"

enum
{
    StartForwardMotion = 10,
    MonitorForwardDistanceToStop = 11,
    StartStopping = 12,
    WaitForFullStop = 13,
    GenerateHeadingTowardsUnscoutedNonBlockedCells = 14,
    JustTurnTowardsDirection = 15,
    FollowPath = 16,
    CompleteUTurn = 17,

    DistanceForOneCellCm = 5,
    DistanceToCountAWallInThatDirectionCm = 10,

    CurrentHeading_N = 0,
    CurrentHeading_S = 1,
    CurrentHeading_W = 2,
    CurrentHeading_E = 3,

    ForwardDistanceThresholdForTurnCm = 20,
    DistanceToKickForwardMotion = 50,

    TicksForATurn = 90,

    Left = 100,
    Right = 101
};

void ScoutingController_Start(ScoutingController_t *instance)
{
    instance->start = true;
}

static void MarkFrontWallAsBlocked(ScoutingController_t *instance, DistanceInCm_t distanceToFrontWall)
{
    uint8_t cellsToFrontWall = (uint8_t)ceil((float)distanceToFrontWall / DistanceForOneCellCm);

    switch(instance->currentHeading)
    {
        case CurrentHeading_N:
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                &instance->blockedAreasGrid, instance->xpos, instance->ypos + cellsToFrontWall);
            break;

        case CurrentHeading_S:
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                &instance->blockedAreasGrid, instance->xpos, instance->ypos - cellsToFrontWall);
            break;

        case CurrentHeading_W:
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                &instance->blockedAreasGrid, instance->xpos - cellsToFrontWall, instance->ypos);
            break;

        case CurrentHeading_E:
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                &instance->blockedAreasGrid, instance->xpos + cellsToFrontWall, instance->ypos);
            break;
    }
}

static void PopulateGridsAndUpdatePosition(ScoutingController_t *instance, DistanceInCm_t distanceMoved)
{
    bool increaseX = false;
    bool decreaseX = false;
    bool increaseY = false;
    bool decreaseY = false;

    uint8_t numberOfGridCellsToOccupy = distanceMoved / DistanceForOneCellCm;

    switch(instance->currentHeading)
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
        uint8_t newXPosEnd = instance->xpos + numberOfGridCellsToOccupy;
        for(i = instance->xpos; i <= newXPosEnd; i++)
        {
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                &instance->visitedAreasGrid,
                i,
                instance->ypos);

            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) < DistanceToCountAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    &instance->blockedAreasGrid,
                    i,
                    instance->ypos + 1);
            }

            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) < DistanceToCountAWallInThatDirectionCm)
           {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    &instance->blockedAreasGrid,
                    i,
                    instance->ypos - 1);
           }
        }

        instance->xpos += numberOfGridCellsToOccupy;
    }

    else if(decreaseX)
    {
        uint8_t newXPosEnd = instance->xpos - numberOfGridCellsToOccupy;
        for(i = instance->xpos; i >= newXPosEnd; i--)
        {
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                &instance->visitedAreasGrid,
                i,
                instance->ypos);

            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) < DistanceToCountAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    &instance->blockedAreasGrid,
                    i,
                    instance->ypos - 1);
            }

            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) < DistanceToCountAWallInThatDirectionCm)
           {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    &instance->blockedAreasGrid,
                    i,
                    instance->ypos + 1);
           }
        }

        instance->xpos -= numberOfGridCellsToOccupy;
    }

    else if(increaseY)
    {
        uint8_t newYPosEnd = instance->ypos + numberOfGridCellsToOccupy;
        for(i = instance->ypos; i <= newYPosEnd; i++)
        {
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                &instance->visitedAreasGrid,
                instance->xpos,
                i);

            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) < DistanceToCountAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    &instance->blockedAreasGrid,
                    instance->xpos - 1,
                    i);
            }

            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) < DistanceToCountAWallInThatDirectionCm)
           {
               GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                   &instance->blockedAreasGrid,
                   instance->xpos + 1,
                   i);
           }
        }

        instance->ypos += numberOfGridCellsToOccupy;
    }

    else if(decreaseY)
    {
        uint8_t newYPosEnd = instance->ypos - numberOfGridCellsToOccupy;
        for(i = instance->ypos; i >= newYPosEnd; i--)
        {
            GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                &instance->visitedAreasGrid,
                instance->xpos,
                i);

            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) < DistanceToCountAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                    &instance->blockedAreasGrid,
                    instance->xpos + 1,
                    i);
            }

            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) < DistanceToCountAWallInThatDirectionCm)
           {
               GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(
                   &instance->blockedAreasGrid,
                   instance->xpos - 1,
                   i);
           }
        }

        instance->ypos -= numberOfGridCellsToOccupy;
    }
}

static void TurnTowardsDirection(ScoutingController_t *instance, XYCoordinate_t heading)
{
    if(heading.x > instance->xpos)
    {
        switch(instance->currentHeading)
        {
            case CurrentHeading_N:
                MotorController_TurnRight(instance->motorController, TicksForATurn);
                instance->currentHeading = CurrentHeading_E;
                break;

            case CurrentHeading_W:
            case CurrentHeading_S:
                if(instance->currentHeading == CurrentHeading_W)
                {
                    instance->state = CompleteUTurn;
                }
                MotorController_TurnLeft(instance->motorController, TicksForATurn);
                instance->currentHeading = CurrentHeading_E;
                break;
        }
    }
    else if(heading.x < instance->xpos)
    {
        switch(instance->currentHeading)
        {
            case CurrentHeading_E:
            case CurrentHeading_N:
                if(instance->currentHeading == CurrentHeading_E)
                {
                  instance->state = CompleteUTurn;
                }
                MotorController_TurnLeft(instance->motorController, TicksForATurn);
                instance->currentHeading = CurrentHeading_W;
                break;

            case CurrentHeading_S:
                MotorController_TurnRight(instance->motorController, TicksForATurn);
                instance->currentHeading = CurrentHeading_W;
                break;
        }
    }
    else if(heading.y > instance->ypos)
    {
        switch(instance->currentHeading)
        {
            case CurrentHeading_E:
            case CurrentHeading_S:
                if(instance->currentHeading == CurrentHeading_S)
                {
                    instance->state = CompleteUTurn;
                }
                MotorController_TurnLeft(instance->motorController, TicksForATurn);
                instance->currentHeading = CurrentHeading_N;
                break;

            case CurrentHeading_W:
                MotorController_TurnRight(instance->motorController, TicksForATurn);
                instance->currentHeading = CurrentHeading_N;
                break;
        }
    }
    else if(heading.y < instance->ypos)
    {
        switch(instance->currentHeading)
        {
            case CurrentHeading_W:
            case CurrentHeading_N:
                if(instance->currentHeading == CurrentHeading_N)
                {
                    instance->state = CompleteUTurn;
                }
                MotorController_TurnLeft(instance->motorController, TicksForATurn);
                instance->currentHeading = CurrentHeading_S;
                break;

            case CurrentHeading_E:
                MotorController_TurnRight(instance->motorController, TicksForATurn);
                instance->currentHeading = CurrentHeading_S;
                break;
        }
    }
}

void ScoutingController_Run(ScoutingController_t *instance)
{
    if(instance->start)
    {
        switch(instance->state)
        {
            case StartForwardMotion:
                {
                    instance->state = MonitorForwardDistanceToStop;
                    PopulateGridsAndUpdatePosition(instance, 0);

                    MotorController_Forward(
                        instance->motorController,
                        DistanceProviderCm_GetTicksFromCm(
                            instance->distanceProvider,
                            DistanceToKickForwardMotion));
                }
                break;

            case MonitorForwardDistanceToStop:
                {
                    DistanceInCm_t distanceMovedSoFar =
                        DistanceProviderCm_GetDistance(instance->distanceProvider);

                    if(distanceMovedSoFar > 0)
                    {
                        instance->runningDistance += distanceMovedSoFar;
                        PopulateGridsAndUpdatePosition(instance, distanceMovedSoFar);
                        DistanceProviderCm_ClearDistance(instance->distanceProvider);
                    }

                    DistanceInCm_t forwardDistanceWithoutObstacle =
                            DistanceSensor_GetDistanceInCm(instance->frontSensor);

                    if(forwardDistanceWithoutObstacle <= ForwardDistanceThresholdForTurnCm)
                    {
                        instance->state = StartStopping;
                    }
                }
                break;

            case StartStopping:
                {
                    DistanceInCm_t forwardDistanceWithoutObstacle =
                        DistanceSensor_GetDistanceInCm(instance->frontSensor);
                    instance->state = WaitForFullStop;

                    MarkFrontWallAsBlocked(instance, forwardDistanceWithoutObstacle);

                    MotorController_ClearState(instance->motorController);

                    MotorController_Forward(
                        instance->motorController,
                        DistanceProviderCm_GetTicksFromCm(
                            instance->distanceProvider,
                            forwardDistanceWithoutObstacle - DistanceForOneCellCm));
                }
                break;

            case WaitForFullStop:
                {
                    DistanceInCm_t distanceMovedSoFar =
                        DistanceProviderCm_GetDistance(instance->distanceProvider);

                    if(distanceMovedSoFar > 0)
                    {
                        instance->runningDistance += distanceMovedSoFar;
                        PopulateGridsAndUpdatePosition(instance, distanceMovedSoFar);
                        DistanceProviderCm_ClearDistance(instance->distanceProvider);
                    }

                    if(!MotorController_Busy(instance->motorController))
                    {
                        instance->state = GenerateHeadingTowardsUnscoutedNonBlockedCells;
                    }
                }
                break;

            case GenerateHeadingTowardsUnscoutedNonBlockedCells:
                {
                    XYCoordinate_t currentPosition = { instance->xpos, instance->ypos };
                    XYCoordinate_t openSpot =
                        WayPointProvider_GetWayPoint(instance->waypointProvider, currentPosition);
                    instance->path =
                        PathFinder_GetPath(
                            instance->pathFinder,
                            &instance->blockedAreasGrid,
                            currentPosition,
                            openSpot);

                    if(Stack_Size(instance->path))
                    {
                        instance->state = JustTurnTowardsDirection;
                        TurnTowardsDirection(instance, openSpot);
                    }
                    else
                    {
                        instance->state = FollowPath;
                    }
                }
                break;

            case JustTurnTowardsDirection:
                {
                    if(!MotorController_Busy(instance->motorController))
                    {
                        instance->state = StartForwardMotion;
                    }
                }
                break;

            case CompleteUTurn:
                {
                    if(!MotorController_Busy(instance->motorController))
                    {
                        MotorController_TurnLeft(instance->motorController, TicksForATurn);
                        instance->state = JustTurnTowardsDirection;
                    }
                }
                break;

            case FollowPath:
                {

                }
                break;

            default:
                break;
        }
    }
}

void ScoutingController_Init(
    ScoutingController_t *instance,
    I_DistanceSensor_t *frontSensor,
    I_DistanceSensor_t *leftSensor,
    I_DistanceSensor_t *rightSensor,
    I_MotorController_t *motorController,
    DistanceProviderCm_t *distanceProvider,
    I_WayPointProvider_t *unscoutedWaypointProvider,
    I_PathFinder_t *pathFinder,
    uint8_t startX,
    uint8_t startY)
{
   GridMap_FirstQuadrant5cmCell3m2x3m2_Init(&instance->visitedAreasGrid);
   GridMap_FirstQuadrant5cmCell3m2x3m2_Init(&instance->blockedAreasGrid);
   instance->frontSensor = frontSensor;
   instance->leftSensor = leftSensor;
   instance->rightSensor = rightSensor;
   instance->motorController = motorController;
   instance->distanceProvider = distanceProvider;
   instance->waypointProvider = unscoutedWaypointProvider;
   instance->pathFinder = pathFinder;
   instance->state = StartForwardMotion;
   instance->start = false;
   instance->currentHeading = CurrentHeading_N;
   instance->runningDistance = 0;
   instance->xpos = startX;
   instance->ypos = startY;
}
