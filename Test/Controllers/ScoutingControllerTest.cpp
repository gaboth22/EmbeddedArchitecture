#include "MotorControllerMock.h"
#include "DistanceSensorTestDouble.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C"
{
#include "DistanceProviderCm.h"
#include "ScoutingController.h"
#include "Event_Synchronous.h"
#include "WayPointProvider_Unscouted.h"
#include "PathFinder_AStar.h"
#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"
}

enum
{
    TicksToMove1Cm = 1,
    TicksToMove5Cm = 5
};

enum
{
    StartForwardMotion = 10,
    MonitorForwardDistanceToStop = 11,
    StartStopping = 12,
    WaitForFullStopAndRegisterDataInGrid = 13,
    GenerateHeadingTowardsUnscoutedCells = 14,

    DistanceForOneCellCm = 5,

    CurrentHeading_N = 0,
    CurrentHeading_S = 1,
    CurrentHeading_W = 2,
    CurrentHeading_E = 3,

    ForwardDistanceThresholdForTurnCm = 20,
    DistanceToGoBeforeStoppingToTurnCm = 15,
    DistanceToKickForwardMotion = 50,
};

TEST_GROUP(ScoutingControllerTests)
{
    ScoutingController_t scoutingController;
    DistanceSensorTestDouble_t frontSensorTestDouble;
    DistanceSensorTestDouble_t leftSensorTestDouble;
    DistanceSensorTestDouble_t rightSensorTestDouble;
    MotorControllerMock motorControllerMock;
    DistanceProviderCm_t distanceProvider;
    Event_Synchronous_t onEncoderTick;
    WayPointProvider_Unscouted_t wayPointProvider;
    PathFinder_AStar_t pathFinder;

    void setup()
    {
        uint8_t startX = 32;
        uint8_t startY = 32;

        Event_Synchronous_Init(&onEncoderTick);

        DistanceProviderCm_Init(
            &distanceProvider,
            &onEncoderTick.interface,
            TicksToMove5Cm,
            TicksToMove1Cm);

        DistanceProviderCm_EnableDistanceTracking(&distanceProvider);

        WayPointProvider_Unscouted_Init(
            &wayPointProvider,
            &scoutingController.visitedAreasGrid,
            &scoutingController.blockedAreasGrid);

        PathFinder_AStar_Init(&pathFinder);

        DistanceSensorTestDouble_Init(&frontSensorTestDouble);
        DistanceSensorTestDouble_Init(&rightSensorTestDouble);
        DistanceSensorTestDouble_Init(&leftSensorTestDouble);

        motorControllerMock = MotorControllerMock();
        I_MotorController_t *motorController = &motorControllerMock.motorController;

        ScoutingController_Init(
            &scoutingController,
            &frontSensorTestDouble.interface,
            &leftSensorTestDouble.interface,
            &rightSensorTestDouble.interface,
            motorController,
            &distanceProvider,
            &wayPointProvider.interface,
            &pathFinder.interface,
            startX,
            startY);
    }

    void GivenTheDistanceSensorReturnsADistance(DistanceSensorTestDouble_t *sensor, DistanceInCm_t toReturn)
    {
        DistanceSensorTestDouble_SetDistanceToReturn(sensor, toReturn);
    }

    void GivenTheDistanceProviderSaysWeHaveMovedADistanceInIncrementsOf5Cm(DistanceInCm_t dist)
    {
        for(int i = 0; i < dist; i++)
        {
            Event_Publish(&onEncoderTick.interface, NULL);
        }
    }

    void TheseCoordinatesShouldBeMarkedAsVisited(XYCoordinate_t *coords, uint8_t numCoord)
    {
        for(int i = 0; i < numCoord; i++)
        {
            XYCoordinate_t c = *(coords + i);
            CHECK_EQUAL(1, GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(&scoutingController.visitedAreasGrid, c.x, c.y));
        }
    }

    void TheseCoordinatesShouldBeMarkedAsBlocked(XYCoordinate_t *coords, uint8_t numCoord)
    {
        for(int i = 0; i < numCoord; i++)
        {
            XYCoordinate_t c = *(coords + i);
            CHECK_EQUAL(1, GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(&scoutingController.blockedAreasGrid, c.x, c.y));
        }
    }
};

TEST(ScoutingControllerTests, ShouldNotTryToMoveTheMotorsUntilItsStarted)
{
    mock().expectNoCall("Forward");
    ScoutingController_Run(&scoutingController);
    ScoutingController_Start(&scoutingController);
    mock().expectOneCall("Forward").withParameter("numberOfTicksToMove", 50 * TicksToMove1Cm);
    ScoutingController_Run(&scoutingController);
}

