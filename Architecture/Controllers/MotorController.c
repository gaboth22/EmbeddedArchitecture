#include "MotorController.h"
#include "GpioTable.h"
#include "Utils.h"

static void LeftEncoderCallback(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, MotorController_t *);

    if(instance->leftMotorPidOutput > 0)
    {
        instance->leftEncoderTick++;
    }
    else{
        instance->leftEncoderTick--;
    }
}

static void RightEncoderCallback(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, MotorController_t *);

    if(instance->rightMotorPidOutput > 0)
    {
        instance->rightEncoderTick++;
    }
    else{
        instance->rightEncoderTick--;
    }
}

static void LeftMotorPidOutputMotorDirectionCheck(MotorController_t *instance)
{
    // if positive, move left motors forward
    if(instance->leftMotorPidOutput > 0)
    {
        Pwm_ChangePortMap(instance->leftPwm, GpioPwm1_P2B4);
    }
    else{
        Pwm_ChangePortMap(instance->leftPwm, GpioPwm3_P2B6);
    }
}

static void RightMotorPidOutputMotorDirectionCheck(MotorController_t *instance)
{
    // if positive, move right motors forward
    if(instance->rightMotorPidOutput > 0)
    {
        Pwm_ChangePortMap(instance->rightPwm, GpioPwm4_P2B7);
    }
    else{
        Pwm_ChangePortMap(instance->rightPwm, GpioPwm2_P2B5);
    }
}

static void MotorController_Foward(MotorController_t *instance, uint8_t distanceToMove)
{
    if(instance->leftMotorForward)
    {
        bool switchDir = false;
        int64_t leftMotorPidOutput = PidController_Run(instance->leftPid, instance->leftEncoderTick, distanceToMove);

        if((instance->leftMotorPidOutput > 0 && leftMotorPidOutput < 0) || (instance->leftMotorPidOutput < 0 && leftMotorPidOutput > 0))
        {
            switchDir = true;
        }

        instance->leftMotorPidOutput = leftMotorPidOutput;

        if(switchDir)
        {
//            LeftMotorPidOutputMotorDirectionCheck(instance);
        }

        Pwm_SetDutyCycle(instance->leftPwm, (uint8_t)leftMotorPidOutput);
    }

    if(instance->rightMotorForward)
    {
        bool switchDir = false;

        int64_t rightMotorPidOutput = PidController_Run(instance->rightPid, instance->rightEncoderTick, distanceToMove);

        if((instance->rightMotorPidOutput > 0 && rightMotorPidOutput < 0) || (instance->rightMotorPidOutput < 0 && rightMotorPidOutput > 0))
        {
            switchDir = true;
        }

        instance->rightMotorPidOutput = rightMotorPidOutput;

        if(switchDir)
        {
//            RightMotorPidOutputMotorDirectionCheck(instance);
        }

        Pwm_SetDutyCycle(instance->rightPwm, (uint8_t)rightMotorPidOutput);
    }

    if(PidController_GoalAchieved(instance->leftPid))
    {
        instance->leftMotorForward = false;
        PidController_ClearState(instance->leftPid);
    }

    if(PidController_GoalAchieved(instance->rightPid))
    {
        instance->rightMotorForward = false;
        PidController_ClearState(instance->rightPid);
    }
}

/*
 * To move right: Port map right motor to go backwards so 2.7 and 2.5 need to switch
 */
static void MotorController_Right(MotorController_t *instance)
{
    if(instance->leftMotorForward)
    {
        uint64_t leftMotorPidReading = PidController_Run(instance->leftPid, instance->leftEncoderTick, 74);
        Pwm_SetDutyCycle(instance->leftPwm, (uint8_t)leftMotorPidReading);
    }

    if(instance->rightMotorForward)
    {
        uint64_t rightMotorPidReading = PidController_Run(instance->rightPid, instance->rightEncoderTick, 74);
        Pwm_SetDutyCycle(instance->rightPwm, (uint8_t)rightMotorPidReading);
    }

    if(PidController_GoalAchieved(instance->leftPid))
    {
        instance->leftMotorForward = false;
        PidController_ClearState(instance->leftPid);
    }

    if(PidController_GoalAchieved(instance->rightPid))
    {
        instance->rightMotorForward = false;
        PidController_ClearState(instance->rightPid);
    }
}

void MotorController_Run(MotorController_t *instance, MotorControllerDirection_t motorDirection, uint8_t distanceToMove)
{
    switch(motorDirection)
    {
        case MotorControllerDirection_Forward:
            MotorController_Foward(instance, distanceToMove);
            break;
        case MotorControllerDirection_Right:
            if(instance->rightMotorSwapped)
            {
                MotorController_Right(instance);
            }
            else{
                Pwm_ChangePortMap(instance->rightPwm, GpioPwm2_P2B5);
                instance->rightMotorSwapped = true;
            }
            break;
        default:
            break;
    }
}

void MotorController_Init(
    MotorController_t *instance,
    I_Event_t *leftEncoderEvent,
    I_Event_t *rightEncoderEvent,
    I_Pwm_t *leftPwm,
    I_Pwm_t *rightPwm,
    PidController_t *leftPid,
    PidController_t *rightPid)
{
    instance->rightMotorPidOutput = 0;
    instance->leftMotorPidOutput = 0;

    instance->rightMotorSwapped = false;

    instance->leftMotorForward = true;
    instance->rightMotorForward = true;

    instance->leftEncoderTick = 0;
    instance->rightEncoderTick = 0;

    instance->leftPwm = leftPwm;
    instance->rightPwm = rightPwm;

    instance->leftPid = leftPid;
    instance->rightPid = rightPid;

    EventSubscriber_Synchronous_Init(&instance->leftEncoderTickSubscriber, LeftEncoderCallback, instance);
    Event_Subscribe(leftEncoderEvent, &instance->leftEncoderTickSubscriber.interface);

    EventSubscriber_Synchronous_Init(&instance->rightEncoderTickSubscriber, RightEncoderCallback, instance);
    Event_Subscribe(rightEncoderEvent, &instance->rightEncoderTickSubscriber.interface);
}
