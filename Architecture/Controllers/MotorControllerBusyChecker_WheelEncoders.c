#include "MotorControllerBusyChecker_WheelEncoders.h"
#include "utils.h"

enum
{
    TimeToConsiderEncoderAsNotBusyMs = 500
};

static void MarkRightAsNotBusy(void *context)
{
    RECAST(instance, context, MotorControllerBusyChecker_WheelEncoders_t *);
    instance->rightBusy = false;
}

static void StartTimerToMarkRightAsNotBusy(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, MotorControllerBusyChecker_WheelEncoders_t *);
    instance->rightBusy = true;

    TimerOneShot_Stop(&instance->markRightAsNotBusyTimer);
    TimerOneShot_Start(&instance->markRightAsNotBusyTimer);
}

static void MarkLeftAsNotBusy(void *context)
{
    RECAST(instance, context, MotorControllerBusyChecker_WheelEncoders_t *);
    instance->leftBusy = false;
}

static void StartTimerToMarkLeftAsNotBusy(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, MotorControllerBusyChecker_WheelEncoders_t *);
    instance->leftBusy = true;

    TimerOneShot_Stop(&instance->markLeftAsNotBusyTimer);
    TimerOneShot_Start(&instance->markLeftAsNotBusyTimer);
}

static bool Busy(I_MotorControllerBusyChecker_t *_instance)
{
    RECAST(instance, _instance, MotorControllerBusyChecker_WheelEncoders_t *);
    return instance->leftBusy && instance->rightBusy;
}

static const MotorControllerBusyCheckerApi_t api =
    { Busy };

void MotorControllerBusyChecker_WheelEncoders_Init(
    MotorControllerBusyChecker_WheelEncoders_t *instance,
    I_Event_t *onRightWheelEncoderTick,
    I_Event_t *onLeftWheelEncoderTick,
    TimerModule_t *timerModule)
{
    instance->interface.api = &api;
    instance->rightBusy = false;
    instance->leftBusy = false;

    TimerOneShot_Init(
        &instance->markLeftAsNotBusyTimer,
        timerModule,
        TimeToConsiderEncoderAsNotBusyMs,
        MarkLeftAsNotBusy,
        instance);

    TimerOneShot_Init(
        &instance->markRightAsNotBusyTimer,
        timerModule,
        TimeToConsiderEncoderAsNotBusyMs,
        MarkRightAsNotBusy,
        instance);

    EventSubscriber_Synchronous_Init(
        &instance->leftEncoderTickSub,
        StartTimerToMarkLeftAsNotBusy,
        instance);
    Event_Subscribe(onLeftWheelEncoderTick, &instance->leftEncoderTickSub.interface);

    EventSubscriber_Synchronous_Init(
        &instance->rightEncoderTickSub,
        StartTimerToMarkRightAsNotBusy,
        instance);
    Event_Subscribe(onRightWheelEncoderTick, &instance->rightEncoderTickSub.interface);
}
