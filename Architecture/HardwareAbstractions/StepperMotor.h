#ifndef STEPPERMOTOR_GASLEVELDISPLAY_H
#define STEPPERMOTOR_GASLEVELDISPLAY_H

#include "../Timer/TimerOneShot.h"
#include "../Timer/TimerPeriodic.h"
#include "I_StepperMotor.h"
#include "I_GpioGroup.h"
#include "Event_Synchronous.h"
#include "TimerModule.h"

typedef struct
{
    GpioChannel_t pwm1Channel;
    GpioChannel_t pwm2Channel;
    GpioChannel_t pwm3Channel;
    GpioChannel_t pwm4Channel;
} StepperMotorConfig_t;

typedef struct
{
    I_StepperMotor_t interface;
    Event_Synchronous_t onStepsDoneEvent;
    TimerPeriodic_t moveStepTimer;
    TimerOneShot_t stopStepsTimer;
    TimerOneShot_t publishStepsDoneEventTimer;
    StepDirection_t currentStepDirection;
    I_GpioGroup_t *gpioGroup;
    const StepperMotorConfig_t *config;
    uint8_t pwmIndex;
    bool busy;
} StepperMotor_t;

/*
 * Initialize stepper motor
 */
void StepperMotor_Init(
        StepperMotor_t *instance,
        TimerModule_t *timerModule,
        I_GpioGroup_t *gpioGroup,
        const StepperMotorConfig_t *config);

#endif
