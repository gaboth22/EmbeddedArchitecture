#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include "I_MotorController.h"
#include "EventSubscriber_Synchronous.h"
#include "I_Event.h"
#include "I_Pwm.h"
#include "I_MotorControllerBusyChecker.h"
#include "MotorDriveCorrectionController.h"
#include "PidController.h"
#include "types.h"
#include "TimerPeriodic.h"
#include "TimerModule.h"

enum ControllerDirection
{
    ControllerDirection_Forward = 0,
    ControllerDirection_Backward,
    ControllerDirection_Left,
    ControllerDirection_Right,
};
typedef uint8_t ControllerDirection_t;

enum MotorDirection
{
  MotorDirection_Forward = 0,
  MotorDirection_Backward
};
typedef uint8_t MotorDirection_t;

typedef struct
{
    I_MotorController_t interface;
    EventSubscriber_Synchronous_t leftEncoderTickSubscriber;
    EventSubscriber_Synchronous_t rightEncoderTickSubscriber;
    int64_t leftEncoderTick;
    int64_t rightEncoderTick;
    int64_t leftMotorPidOutput;
    int64_t rightMotorPidOutput;
    I_Pwm_t *pwmLeftFwd;
    I_Pwm_t *pwmLeftBwd;
    I_Pwm_t *pwmRightFwd;
    I_Pwm_t *pwmRightBwd;
    PidController_t *leftPid;
    PidController_t *rightPid;
    ControllerDirection_t controllerDirection;
    MotorDirection_t leftMotorDirection;
    MotorDirection_t rightMotorDirection;
    uint16_t leftMotorDistanceToMove;
    uint16_t rightMotorDistanceToMove;
    bool doSmoothStartup;
    bool stopSmoothnessTimer;
    int64_t smoothnessFactor;
    int64_t runningSmoothnessFactor;
    TimerPeriodic_t smoothStartupTimer;
    I_MotorControllerBusyChecker_t *busyChecker;
    MotorDriveCorrectionController_t *correctionController;
    bool busy;
} MotorController_t;

/*
 * Initialize motor controller
 */
void MotorController_Init(
    MotorController_t *instance,
    I_Event_t *leftEncoderEvent,
    I_Event_t *rightEncoderEvent,
    I_Pwm_t *pwmLeftFwd,
    I_Pwm_t *pwmLeftBwd,
    I_Pwm_t *pwmRightFwd,
    I_Pwm_t *pwmRightBwd,
    PidController_t *leftPid,
    PidController_t *rightPid,
    TimerModule_t *timerModule,
    int64_t smoothnessFactor,
    I_MotorControllerBusyChecker_t *busyChecker,
    MotorDriveCorrectionController_t *correctionController);

#endif
