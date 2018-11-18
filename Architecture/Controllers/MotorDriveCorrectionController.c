#include "MotorDriveCorrectionController.h"
#include "TimerPeriodic.h"
#include "utils.h"

enum
{
    ThresholToConsiderAWallInThatDirectionCm = 30,
    RightThresholdToConsiderWallTooCloseCm = 14,
    LeftThresholdToConsiderWallTooCloseCm = 15,
    ForceCorrectionFactor = 14,
    OverFactor = 8,
    SmallFactor = 5
};

int64_t MotorDriveCorrectionController_GetCorrectionFactor(MotorDriveCorrectionController_t *instance)
{
    DistanceInCm_t leftUltraSonicDistanceCm =
        DistanceSensor_GetDistanceInCm(instance->leftDistSensor);
    DistanceInCm_t rightUltraSonicDistanceCm =
        DistanceSensor_GetDistanceInCm(instance->rightDistSensor);

    int64_t factor = 0;

    if(leftUltraSonicDistanceCm <= LeftThresholdToConsiderWallTooCloseCm)
    {
        return ForceCorrectionFactor;
    }
    else if(rightUltraSonicDistanceCm <= RightThresholdToConsiderWallTooCloseCm)
    {
        return -1 * ForceCorrectionFactor;
    }
    else if(leftUltraSonicDistanceCm < ThresholToConsiderAWallInThatDirectionCm &&
            rightUltraSonicDistanceCm < ThresholToConsiderAWallInThatDirectionCm)
    {
//        factor = rightUltraSonicDistanceCm > leftUltraSonicDistanceCm ?
//                TRUNCATE_U64_SUBSTRACTION(rightUltraSonicDistanceCm, leftUltraSonicDistanceCm) :
//                -(TRUNCATE_U64_SUBSTRACTION(leftUltraSonicDistanceCm, rightUltraSonicDistanceCm));

//        factor = (factor > 0) ? (factor + OverFactor) : (factor - OverFactor);

        factor = rightUltraSonicDistanceCm > leftUltraSonicDistanceCm ? SmallFactor : -1 * SmallFactor;
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
