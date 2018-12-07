#include <string.h>
#include <math.h>
#include "ScoutingController.h"
#include "utils.h"

enum
{
    UturnToTheLeft = 1,
    UturnToTheRight = 2,
    StartForwardMotion = 10,
    MonitorForwardDistanceToStop = 11,
    StartStopping = 12,
    WaitForFullStop = 13,
    GenerateHeadingTowardsUnscoutedNonBlockedCells = 14,
    JustTurnTowardsDirection = 15,
    FollowPath = 16,
    CompleteUTurn = 17,
    DoNothing = 20,
    TurnAndLookAtImage = 25,
    CompensateForMessedUpUTurn = 26,
    FixMessedUpUTurn = 27,

    DistanceForOneCellCm = 5,

    CurrentHeading_N = 0,
    CurrentHeading_S = 1,
    CurrentHeading_W = 2,
    CurrentHeading_E = 3,

    ForwardDistanceThresholdForTurnCm = 15,
    DistanceToKickForwardMotion = 300,

    TicksForARightTurn = 83,
    TicksForALeftTurn = 81,

    DistanceToMoveBackwardsToDetectImageCm = 15,

    Left = 100,
    Right = 101,

    DistanceToMoveAfterDetectingAWallUpfrontCm = 5,

    DistanceToConsiderAWallInThatDirectionCm = 30,

    TimeToWaitAfterFindingImageMs = 11500,

    ThresholdToTurnTowardsImageCm = 14,
    TicksToTurnTowardsImage = 14,

    DistanceToConsiderAMessedUpUTurnCm = 40,

    TicksToBackUpAfterMessedUpUTurn = 40,

    DistanceToConsiderWallTooCloseAfterUTurnCm = 12,

    TicksToFixHeadingAfterMessedUpUTurn = 14,

    TickToDoBackAndForthRoutine = 60
};

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
                MotorController_TurnRight(instance->motorController, TicksForARightTurn);
                instance->currentHeading = CurrentHeading_E;
                break;

            case CurrentHeading_W:
            case CurrentHeading_S:
                if(instance->currentHeading == CurrentHeading_W)
                {
                    instance->state = CompleteUTurn;
                }
                MotorController_TurnLeft(instance->motorController, TicksForALeftTurn);
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
                MotorController_TurnLeft(instance->motorController, TicksForALeftTurn);
                instance->currentHeading = CurrentHeading_W;
                break;

            case CurrentHeading_S:
                MotorController_TurnRight(instance->motorController, TicksForARightTurn);
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
                MotorController_TurnLeft(instance->motorController, TicksForALeftTurn);
                instance->currentHeading = CurrentHeading_N;
                break;

            case CurrentHeading_W:
                MotorController_TurnRight(instance->motorController, TicksForARightTurn);
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
                MotorController_TurnLeft(instance->motorController, TicksForALeftTurn);
                instance->currentHeading = CurrentHeading_S;
                break;

            case CurrentHeading_E:
                MotorController_TurnRight(instance->motorController, TicksForARightTurn);
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

