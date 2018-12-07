#include "MotorControllerBusyChecker_WheelEncoders.h"
#include "utils.h"

enum
{
    TimeToPollEncodersMs = 190
};

static void IncreaseTickCount(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, MotorControllerBusyChecker_WheelEncoders_t *);
    instance->tickCount++;
}

static void ResetTickCount(void *context)
{
    RECAST(instance, context, MotorControllerBusyChecker_WheelEncoders_t *);

    if(instance->tickCount > 0)
    {
        instance->tickCount = 0;
    }
    else
    {
        instance->stopTimer = true;
    }
}

static bool Busy(I_MotorControllerBusyChecker_t *_instance)
{
    RECAST(instance, _instance, MotorControllerBusyChecker_WheelEncoders_t *);
    return instance->busy;
}

static void StartChecking(I_MotorControllerBusyChecker_t *_instance)
{
    RECAST(instance, _instance, MotorControllerBusyChecker_WheelEncoders_t *);
    instance->busy = true;
    instance->running = true;
    instance->stopTimer = false;
    instance->tickCount = 1;
    TimerPeriodic_Command(&instance->timerToMarkMotorsAsNotBusy, TimerPeriodicCommand_Stop);
    TimerPeriodic_Start(&instance->timerToMarkMotorsAsNotBusy);
}

static void Run(I_MotorControllerBusyChecker_t *_instance)
{
    RECAST(instance, _instance, MotorControllerBusyChecker_WheelEncoders_t *);

    if(instance->stopTimer && instance->running)
    {
        instance->running = false;
        TimerPeriodic_Command(
            &instance->timerToMarkMotorsAsNotBusy,
            TimerPeriodicCommand_Stop);
        instance->busy = false;
    }
}

static const MotorControllerBusyCheckerApi_t api =
    { Busy, StartChecking, Run };

void MotorControllerBusyChecker_WheelEncoders_Init(
    MotorControllerBusyChecker_WheelEncoders_t *instance,
    I_Event_t *onRightWheelEncoderTick,
    I_Event_t *onLeftWheelEncoderTick,
    TimerModule_t *timerModule)
{
    instance->interface.api = &api;
    instance->running = false;
    instance->stopTimer = false;
    instance->busy = false;
    instance->tickCount = 1;

    TimerPeriodic_Init(
        &instance->timerToMarkMotorsAsNotBusy,
        timerModule,
        TimeToPollEncodersMs,
        ResetTickCount,
        instance);

    EventSubscriber_Synchronous_Init(
        &instance->leftEncoderTickSub,
        IncreaseTickCount,
        instance);
    Event_Subscribe(onLeftWheelEncoderTick, &instance->leftEncoderTickSub.interface);

    EventSubscriber_Synchronous_Init(
        &instance->rightEncoderTickSub,
        IncreaseTickCount,
        instance);
    Event_Subscribe(onRightWheelEncoderTick, &instance->rightEncoderTickSub.interface);
}
