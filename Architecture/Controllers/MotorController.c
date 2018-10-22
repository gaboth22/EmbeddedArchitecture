#include "MotorController.h"
#include "GpioTable.h"
#include "Utils.h"

enum
{
    PeriodToDoSmoothStartupCycleMs = 5
};

static void DoSmoothPwmStartup(void *context)
{
    RECAST(instance, context, MotorController_t *);
    instance->runningSmoothnessFactor -= 1;

    if(instance->runningSmoothnessFactor <= 0)
    {
        instance->stopSmoothnessTimer = true;
    }
}

static int64_t GetSmoothedOutPidOutput(MotorController_t *instance, int64_t pidOutput)
{
    if(instance->doSmoothStartup)
    {
        if(pidOutput > 0)
        {
            pidOutput -= instance->runningSmoothnessFactor;
            if(pidOutput < 0)
            {
                pidOutput = 0;
            }
        }
        else
        {
            pidOutput += instance->runningSmoothnessFactor;
            if(pidOutput > 0)
            {
                pidOutput = 0;
            }
        }
    }

    return pidOutput;
}

static void LeftEncoderCallback(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, MotorController_t *);

    if(instance->leftMotorDirection == MotorDirection_Forward)
    {
        instance->leftEncoderTick++;
    }
    else
    {
        instance->leftEncoderTick--;
    }
}

static void RightEncoderCallback(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, MotorController_t *);

    if(instance->rightMotorDirection == MotorDirection_Forward)
    {
        instance->rightEncoderTick++;
    }
    else
    {
        instance->rightEncoderTick--;
    }
}

static void MotorController_RunPidForward(MotorController_t *instance)
{
    int64_t leftMotorPidOutput = PidController_Run(instance->leftPid, instance->leftEncoderTick, instance->leftMotorDistanceToMove);
    leftMotorPidOutput = GetSmoothedOutPidOutput(instance, leftMotorPidOutput);

    if(leftMotorPidOutput < 0)
    {
        Pwm_SetDutyCycle(instance->pwmLeftFwd, 0);
        Pwm_SetDutyCycle(instance->pwmLeftBwd, (uint8_t) (-1 * leftMotorPidOutput));
        instance->leftMotorDirection = MotorDirection_Backwards;
    }
    else
    {
        Pwm_SetDutyCycle(instance->pwmLeftBwd, 0);
        Pwm_SetDutyCycle(instance->pwmLeftFwd, (uint8_t)leftMotorPidOutput);
        instance->leftMotorDirection = MotorDirection_Forward;
    }

    int64_t rightMotorPidOutput = PidController_Run(instance->rightPid, instance->rightEncoderTick, instance->rightMotorDistanceToMove);
    rightMotorPidOutput = GetSmoothedOutPidOutput(instance, rightMotorPidOutput);

    if(rightMotorPidOutput < 0)
    {
        Pwm_SetDutyCycle(instance->pwmRightFwd, 0);
        Pwm_SetDutyCycle(instance->pwmRightBwd, (uint8_t) (-1 * rightMotorPidOutput));
        instance->rightMotorDirection = MotorDirection_Backwards;
    }
    else
    {
        Pwm_SetDutyCycle(instance->pwmRightBwd, 0);
        Pwm_SetDutyCycle(instance->pwmRightFwd, (uint8_t)rightMotorPidOutput);
        instance->rightMotorDirection = MotorDirection_Forward;
    }

    if(leftMotorPidOutput == 0 && rightMotorPidOutput == 0)
    {
        instance->busy = false;
    }
}

static void MotorController_RunPidRight(MotorController_t *instance)
{
    int64_t leftMotorPidOutput = PidController_Run(instance->leftPid, instance->leftEncoderTick, instance->leftMotorDistanceToMove);
    leftMotorPidOutput = GetSmoothedOutPidOutput(instance, leftMotorPidOutput);

    if(leftMotorPidOutput < 0)
    {
        Pwm_SetDutyCycle(instance->pwmLeftFwd, 0);
        Pwm_SetDutyCycle(instance->pwmLeftBwd, (uint8_t) (-1 * leftMotorPidOutput));
        instance->leftMotorDirection = MotorDirection_Backwards;
    }
    else
    {
        Pwm_SetDutyCycle(instance->pwmLeftBwd, 0);
        Pwm_SetDutyCycle(instance->pwmLeftFwd, (uint8_t)leftMotorPidOutput);
        instance->leftMotorDirection = MotorDirection_Forward;
    }

    int64_t rightMotorPidOutput = PidController_Run(instance->rightPid, instance->rightEncoderTick, instance->rightMotorDistanceToMove);
    rightMotorPidOutput = GetSmoothedOutPidOutput(instance, rightMotorPidOutput);

    if(rightMotorPidOutput < 0)
    {
        Pwm_SetDutyCycle(instance->pwmRightBwd, 0);
        Pwm_SetDutyCycle(instance->pwmRightFwd, (uint8_t) (-1 * rightMotorPidOutput));
        instance->rightMotorDirection = MotorDirection_Backwards;
    }
    else
    {
        Pwm_SetDutyCycle(instance->pwmRightFwd, 0);
        Pwm_SetDutyCycle(instance->pwmRightBwd, (uint8_t)rightMotorPidOutput);
        instance->rightMotorDirection = MotorDirection_Forward; //moving forward to respect to other PWM
    }

    if(leftMotorPidOutput == 0 && rightMotorPidOutput == 0)
    {
        instance->busy = false;
    }
}

