#include "ScoutingController.h"

enum
{
    StartForwardMotion = 0,
    MonitorForwardDistanceToStop = 1,
    StartStopping = 2,
    WaitForFullStop = 3,
    GenerateHeadingTowardsUnscoutedNonBlockedCells = 4,

    DistanceForOneCellCm = 5,
    DistanceToCountAWallInThatDirectionCm = 10,

    CurrentHeading_N = 0,
    CurrentHeading_S = 1,
    CurrentHeading_W = 2,
    CurrentHeading_E = 3,

    ForwardDistanceThresholdForTurnCm = 20,
    DistanceToKickForwardMotion = 50
};

void ScoutingController_Start(ScoutingController_t *instance)
{
    instance->start = true;
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
        for(i = instance->xpos + 1; i <= newXPosEnd; i++)
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
        for(i = instance->xpos - 1; i >= newXPosEnd; i--)
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
        for(i = instance->ypos + 1; i <= newYPosEnd; i++)
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
        for(i = instance->ypos - 1; i >= newYPosEnd; i--)
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
    DistanceProviderCm_t *distanceProvider)
{
   GridMap_FirstQuadrant5cmCell3m2x3m2_Init(&instance->visitedAreasGrid);
   GridMap_FirstQuadrant5cmCell3m2x3m2_Init(&instance->blockedAreasGrid);
   instance->frontSensor = frontSensor;
   instance->leftSensor = leftSensor;
   instance->rightSensor = rightSensor;
   instance->motorController = motorController;
   instance->distanceProvider = distanceProvider;
   instance->state = StartForwardMotion;
   instance->start = false;
   instance->currentHeading = CurrentHeading_N;
   instance->runningDistance = 0;
   instance->xpos = 0;
   instance->ypos = 0;
}
