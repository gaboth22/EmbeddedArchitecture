#include "DistanceProviderCm.h"
#include "utils.h"

static void GetTickCount(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, DistanceProviderCm_t *);
    instance->tickCount++;
}

void DistanceProviderCm_EnableDistanceTracking(DistanceProviderCm_t *instance)
{
    Event_Subscribe(instance->onEncoderTick, &instance->encoderTickSub.interface);
}

void DistanceProviderCm_DisableDistanceTracking(DistanceProviderCm_t *instance)
{
    Event_Unsubscribe(instance->onEncoderTick, &instance->encoderTickSub.interface);
}

DistanceInCm_t DistanceProviderCm_GetDistance(DistanceProviderCm_t *instance)
{
    return 5 * (instance->tickCount / instance->ticksToMove5Cm);
}

void DistanceProviderCm_ClearDistance(DistanceProviderCm_t *instance)
{
    instance->tickCount = 0;
}

uint16_t DistanceProviderCm_GetTicksFromCm(DistanceProviderCm_t *instance, DistanceInCm_t cm)
{
    return cm * instance->ticksToMove1Cm;
}

void DistanceProviderCm_Init(
    DistanceProviderCm_t *instance,
    I_Event_t *onEncoderTick,
    uint16_t ticksToMove5Cm,
    uint16_t ticksToMove1Cm)
{
    EventSubscriber_Synchronous_Init(
        &instance->encoderTickSub,
        GetTickCount,
        instance);
    instance->onEncoderTick = onEncoderTick;
    instance->ticksToMove5Cm = ticksToMove5Cm;
    instance->ticksToMove1Cm = ticksToMove1Cm;
    instance->tickCount = 0;
}