TEST(ScoutingControllerTests, ShouldStartToMonitorForwardDistanceOnceItStartsForwardMotion)
{
    ScoutingController_Start(&scoutingController);
    mock().expectOneCall("Forward").withParameter("numberOfTicksToMove", 50 * TicksToMove1Cm);
    ScoutingController_Run(&scoutingController);
    CHECK_EQUAL(MonitorForwardDistanceToStop, scoutingController.state);
    GivenTheDistanceSensorReturnsADistance(&frontSensorTestDouble, 50);
    mock().expectNoCall("Forward");
    ScoutingController_Run(&scoutingController);
    GivenTheDistanceSensorReturnsADistance(&frontSensorTestDouble, 10);
    mock().expectNoCall("Forward");
    ScoutingController_Run(&scoutingController);
    mock().expectOneCall("ClearState");
    mock().expectOneCall("Forward").withParameter("numberOfTicksToMove", 5 * TicksToMove1Cm);
    ScoutingController_Run(&scoutingController);
}

TEST(ScoutingControllerTests, ShouldSwitchGeneratingHeadingToUnscoutedCellsOnceMotorsStop)
{
    ScoutingController_Start(&scoutingController);
    motorControllerMock.SetBusyState(true);
    scoutingController.state = WaitForFullStopAndRegisterDataInGrid;
    mock().expectOneCall("Busy");
    ScoutingController_Run(&scoutingController);
    CHECK_EQUAL(WaitForFullStopAndRegisterDataInGrid, scoutingController.state);
    motorControllerMock.SetBusyState(false);
    mock().expectOneCall("Busy");
    ScoutingController_Run(&scoutingController);
    CHECK_EQUAL(GenerateHeadingTowardsUnscoutedCells, scoutingController.state);
}

TEST(ScoutingControllerTests, ShouldBuildMapCorrectly)
{
    mock().expectNoCall("Forward");
    ScoutingController_Run(&scoutingController);

    ScoutingController_Start(&scoutingController);
    mock().expectOneCall("Forward").withParameter("numberOfTicksToMove", 50 * TicksToMove1Cm);

    DistanceSensorTestDouble_SetDistanceToReturn(&frontSensorTestDouble, 30);
    DistanceSensorTestDouble_SetDistanceToReturn(&leftSensorTestDouble, 5);
    DistanceSensorTestDouble_SetDistanceToReturn(&rightSensorTestDouble, 5);
    ScoutingController_Run(&scoutingController);

    GivenTheDistanceProviderSaysWeHaveMovedADistanceInIncrementsOf5Cm(10);
    ScoutingController_Run(&scoutingController);
    CHECK_EQUAL(10, scoutingController.runningDistance);

    XYCoordinate_t visitedCoords[] =
        { { 32, 32 }, { 32, 33 }, { 32, 34 } };
    TheseCoordinatesShouldBeMarkedAsVisited(&visitedCoords[0], 3);

    XYCoordinate_t blockedCoords[] =
        { { 31, 32 }, { 33, 32 }, { 31, 33 }, { 33, 33 }, { 31, 34 }, { 33, 34 } };

    TheseCoordinatesShouldBeMarkedAsBlocked(&blockedCoords[0], 6);
}

TEST(ScoutingControllerTests, ShouldMarkFrontWallAsBlockedWhenDistanceIsBelowThreshold)
{
    mock().expectNoCall("Forward");
    ScoutingController_Run(&scoutingController);

    ScoutingController_Start(&scoutingController);
    mock().expectOneCall("Forward").withParameter("numberOfTicksToMove", 50 * TicksToMove1Cm);

    DistanceSensorTestDouble_SetDistanceToReturn(&frontSensorTestDouble, 19);
    DistanceSensorTestDouble_SetDistanceToReturn(&leftSensorTestDouble, 5);
    DistanceSensorTestDouble_SetDistanceToReturn(&rightSensorTestDouble, 5);
    ScoutingController_Run(&scoutingController);

    GivenTheDistanceProviderSaysWeHaveMovedADistanceInIncrementsOf5Cm(10);
    ScoutingController_Run(&scoutingController);
    CHECK_EQUAL(StartStopping, scoutingController.state);

    uint8_t x =scoutingController.xpos;
    uint8_t y = scoutingController.ypos + 4;
    XYCoordinate_t blockedCoords[] = { { x, y } };

    mock().expectOneCall("ClearState");
    mock().expectOneCall("Forward").withParameter("numberOfTicksToMove", 14);
    ScoutingController_Run(&scoutingController);
    TheseCoordinatesShouldBeMarkedAsBlocked(&blockedCoords[0], 1);
}