static void MotorController_RunPidLeft(MotorController_t *instance)
{
    int64_t leftMotorPidOutput = PidController_Run(instance->leftPid, instance->leftEncoderTick, instance->leftMotorDistanceToMove);
    leftMotorPidOutput = GetSmoothedOutPidOutput(instance, leftMotorPidOutput);

    if(leftMotorPidOutput < 0)
    {
        Pwm_SetDutyCycle(instance->pwmLeftBwd, 0);
        Pwm_SetDutyCycle(instance->pwmLeftFwd, (uint8_t) (-1 * leftMotorPidOutput));
        instance->leftMotorDirection = MotorDirection_Backwards; //move backwards with respect to PWM
    }
    else
    {
        Pwm_SetDutyCycle(instance->pwmLeftFwd, 0);
        Pwm_SetDutyCycle(instance->pwmLeftBwd, (uint8_t)leftMotorPidOutput);
        instance->leftMotorDirection = MotorDirection_Forward;
    }

    int64_t rightMotorPidOutput = PidController_Run(instance->rightPid, instance->rightEncoderTick, instance->rightMotorDistanceToMove);
    rightMotorPidOutput = GetSmoothedOutPidOutput(instance, rightMotorPidOutput);

    if(rightMotorPidOutput < 0)
    {
        Pwm_SetDutyCycle(instance->pwmRightFwd, 0);
        Pwm_SetDutyCycle(instance->pwmRightBwd, (uint8_t) (-1 * rightMotorPidOutput));
        instance->rightMotorDirection = MotorDirection_Backwards; //move forward with respect to PWM
    }
    else
    {
        Pwm_SetDutyCycle(instance->pwmRightBwd, 0);
        Pwm_SetDutyCycle(instance->pwmRightFwd, (uint8_t)rightMotorPidOutput);
        instance->rightMotorDirection = MotorDirection_Forward;
    }

    if(leftMotorPidOutput == 0 && rightMotorPidOutput == 0)
    {
        instance->busy = false;
    }
}

static void MotorController_SetupDirection(MotorController_t *instance, uint16_t distanceToMove)
{
    instance->leftEncoderTick = 0;
    instance->rightEncoderTick = 0;
    instance->leftMotorDistanceToMove = distanceToMove;
    instance->rightMotorDistanceToMove = (distanceToMove - 7);
}

void MotorController_Forward(MotorController_t *instance, uint16_t distanceToMove)
{
    instance->busy = true;
    instance->doSmoothStartup = true;
    instance->stopSmoothnessTimer = false;
    instance->runningSmoothnessFactor = instance->smoothnessFactor;
    MotorController_SetupDirection(instance, distanceToMove);
    instance->controllerDirection = ControllerDirection_Forward;

    TimerPeriodic_Start(&instance->smoothStartupTimer);
}

void MotorController_TurnRight(MotorController_t *instance, uint16_t distanceToMove)
{
    instance->busy = true;
    instance->doSmoothStartup = true;
    instance->stopSmoothnessTimer = false;
    instance->runningSmoothnessFactor = instance->smoothnessFactor;
    MotorController_SetupDirection(instance, distanceToMove);
    instance->controllerDirection = ControllerDirection_Right;

    TimerPeriodic_Start(&instance->smoothStartupTimer);
}

void MotorController_TurnLeft(MotorController_t *instance, uint16_t distanceToMove)
{
    instance->busy = true;
    instance->doSmoothStartup = true;
    instance->stopSmoothnessTimer = false;
    instance->runningSmoothnessFactor = instance->smoothnessFactor;
    MotorController_SetupDirection(instance, distanceToMove);
    instance->controllerDirection = ControllerDirection_Left;

    TimerPeriodic_Start(&instance->smoothStartupTimer);
}

void MotorController_Run(MotorController_t *instance)
{
    if(instance->stopSmoothnessTimer)
    {
        instance->doSmoothStartup = false;
        TimerPeriodic_Command(&instance->smoothStartupTimer, TimerPeriodicCommand_Stop);
    }

    switch(instance->controllerDirection)
    {
        case ControllerDirection_Forward:
            MotorController_RunPidForward(instance);
            break;
        case ControllerDirection_Right:
            MotorController_RunPidRight(instance);
            break;
        case ControllerDirection_Left:
            MotorController_RunPidLeft(instance);
            break;
        default:
            break;
    }
}

bool MotorController_Busy(MotorController_t *instance)
{
    return instance->busy;
}

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
    int64_t smoothnessFactor)
{
    instance->busy = false;

    instance->leftMotorDirection = MotorDirection_Forward;
    instance->rightMotorDirection = MotorDirection_Forward;

    instance->rightMotorPidOutput = 0;
    instance->leftMotorPidOutput = 0;

    instance->leftEncoderTick = 0;
    instance->rightEncoderTick = 0;

    instance->pwmLeftFwd = pwmLeftFwd;
    instance->pwmLeftBwd = pwmLeftBwd;
    instance->pwmRightFwd = pwmRightFwd;
    instance->pwmRightBwd = pwmRightBwd;

    instance->leftPid = leftPid;
    instance->rightPid = rightPid;

    instance->doSmoothStartup = false;
    instance->smoothnessFactor = smoothnessFactor;
    instance->stopSmoothnessTimer = false;

    TimerPeriodic_Init(
        &instance->smoothStartupTimer,
        timerModule,
        PeriodToDoSmoothStartupCycleMs,
        DoSmoothPwmStartup,
        instance);

    EventSubscriber_Synchronous_Init(&instance->leftEncoderTickSubscriber, LeftEncoderCallback, instance);
    Event_Subscribe(leftEncoderEvent, &instance->leftEncoderTickSubscriber.interface);

    EventSubscriber_Synchronous_Init(&instance->rightEncoderTickSubscriber, RightEncoderCallback, instance);
    Event_Subscribe(rightEncoderEvent, &instance->rightEncoderTickSubscriber.interface);
}