static void DoCompleteUTurn(ScoutingController_t *instance)
{
    MotorController_ClearState(instance->motorController);

    switch(instance->currentHeading)
    {
        case Heading_N:
            instance->currentHeading = Heading_S;
            break;
        case Heading_S:
            instance->currentHeading = Heading_N;
            break;
        case Heading_W:
            instance->currentHeading = Heading_E;
            break;
        case Heading_E:
            instance->currentHeading = Heading_W;
            break;
    }

    if(instance->uTurnDirection == UturnToTheLeft)
    {
        MotorController_TurnLeft(instance->motorController, TicksForALeftTurn);
    }
    else
    {
        MotorController_TurnRight(instance->motorController, TicksForARightTurn);
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
                    if(instance->uTurnCount == 0)
                    {
                        DistanceProviderCm_EnableDistanceTracking(instance->distanceProvider);
                    }
                    MotorController_ClearState(instance->motorController);
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
                    if(!MotorController_Busy(instance->motorController))
                    {
                        XYCoordinate_t currentPosition = { instance->xpos, instance->ypos };
                        ClearCellThatIsActuallyOpen(instance);
                        XYCoordinate_t openSpot =
                            WayPointProvider_GetWayPoint(
                                instance->waypointProvider,
                                currentPosition,
                                (Heading_t)instance->currentHeading);

                        if(openSpot.x == UINT8_MAX || openSpot.y == UINT8_MAX )
                        {
                            instance->uTurnCount++;

                            if(instance->uTurnCount ==  1)
                            {

                                MotorController_Backward(
                                    instance->motorController,
                                    DistanceProviderCm_GetTicksFromCm(
                                        instance->distanceProvider,
                                        DistanceToMoveBackwardsToDetectImageCm));

                                instance->state = TurnAndLookAtImage;
                                TimerOneShot_Start(&instance->timerToWaitAfterFindingImage);
                                break;
                            }

                            instance->state = CompleteUTurn;
                            MotorController_ClearState(instance->motorController);
                            DistanceProviderCm_DisableDistanceTracking(instance->distanceProvider);
                            DistanceProviderCm_ClearDistance(instance->distanceProvider);

                            DistanceInCm_t distToLeftWall =
                                DistanceSensor_GetDistanceInCm(instance->leftSensor);

                            DistanceInCm_t distToRightWall =
                                DistanceSensor_GetDistanceInCm(instance->rightSensor);

                            if(distToRightWall > distToLeftWall)
                            {
                                instance->uTurnDirection = UturnToTheLeft;
                                MotorController_TurnLeft(instance->motorController, TicksForALeftTurn);
                            }
                            else
                            {
                                instance->uTurnDirection = UturnToTheRight;
                                MotorController_TurnRight(instance->motorController, TicksForARightTurn);
                            }
                        }
                        else
                        {
                            instance->state = JustTurnTowardsDirection;
                            TurnTowardsDirection(instance, openSpot);
                        }
                    }
                }
                break;

            case JustTurnTowardsDirection:
                {
                    if(!MotorController_Busy(instance->motorController))
                    {
//                        DistanceInCm_t leftDistance =
//                                DistanceSensor_GetDistanceInCm(instance->leftSensor);
//                        DistanceInCm_t rightDistance =
//                                DistanceSensor_GetDistanceInCm(instance->rightSensor);
//                        if(leftDistance < 12)
//                        {
//                            MotorController_TurnRight(instance->motorController, 13);
//                            break;
//                        }
//                        else if(rightDistance < 12)
//                        {
//                            MotorController_TurnLeft(instance->motorController, 13);
//                            break;
//                        }

                        if(instance->uTurnCount == 3)
                        {
                            instance->state = DoNothing;
                            break;
                        }
                        instance->state = StartForwardMotion;
                    }
                }
                break;

            case CompleteUTurn:
                {
                    if(!MotorController_Busy(instance->motorController))
                    {
                        DoCompleteUTurn(instance);
                        instance->state = CompensateForMessedUpUTurn;
                    }
                }
                break;

            case CompensateForMessedUpUTurn:
                {
                    if(!MotorController_Busy(instance->motorController))
                    {
                        if(DistanceSensor_GetDistanceInCm(instance->frontSensor) < DistanceToConsiderAMessedUpUTurnCm)
                        {
                            instance->fixingRoutineFwd = true;
                            MotorController_Backward(instance->motorController, TicksToBackUpAfterMessedUpUTurn);
                        }

                        instance->state = FixMessedUpUTurn;
                    }
                }
                break;

            case FixMessedUpUTurn:
                {
                    if(!MotorController_Busy(instance->motorController) && instance->uTurnCount < 3)
                    {
                        DistanceInCm_t lDist =
                            DistanceSensor_GetDistanceInCm(instance->leftSensor);
                        DistanceInCm_t rDist =
                            DistanceSensor_GetDistanceInCm(instance->rightSensor);

                        if(lDist > 30)
                        {
                            MotorController_TurnLeft(instance->motorController, 25);
                            break;
                        }
                        else if(rDist > 30)
                        {
                            MotorController_TurnRight(instance->motorController, 25);
                            break;
                        }

                        if(abs(lDist - rDist) > 6)
                        {
                            if(instance->fixingRoutineFwd)
                            {
                                instance->fixingRoutineFwd = false;
                                MotorController_Forward(
                                    instance->motorController,
                                    TickToDoBackAndForthRoutine);
                            }
                            else
                            {
                                instance->fixingRoutineFwd = true;
                                MotorController_Backward(
                                    instance->motorController,
                                    TickToDoBackAndForthRoutine - 10);
                            }
                        }
                        else
                        {
//                            DistanceInCm_t frontDistance =
//                                DistanceSensor_GetDistanceInCm(instance->frontSensor);
//                            lDist = DistanceSensor_GetDistanceInCm(instance->leftSensor);
//                            rDist = DistanceSensor_GetDistanceInCm(instance->rightSensor);
//                            if(frontDistance < 25)
//                            {
//                                if(lDist < rDist)
//                                {
//                                    MotorController_TurnRight(instance->motorController, 10);
//                                }
//                                else
//                                {
//                                    MotorController_TurnLeft(instance->motorController, 10);
//                                }
//                                break;
//                            }
                            instance->state = JustTurnTowardsDirection;
                        }
                    }
                }
                break;

            case TurnAndLookAtImage:
                {
                    if(!MotorController_Busy(instance->motorController))
                    {
                        instance->state = DoNothing;

//                        if(DistanceSensor_GetDistanceInCm(instance->leftSensor) < ThresholdToTurnTowardsImageCm)
//                        {
//                            MotorController_TurnRight(instance->motorController, TicksToTurnTowardsImage);
//                        }
//
//                        if(DistanceSensor_GetDistanceInCm(instance->rightSensor) < ThresholdToTurnTowardsImageCm)
//                        {
//                            MotorController_TurnLeft(instance->motorController, TicksToTurnTowardsImage);
//                        }
                    }
                }
                break;

            default:
                break;
        }
    }
}

static void ChangeStateBackToGoingForward(void *context)
{
    RECAST(instance, context, ScoutingController_t *);
    instance->state = StartForwardMotion;
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
    MapBuilder_t *mapBuilder,
    TimerModule_t *timerModule,
    ImageRecognitionController_t *imgRecognitionController)
{

    instance->imgRecognitionController = imgRecognitionController;

    TimerOneShot_Init(
        &instance->timerToWaitAfterFindingImage,
        timerModule,
        TimeToWaitAfterFindingImageMs,
        ChangeStateBackToGoingForward,
        instance);
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
    instance->uTurnCount = 0;
}
