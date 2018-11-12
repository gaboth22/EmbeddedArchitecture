#ifndef DISTANCESENSORTESTDOUBLE_H
#define DISTANCESENSORTESTDOUBLE_H

extern "C"
{
#include "I_DistanceSensor.h"
}

typedef struct
{
    I_DistanceSensor_t interface;
    DistanceInCm_t distance;
} DistanceSensorTestDouble_t;

/*
 * Set the value that the double will return when the GetDistance method is called
 */
void DistanceSensorTestDouble_SetDistanceToReturn(
    DistanceSensorTestDouble_t *instance,
    DistanceInCm_t toReturn);

/*
 * Initialize the test double
 */
void DistanceSensorTestDouble_Init(DistanceSensorTestDouble_t *instance);

#endif
