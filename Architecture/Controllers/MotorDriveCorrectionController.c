#include "MotorDriveCorrectionController.h"
#include "TimerPeriodic.h"
#include "utils.h"

enum
{
    ThresholToConsiderAWallInThatDirectionCm = 30,
    RightThresholdToConsiderWallTooCloseCm = 13,
    LeftThresholdToConsiderWallTooCloseCm = 17,
    LeftCorrectionFactor = 25,
    RightCorrectionFactor = 27,
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

//    if(abs(leftUltraSonicDistanceCm - rightUltraSonicDistanceCm) > 4)
//    {
//        if(leftUltraSonicDistanceCm <= LeftThresholdToConsiderWallTooCloseCm)
//        {
//            factor = RightCorrectionFactor;
//        }
//        else if(rightUltraSonicDistanceCm <= RightThresholdToConsiderWallTooCloseCm)
//        {
//            factor = -1 * LeftCorrectionFactor;
//        }
//    }

    if(leftUltraSonicDistanceCm <= 14)
    {
        factor = 30;
    }
    else if(rightUltraSonicDistanceCm <= 13)
    {
        factor = -1 * 30;
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
