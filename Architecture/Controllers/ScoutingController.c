#include <string.h>
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

    CurrentHeading_N = 0,
    CurrentHeading_S = 1,
    CurrentHeading_W = 2,
    CurrentHeading_E = 3,

    ForwardDistanceThresholdForTurnCm = 22,
    DistanceToKickForwardMotion = 150,

    TicksForATurn = 85,

    Left = 100,
    Right = 101,

    DistanceToMoveAfterDetectingAWallUpfrontCm = 5,

    DistanceToConsiderAWallInThatDirectionCm = 30
};

static void Uint8To2LengthString(uint8_t data, char *buffer)
{
    uint8_t byte1 = (data / 10);
    uint8_t byte0 = (data % 10);

    buffer[0] = (byte1 + '0');
    buffer[1] = (byte0 + '0');
}

void ScoutingController_Start(ScoutingController_t *instance)
{
    instance->start = true;
}

static void MarkFrontWallAsBlocked(ScoutingController_t *instance)
{
    uint8_t cellsToFrontWall = 1;

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

static void TurnTowardsDirection(ScoutingController_t *instance, XYCoordinate_t heading)
{
    DistanceProviderCm_DisableDistanceTracking(instance->distanceProvider);
    MotorController_ClearState(instance->motorController);

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

static void ClearCellThatIsActuallyOpen(ScoutingController_t *instance)
{
    switch(instance->currentHeading)
    {
        case CurrentHeading_N:
            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) > DistanceToConsiderAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(
                    &instance->blockedAreasGrid, instance->xpos - 1, instance->ypos);
            }

            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) > DistanceToConsiderAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(
                    &instance->blockedAreasGrid, instance->xpos + 1, instance->ypos);
            }

            break;

        case CurrentHeading_S:
            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) > DistanceToConsiderAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(
                    &instance->blockedAreasGrid, instance->xpos + 1, instance->ypos);
            }

            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) > DistanceToConsiderAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(
                    &instance->blockedAreasGrid, instance->xpos - 1, instance->ypos);
            }

            break;

        case CurrentHeading_W:
            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) > DistanceToConsiderAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(
                    &instance->blockedAreasGrid, instance->xpos, instance->ypos + 1);
            }

            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) > DistanceToConsiderAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(
                    &instance->blockedAreasGrid, instance->xpos, instance->ypos - 1);
            }

            break;

        case CurrentHeading_E:
            if(DistanceSensor_GetDistanceInCm(instance->rightSensor) > DistanceToConsiderAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(
                    &instance->blockedAreasGrid, instance->xpos, instance->ypos - 1);
            }

            if(DistanceSensor_GetDistanceInCm(instance->leftSensor) > DistanceToConsiderAWallInThatDirectionCm)
            {
                GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(
                    &instance->blockedAreasGrid, instance->xpos, instance->ypos + 1);
            }

            break;
    }
}

void ScoutingController_Run(ScoutingController_t *instance)
{
    if(instance->start)
    {
        XYCoordinate_t newPosition =
            MapBuilder_Run(
                instance->mapBuilder,
                instance->currentHeading,
                instance->xpos,
                instance->ypos,
                &instance->visitedAreasGrid,
                &instance->blockedAreasGrid);

        instance->xpos = newPosition.x;
        instance->ypos = newPosition.y;

        switch(instance->state)
        {
            case StartForwardMotion:
                {
                    ClearCellThatIsActuallyOpen(instance);

                    instance->state = MonitorForwardDistanceToStop;
                    DistanceProviderCm_EnableDistanceTracking(instance->distanceProvider);

                    MotorController_Forward(
                        instance->motorController,
                        DistanceProviderCm_GetTicksFromCm(
                            instance->distanceProvider,
                            DistanceToKickForwardMotion));
                }
                break;

            case MonitorForwardDistanceToStop:
                {
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
                            DistanceToMoveAfterDetectingAWallUpfrontCm));
                }
                break;

            case WaitForFullStop:
                {
                    if(!MotorController_Busy(instance->motorController))
                    {
                        MarkFrontWallAsBlocked(instance);
                        instance->state = GenerateHeadingTowardsUnscoutedNonBlockedCells;
                    }
                }
                break;

            case GenerateHeadingTowardsUnscoutedNonBlockedCells:
                {
                    XYCoordinate_t currentPosition = { instance->xpos, instance->ypos };
                    ClearCellThatIsActuallyOpen(instance);
                    XYCoordinate_t openSpot =
                        WayPointProvider_GetWayPoint(instance->waypointProvider, currentPosition);
//                    instance->path =
//                        PathFinder_GetPath(
//                            instance->pathFinder,
//                            &instance->blockedAreasGrid,
//                            currentPosition,
//                            openSpot);

//                    char msg[16];
//
//                    char at[] = "(  ,  )";
//                    char buff[2];
//
//                    Uint8To2LengthString(instance->xpos, &buff[0]);
//
//                    at[1] = buff[0];
//                    at[2] = buff[1];
//
//                    Uint8To2LengthString(instance->ypos, &buff[0]);
//
//                    at[4] = buff[0];
//                    at[5] = buff[1];
//
//                    char open[] = "(  ,  )";
//
//                    Uint8To2LengthString(openSpot.x, &buff[0]);
//
//                    open[1] = buff[0];
//                    open[2] = buff[1];
//
//                    Uint8To2LengthString(openSpot.y, &buff[0]);
//
//                    open[4] = buff[0];
//                    open[5] = buff[1];
//
//                    strcpy(msg, at);
//                    strcat(msg, open);

//                    LcdDisplayController_WriteString(instance->lcd, msg, 14);

//                    if(Stack_Size(instance->path) == 1)
//                    {
//                        instance->state = JustTurnTowardsDirection;
//                        TurnTowardsDirection(instance, openSpot);
//                    }
//                    else
//                    {
                        instance->state = JustTurnTowardsDirection;
                        TurnTowardsDirection(instance, openSpot);
//                    }
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
    uint8_t startY,
    LcdDisplayController_t *lcd,
    MapBuilder_t *mapBuilder)
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
   instance->lcd = lcd;
   instance->mapBuilder = mapBuilder;
}
