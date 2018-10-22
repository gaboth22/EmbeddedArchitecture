#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include "EventSubscriber_Synchronous.h"
#include "I_Event.h"
#include "I_Pwm.h"
#include "PidController.h"
#include "types.h"
#include "TimerPeriodic.h"
#include "TimerModule.h"

enum ControllerDirection
{
    ControllerDirection_Forward = 0,
    ControllerDirection_Backwards,
    ControllerDirection_Left,
    ControllerDirection_Right,
};
typedef uint8_t ControllerDirection_t;

enum MotorDirection
{
  MotorDirection_Forward = 0,
  MotorDirection_Backwards
};
typedef uint8_t MotorDirection_t;

typedef struct
{
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
    bool busy;
    bool doSmoothStartup;
    bool stopSmoothnessTimer;
    int64_t smoothnessFactor;
    int64_t runningSmoothnessFactor;
    TimerPeriodic_t smoothStartupTimer;
} MotorController_t;

/*
 * Get instance of MotorController and
 * goal (forward, backwards, left, right)
 */
void MotorController_Run(MotorController_t *instance);

/*
 * Move Motor controller forward
 */
void MotorController_Forward(MotorController_t *instance, uint16_t numberOfTicksToMove);

/*
 * Move Motor controller right, turning rightMotorPid backwards on
 * right motor
 */
void MotorController_TurnRight(MotorController_t *instance, uint16_t numberOfTicksToMove);

/*
 * Move Motor controller left by turning left motor backwards
 */
void MotorController_TurnLeft(MotorController_t *instance, uint16_t numberOfTicksToMove);

/*
 * Returns true if the controller is moving the motors, false otherwise
 */
bool MotorController_Busy(MotorController_t *instance);

/*
 * Init Motor Controller
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
    int64_t smoothnessFactor);

#endif
