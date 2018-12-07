#include "MotorDriveCorrectionController.h"
#include "TimerPeriodic.h"
#include "utils.h"

int64_t MotorDriveCorrectionController_GetCorrectionFactor(MotorDriveCorrectionController_t *instance)
{
    DistanceInCm_t leftUltraSonicDistanceCm =
        DistanceSensor_GetDistanceInCm(instance->leftDistSensor);
    DistanceInCm_t rightUltraSonicDistanceCm =
        DistanceSensor_GetDistanceInCm(instance->rightDistSensor);

    int64_t factor = 0;

    if(leftUltraSonicDistanceCm <= 15)
    {
        factor = 16;
    }
    if(leftUltraSonicDistanceCm <= 12)
    {
        factor = 32;
    }

    if(rightUltraSonicDistanceCm <= 15)
    {
        factor = -1 * 12;
    }
    else if(rightUltraSonicDistanceCm <= 12)
    {
        factor = -1 * 25;
    }

    return factor;
}

void MotorDriveCorrectionController_Init(
    MotorDriveCorrectionController_t *instance,
    I_DistanceSensor_t *leftDistSensor,
    I_DistanceSensor_t *rightDistSensor)
{
    instance->leftDistSensor = leftDistSensor;
    instance->rightDistSensor = rightDistSensor;
}
