#ifndef MOTORDRIVECORRECTIONCONTROLLER_H
#define MOTORDRIVECORRECTIONCONTROLLER_H

#include "I_Pwm.h"
#include "types.h"
#include "UltrasonicSensorCommon.h"
#include "DistanceSensor_UltraSonicHCSR01.h"

enum
{
    PeriodToDoMotorCorrectionMs = 50
};

typedef struct
{
    I_Pwm_t *pwmLeftFwd;
    I_Pwm_t *pwmRightFwd;
    DistanceSensor_UltraSonicHCSR01_t leftUltraSonic;
    DistanceSensor_UltraSonicHCSR01_t rightUltraSonic;
    TimerPeriodic_t motorCorrectionTimer;
    uint8_t pwmDutyCycleToCorrectMotors;
} MotorDriveCorrectionController_t;

/*
 * Resumes Motor Drive Correction.
 */
void MotorDriveCorrectionController_Resume(MotorDriveCorrectionController_t *instance);

/*
 * Pauses Motor Drive Correction. Useful when making turns so we don't correct for that.
 */
void MotorDriveCorrectionController_Pause(MotorDriveCorrectionController_t *instance);

/*
 * Init Motor Drive Correction Controller
 */
void MotorDriveCorrectionController_Init(
   MotorDriveCorrectionController_t *instance,
   TimerModule_t *timerModule,
   I_Pwm_t *pwmLeftFwd,
   I_Pwm_t *pwmRightFwd,
   DistanceSensor_UltraSonicHCSR01_t leftUltraSonic,
   DistanceSensor_UltraSonicHCSR01_t rightUltraSonic);

#endif
