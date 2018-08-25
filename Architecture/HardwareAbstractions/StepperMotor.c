#include <StepperMotor.h>

#include "../Timer/TimerOneShot.h"
#include "../Timer/TimerPeriodic.h"
#include "GpioTable.h"
#include "utils.h"

enum
{
    Pwm1 = 0,
    Pwm2 = 1,
    Pwm3 = 2,
    Pwm4 = 3,
    Pwm5 = 4,
    Pwm6 = 5,
    Pwm7 = 6,
    Pwm8 = 7,
    MinimumPeriodForStepSubCycleMs = 1,
    PeriodToPublishStepsDoneEventAfterMs = 200
};

static void DoStepSubCycle(void *context)
{
    RECAST(instance, context, StepperMotor_t *);

    if(instance->currentStepDirection == StepDirection_Forward)
    {
        switch(instance->pwmIndex)
        {
            case Pwm1:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm1Channel, GpioState_Low);
                instance->pwmIndex = Pwm2;
                break;
            case Pwm2:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm3Channel, GpioState_High);
                instance->pwmIndex = Pwm3;
                break;
            case Pwm3:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm4Channel, GpioState_Low);
                instance->pwmIndex = Pwm4;
                break;
            case Pwm4:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm2Channel, GpioState_High);
                instance->pwmIndex = Pwm5;
                break;
            case Pwm5:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm3Channel, GpioState_Low);
                instance->pwmIndex = Pwm6;
                break;
            case Pwm6:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm1Channel, GpioState_High);
                instance->pwmIndex = Pwm7;
                break;
            case Pwm7:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm2Channel, GpioState_Low);
                instance->pwmIndex = Pwm8;
                break;
            case Pwm8:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm4Channel, GpioState_High);
                instance->pwmIndex = Pwm1;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(instance->pwmIndex)
        {
            case Pwm1:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm4Channel, GpioState_Low);
                instance->pwmIndex = Pwm2;
                break;
            case Pwm2:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm2Channel, GpioState_High);
                instance->pwmIndex = Pwm3;
                break;
            case Pwm3:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm1Channel, GpioState_Low);
                instance->pwmIndex = Pwm4;
                break;
            case Pwm4:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm3Channel, GpioState_High);
                instance->pwmIndex = Pwm5;
                break;
            case Pwm5:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm2Channel, GpioState_Low);
                instance->pwmIndex = Pwm6;
                break;
            case Pwm6:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm4Channel, GpioState_High);
                instance->pwmIndex = Pwm7;
                break;
            case Pwm7:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm3Channel, GpioState_Low);
                instance->pwmIndex = Pwm8;
                break;
            case Pwm8:
                GpioGroup_SetState(instance->gpioGroup, instance->config->pwm1Channel, GpioState_High);
                instance->pwmIndex = Pwm1;
                break;
            default:
                break;
        }
    }
}

static void TurnAllPwmsOff(StepperMotor_t *instance)
{
    GpioGroup_SetState(instance->gpioGroup, instance->config->pwm1Channel, GpioState_Low);
    GpioGroup_SetState(instance->gpioGroup, instance->config->pwm2Channel, GpioState_Low);
    GpioGroup_SetState(instance->gpioGroup, instance->config->pwm3Channel, GpioState_Low);
    GpioGroup_SetState(instance->gpioGroup, instance->config->pwm4Channel, GpioState_Low);
}

static void PublishStepsDoneEvent(void *context)
{
    RECAST(instance, context, StepperMotor_t *);

    Event_Publish(&instance->onStepsDoneEvent.interface, NULL);
    instance->busy = false;
}

static void StopSteps(void *context)
{
    RECAST(instance, context, StepperMotor_t *);

    TimerPeriodic_Command(&instance->moveStepTimer, TimerPeriodicCommand_Stop);
    TimerOneShot_Start(&instance->publishStepsDoneEventTimer);
}

static void DoStep(I_StepperMotor_t *_instance, StepDirection_t direction, uint16_t numberOfSteps)
{
    RECAST(instance, _instance, StepperMotor_t *);

    if(!instance->busy)
    {
        instance->busy = true;
        instance->currentStepDirection = direction;
        TimerOneShot_UpdateExpiryTicks(&instance->stopStepsTimer, numberOfSteps * (uint16_t)MinimumPeriodForStepSubCycleMs);
        TimerPeriodic_Start(&instance->moveStepTimer);
        TimerOneShot_Start(&instance->stopStepsTimer);
    }
}

static void Stop(I_StepperMotor_t *_instance)
{
    RECAST(instance, _instance, StepperMotor_t *);

    TurnAllPwmsOff(instance);
    TimerPeriodic_Command(&instance->moveStepTimer, TimerPeriodicCommand_Stop);
    TimerOneShot_Stop(&instance->stopStepsTimer);
    TimerOneShot_Start(&instance->publishStepsDoneEventTimer);
}

static I_Event_t * GetOnStepDoneEvent(I_StepperMotor_t *_instance)
{
    RECAST(instance, _instance, StepperMotor_t *);
    return &instance->onStepsDoneEvent.interface;
}

static bool IsBusy(I_StepperMotor_t *_instance)
{
    RECAST(instance, _instance, StepperMotor_t *);
    return instance->busy;
}

static const StepperMotorApi_t api =
    {
         DoStep,
         Stop,
         GetOnStepDoneEvent,
         IsBusy
    };

void StepperMotor_Init(
        StepperMotor_t *instance,
        TimerModule_t *timerModule,
        I_GpioGroup_t *gpioGroup,
        const StepperMotorConfig_t *config)
{
    instance->interface.api = &api;
    instance->pwmIndex = Pwm1;
    instance->busy = false;
    instance->config = config;
    instance->gpioGroup = gpioGroup;
    Event_Synchronous_Init(&instance->onStepsDoneEvent);

    TurnAllPwmsOff(instance);

    TimerOneShot_Init(
            &instance->stopStepsTimer,
            timerModule,
            1,
            StopSteps,
            instance);
    TimerOneShot_Init(
            &instance->publishStepsDoneEventTimer,
            timerModule,
            PeriodToPublishStepsDoneEventAfterMs,
            PublishStepsDoneEvent,
            instance);
    TimerPeriodic_Init(
            &instance->moveStepTimer,
            timerModule,
            MinimumPeriodForStepSubCycleMs,
            DoStepSubCycle,
            instance);
}
