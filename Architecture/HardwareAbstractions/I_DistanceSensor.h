#ifndef I_DISTANCESENSOR_H
#define I_DISTANCESENSOR_H

#include "I_Event.h"
#include "types.h"

typedef uint16_t DistanceInCm_t;
typedef struct _DistanceSensorApi_t DistanceSensorApi_t;

typedef struct
{
    const DistanceSensorApi_t *api;
} I_DistanceSensor_t;

struct _DistanceSensorApi_t
{
    /*
     * Get the latest read distance in cm.
     */
    DistanceInCm_t (*GetDistanceInCm)(I_DistanceSensor_t *instance);
};

#define DistanceSensor_GetDistanceInCm(_instance) \
    (_instance)->api->GetDistanceInCm(_instance) \

#endif
