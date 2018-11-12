#ifndef DISTANCEPROVIDERCM_H
#define DISTANCEPROVIDERCM_H

#include "I_Event.h"
#include "I_DistanceSensor.h"
#include "EventSubscriber_Synchronous.h"
#include "types.h"

typedef struct
{
    I_Event_t *onEncoderTick;
    EventSubscriber_Synchronous_t encoderTickSub;
    uint64_t tickCount;
    uint16_t ticksToMove5Cm;
    uint16_t ticksToMove1Cm;
} DistanceProviderCm_t;

/*
 * Enable distance tracking
 */
void DistanceProviderCm_EnableDistanceTracking(DistanceProviderCm_t *instance);

/*
 * Disable distance tracking
 */
void DistanceProviderCm_DisableDistanceTracking(DistanceProviderCm_t *instance);

/*
 * Get the current distance traversed in cm
 */
DistanceInCm_t DistanceProviderCm_GetDistance(DistanceProviderCm_t *instance);

/*
 * Clear current distance
 */
void DistanceProviderCm_ClearDistance(DistanceProviderCm_t *instance);

/*
 * Clear the current distance and start it from zero
 */
uint16_t DistanceProviderCm_GetTicksFromCm(DistanceProviderCm_t *instance, DistanceInCm_t cm);

/*
 * Initialize module
 */
void DistanceProviderCm_Init(
    DistanceProviderCm_t *instance,
    I_Event_t *onEncoderTick,
    uint16_t ticksToMove5Cm,
    uint16_t ticksToMove1Cm);

#endif
