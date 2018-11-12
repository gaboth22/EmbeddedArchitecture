#include "DistanceSensorTestDouble.h"

extern "C"
{
#include "utils.h"
}

static DistanceInCm_t GetDitance(I_DistanceSensor_t *_instance)
{
    RECAST(instance, _instance, DistanceSensorTestDouble_t *);
    return instance->distance;
}

static const DistanceSensorApi_t api =
    { GetDitance };

void DistanceSensorTestDouble_SetDistanceToReturn(
    DistanceSensorTestDouble_t *instance,
    DistanceInCm_t toReturn)
{
    instance->distance = toReturn;
}

void DistanceSensorTestDouble_Init(DistanceSensorTestDouble_t *instance)
{
    instance->interface.api = &api;
}
