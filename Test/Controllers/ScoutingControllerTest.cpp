#include "MotorControllerMock.h"
#include "DistanceSensorTestDouble.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C"
{
#include "DistanceProviderCm.h"
#include "ScoutingController.h"
#include "Event_Synchronous.h"
}

enum
{
    TicksToMove1Cm = 1,
    TicksToMove5Cm = 5
};

enum
{
    StartForwardMotion = 0,
    MonitorForwardDistanceToStop = 1,
    StartStopping = 2,
    WaitForFullStopAndRegisterDataInGrid = 3,
    GenerateHeadingTowardsUnscoutedCells = 4,

    DistanceForOneCellCm = 5,

    CurrentHeading_N = 0,
    CurrentHeading_S = 1,
    CurrentHeading_W = 2,
    CurrentHeading_E = 3,

    ForwardDistanceThresholdForTurnCm = 20,
    DistanceToGoBeforeStoppingToTurnCm = 15,
    DistanceToKickForwardMotion = 50
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

    void setup()
    {
        Event_Synchronous_Init(&onEncoderTick);

        DistanceProviderCm_Init(
            &distanceProvider,
            &onEncoderTick.interface,
            TicksToMove5Cm,
            TicksToMove1Cm);

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
            &distanceProvider);
    }

    void GivenTheDistanceSensorReturnsADistance(DistanceSensorTestDouble_t *sensor, DistanceInCm_t toReturn)
    {
        DistanceSensorTestDouble_SetDistanceToReturn(sensor, toReturn);
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
