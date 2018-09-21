#include "MotorController.h"
#include "Utils.h"

static void LeftEncoderCallback(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, MotorController_t *);

    instance->leftEncoderTick++;
}

static void RightEncoderCallback(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, MotorController_t *);

    instance->rightEncoderTick++;
}

static void MotorController_Foward(MotorController_t *instance)
{
    if(instance->leftMotorForward)
    {
        uint64_t leftMotorPidReading = PidController_Run(&instance->leftPid, instance->leftEncoderTick, 74);
        Pwm_SetDutyCycle(&instance->leftPwm, (uint8_t)leftMotorPidReading);
    }
    if(instance->rightMotorForward)
    {
        uint64_t rightMotorPidReading = PidController_Run(&instance->rightPid, instance->rightEncoderTick, 74);
        Pwm_SetDutyCycle(&instance->rightPwm, (uint8_t)rightMotorPidReading);
    }
    if(PidController_GoalAchieved(&instance->leftPid))
    {
        instance->leftMotorForward = false;
    }

    if(PidController_GoalAchieved(&instance->rightPid))
    {
        instance->rightMotorForward = false;
    }
}

void MotorController_Run(MotorController_t *instance, uint8_t goal)
{
    switch(goal)
    {
        case Forward:
            MotorController_Foward(instance);
            break;
        default:
            break;
    }
}

void MotorController_Init(
    MotorController_t *instance,
    I_Event_t *leftEncoderEvent,
    I_Event_t *rightEncoderEvent,
    I_Pwm_t leftPwm,
    I_Pwm_t rightPwm,
    PidController_t leftPid,
    PidController_t rightPid)
{
    instance->leftMotorForward = true;
    instance->rightMotorForward = true;

    instance->leftEncoderTick = 0;
    instance->rightEncoderTick = 0;

    instance->leftPwm = leftPwm;
    instance->rightPwm = rightPwm;

    instance->leftPid = leftPid;
    instance->rightPid = rightPid;

    EventSubscriber_Synchronous_Init(&instance->leftEncoderTickSubscriber, LeftEncoderCallback, &instance->leftEncoderTick);
    Event_Subscribe(leftEncoderEvent, &instance->leftEncoderTickSubscriber.interface);

    EventSubscriber_Synchronous_Init(&instance->rightEncoderTickSubscriber, RightEncoderCallback, &instance->rightEncoderTick);
    Event_Subscribe(rightEncoderEvent, &instance->rightEncoderTickSubscriber.interface);
}
