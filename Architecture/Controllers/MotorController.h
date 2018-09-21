#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include "EventSubscriber_Synchronous.h"
#include "I_Event.h"
#include "I_Pwm.h"
#include "PidController.h"

enum
{
    Forward,
    Backwards,
    Left,
    Right
};

typedef struct
{
    EventSubscriber_Synchronous_t leftEncoderTickSubscriber;
    EventSubscriber_Synchronous_t rightEncoderTickSubscriber;
    uint64_t leftEncoderTick;
    uint64_t rightEncoderTick;
    I_Pwm_t leftPwm;
    I_Pwm_t rightPwm;
    PidController_t leftPid;
    PidController_t rightPid;
    bool leftMotorForward;
    bool rightMotorForward;
} MotorController_t;

void MotorController_Run(MotorController_t *instance, uint8_t goal);

/*
 * Init Motorcontroller
 */
void MotorController_Init(
    MotorController_t *instance,
    I_Event_t *leftEncoderEvent,
    I_Event_t *rightEncoderEvent,
    I_Pwm_t leftPwm,
    I_Pwm_t rightPwm,
    PidController_t leftPid,
    PidController_t rightPid);

#endif
