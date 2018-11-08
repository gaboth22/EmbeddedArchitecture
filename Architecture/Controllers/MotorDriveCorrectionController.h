#ifndef MOTORDRIVECORRECTIONCONTROLLER_H
#define MOTORDRIVECORRECTIONCONTROLLER_H

#include "I_Pwm.h"
#include "I_DistanceSensor.h"
#include "types.h"
#include "TimerPeriodic.h"

typedef struct
{
    I_Pwm_t *pwmLeftFwd;
    I_Pwm_t *pwmRightFwd;
    I_DistanceSensor_t *leftDistSensor;
    I_DistanceSensor_t *rightDistSensor;
    TimerPeriodic_t motorCorrectionTimer;
} MotorDriveCorrectionController_t;

/*
 * Gets a factor to add to pwm value for the motors
 */
int64_t MotorDriveCorrectionController_GetCorrectionFactor(MotorDriveCorrectionController_t *instance);

/*
 * Init Motor Drive Correction Controller
 */
void MotorDriveCorrectionController_Init(
    MotorDriveCorrectionController_t *instance,
    I_DistanceSensor_t *leftDistSensor,
    I_DistanceSensor_t *rightDistSensor);

 #endif
