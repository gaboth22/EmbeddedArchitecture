#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include "EventSubscriber_Synchronous.h"
#include "I_Event.h"
#include "I_Pwm.h"
#include "PidController.h"
#include "types.h"

enum MotorControllerDirection
{
    MotorControllerDirection_Forward,
    MotorControllerDirection_Backwards,
    MotorControllerDirection_Left,
    MotorControllerDirection_Right,
    MotorControllerDirection_Right_Run
};
typedef uint8_t MotorControllerDirection_t;

typedef struct
{
    EventSubscriber_Synchronous_t leftEncoderTickSubscriber;
    EventSubscriber_Synchronous_t rightEncoderTickSubscriber;
    uint64_t leftEncoderTick;
    uint64_t rightEncoderTick;
    int64_t leftMotorPidOutput;
    int64_t rightMotorPidOutput;
    I_Pwm_t *leftPwm;
    I_Pwm_t *rightPwm;
    PidController_t *leftPid;
    PidController_t *rightPid;
    bool leftMotorForward;
    bool rightMotorForward;
    bool rightMotorSwapped;
} MotorController_t;

/*
 * Get instance of MotorController and
 * goal (forward, backwards, left, right)
 */
void MotorController_Run(MotorController_t *instance, MotorControllerDirection_t motorDirection, uint8_t distanceToMove);

/*
 * Init Motorcontroller
 */
void MotorController_Init(
    MotorController_t *instance,
    I_Event_t *leftEncoderEvent,
    I_Event_t *rightEncoderEvent,
    I_Pwm_t *leftPwm,
    I_Pwm_t *rightPwm,
    PidController_t *leftPid,
    PidController_t *rightPid);

#endif
