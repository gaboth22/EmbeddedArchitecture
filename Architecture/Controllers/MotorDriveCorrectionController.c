#include "MotorDriveCorrectionController.h"
#include "TimerPeriodic.h"
#include "utils.h"

enum
{
    OverFactor = 1,
    ThresholToConsiderAWallInThatDirectionCm = 40,
    ThresholdToConsiderWallTooCloseCm = 15,
    ForceCorrectionFactor = 15
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
        if(leftUltraSonicDistanceCm < ThresholdToConsiderWallTooCloseCm)
        {
            return ForceCorrectionFactor;
        }
        if(rightUltraSonicDistanceCm < ThresholdToConsiderWallTooCloseCm)
        {
            return -ForceCorrectionFactor;
        }

        factor = rightUltraSonicDistanceCm - leftUltraSonicDistanceCm;
        factor = (factor > 0) ? (factor + OverFactor) : (factor - OverFactor);
    }
    else if(leftUltraSonicDistanceCm < ThresholToConsiderAWallInThatDirectionCm)
    {
        if(leftUltraSonicDistanceCm < ThresholdToConsiderWallTooCloseCm)
        {
//            factor = TRUNCATE_U16_SUBTRACTION(ThresholdToConsiderWallTooCloseToWallCm, leftUltraSonicDistanceCm);
//            factor += factor > 0 ? OverFactor : 0;
            return ForceCorrectionFactor;
        }
    }
    else if(rightUltraSonicDistanceCm < ThresholToConsiderAWallInThatDirectionCm)
    {
        if(rightUltraSonicDistanceCm < ThresholdToConsiderWallTooCloseCm)
        {
//            factor = TRUNCATE_U16_SUBTRACTION(ThresholdToConsiderWallTooCloseToWallCm, leftUltraSonicDistanceCm);
//            factor += factor > 0 ? OverFactor : 0;
//            factor = -1 * factor;
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
