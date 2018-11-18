#ifndef MOTORCONTROLLERCHECKER_WHEELENCODERS_H
#define MOTORCONTROLLERCHECKER_WHEELENCODERS_H

#include "I_MotorControllerBusyChecker.h"
#include "I_Event.h"
#include "EventSubscriber_Synchronous.h"
#include "TimerPeriodic.h"

typedef struct
{
    I_MotorControllerBusyChecker_t interface;
    EventSubscriber_Synchronous_t leftEncoderTickSub;
    EventSubscriber_Synchronous_t rightEncoderTickSub;
    TimerPeriodic_t timerToMarkMotorsAsNotBusy;
    uint64_t lastTickCount;
    uint64_t tickCount;
    bool stopTimer;
    bool busy;
    bool running;
} MotorControllerBusyChecker_WheelEncoders_t;

/*
 * Initialize the module
 */
void MotorControllerBusyChecker_WheelEncoders_Init(
    MotorControllerBusyChecker_WheelEncoders_t *instance,
    I_Event_t *onRightWheelEncoderTick,
    I_Event_t *onLeftWheelEncoderTick,
    TimerModule_t *timerModule);

#endif
