#ifndef WAYPOINTPROVIDER_SIMPLE_H
#define WAYPOINTPROVIDER_SIMPLE_H

#include "I_WayPointProvider.h"
#include "I_DistanceSensor.h"

typedef struct
{
    I_WayPointProvider_t interface;
    I_DistanceSensor_t *left;
    I_DistanceSensor_t *front;
    I_DistanceSensor_t *right;
} WayPointProvider_Simple_t;

void WayPointProvider_Simple(
    WayPointProvider_Simple_t *instance,
    I_DistanceSensor_t *left,
    I_DistanceSensor_t *front,
    I_DistanceSensor_t *right);

#endif
