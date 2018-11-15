#include "MotorDriveCorrectionController.h"
#include "TimerPeriodic.h"
#include "utils.h"

enum
{
    OverFactor = 10,
    ThresholToConsiderAWallInThatDirectionCm = 30,
    ThresholdToConsiderWallTooCloseCm = 14,
    LeftThresholdToConsiderWallTooCloseCm = 10,
    ForceCorrectionFactor = 15,
    SmallOverFactor = 6
};

int64_t MotorDriveCorrectionController_GetCorrectionFactor(MotorDriveCorrectionController_t *instance)
{
    DistanceInCm_t leftUltraSonicDistanceCm =
        DistanceSensor_GetDistanceInCm(instance->leftDistSensor);
    DistanceInCm_t rightUltraSonicDistanceCm =
        DistanceSensor_GetDistanceInCm(instance->rightDistSensor);

    int64_t factor = 0;

    if(leftUltraSonicDistanceCm < ThresholToConsiderAWallInThatDirectionCm &&
       rightUltraSonicDistanceCm < ThresholToConsiderAWallInThatDirectionCm)
    {
        factor = rightUltraSonicDistanceCm > leftUltraSonicDistanceCm ?
                rightUltraSonicDistanceCm - leftUltraSonicDistanceCm :
                -(leftUltraSonicDistanceCm - rightUltraSonicDistanceCm);

        factor = (factor > 0) ? (factor + SmallOverFactor) : (factor - SmallOverFactor);
    }

    if(leftUltraSonicDistanceCm < ThresholToConsiderAWallInThatDirectionCm)
    {
        if(leftUltraSonicDistanceCm < LeftThresholdToConsiderWallTooCloseCm)
        {
            return ForceCorrectionFactor;
        }
    }

    if(rightUltraSonicDistanceCm < ThresholToConsiderAWallInThatDirectionCm)
    {
        if(rightUltraSonicDistanceCm < ThresholdToConsiderWallTooCloseCm)
        {
            return -ForceCorrectionFactor;
        }
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
