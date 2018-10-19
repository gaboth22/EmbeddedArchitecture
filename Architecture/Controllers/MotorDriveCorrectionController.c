#include "MotorDriveCorrectionController.h"
#include "TimerPeriodic.h"
#include "Utils.h"

static void StartMotorCorrectionCycle(void *context)
{
    RECAST(instance, context, MotorDriveCorrectionController_t *);

    DistanceInCm_t leftUltraSonicDistanceCm = DistanceSensor_GetDistanceInCm(&instance->leftUltraSonic.interface);
    DistanceInCm_t rightUltraSonicDistanceCm = DistanceSensor_GetDistanceInCm(&instance->rightUltraSonic.interface);

    if(leftUltraSonicDistanceCm > rightUltraSonicDistanceCm)
    {
        Pwm_SetDutyCycle(instance->pwmRightFwd, instance->pwmDutyCycleToCorrectMotors);
    }

    if(leftUltraSonicDistanceCm < rightUltraSonicDistanceCm)
    {
        Pwm_SetDutyCycle(instance->pwmLeftFwd, instance->pwmDutyCycleToCorrectMotors);
    }
}

void MotorDriveCorrectionController_Resume(MotorDriveCorrectionController_t *instance)
{
    TimerPeriodic_Command(&instance->motorCorrectionTimer, TimerPeriodicCommand_Resume);
}

void MotorDriveCorrectionController_Pause(MotorDriveCorrectionController_t *instance)
{
    TimerPeriodic_Command(&instance->motorCorrectionTimer, TimerPeriodicCommand_Pause);
}

void MotorDriveCorrectionController_Init(
   MotorDriveCorrectionController_t *instance,
   TimerModule_t *timerModule,
   I_Pwm_t *pwmLeftFwd,
   I_Pwm_t *pwmRightFwd,
   DistanceSensor_UltraSonicHCSR01_t leftUltraSonic,
   DistanceSensor_UltraSonicHCSR01_t rightUltraSonic)
{
    instance->pwmLeftFwd = pwmLeftFwd;
    instance->pwmRightFwd = pwmRightFwd;

    instance->leftUltraSonic = leftUltraSonic;
    instance->rightUltraSonic = rightUltraSonic;

    instance->pwmDutyCycleToCorrectMotors = 75;

    TimerPeriodic_Init(
        &instance->motorCorrectionTimer,
        timerModule,
        PeriodToDoMotorCorrectionMs,
        StartMotorCorrectionCycle,
        instance);

    TimerPeriodic_Start(&instance->motorCorrectionTimer);
}
