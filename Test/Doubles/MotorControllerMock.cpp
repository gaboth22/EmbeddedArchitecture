#include "MotorControllerMock.h"

static bool busy;

static void Run(I_MotorController_t *instance)
{
    IGNORE(instance);
    mock().actualCall("Run");
}

static void Forward(I_MotorController_t *instance, MotorEncoderTicks_t numberOfTicksToMove)
{
    IGNORE(instance);
    IGNORE(numberOfTicksToMove);
    mock().actualCall("Forward").withParameter("numberOfTicksToMove", numberOfTicksToMove);
}

static void TurnRight(I_MotorController_t *instance, MotorEncoderTicks_t numberOfTicksToMove)
{
    IGNORE(instance);
    IGNORE(numberOfTicksToMove);
    mock().actualCall("TurnRight").withParameter("numberOfTicksToMove", numberOfTicksToMove);
}

static void TurnLeft(I_MotorController_t *instance, MotorEncoderTicks_t numberOfTicksToMove)
{
    IGNORE(instance);
    IGNORE(numberOfTicksToMove);
    mock().actualCall("TurnLeft").withParameter("numberOfTicksToMove", numberOfTicksToMove);
}

static bool Busy(I_MotorController_t *instance)
{
    IGNORE(instance);
    mock().actualCall("Busy");

    return busy;
}

static void ClearState(I_MotorController_t *instance)
{
    IGNORE(instance);
    mock().actualCall("ClearState");
}

MotorControllerMock::MotorControllerMock()
{
    busy = false;
    const MotorControllerApi_t api =
        { Run, Forward, TurnRight, TurnLeft, Busy, ClearState };
    motorController.api = &api;
}

void MotorControllerMock::SetBusyState(bool newState)
{
    busy = newState;
}
