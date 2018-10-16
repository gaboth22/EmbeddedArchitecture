#include <stdlib.h>
#include "GpioGroupTestDouble.h"

extern "C"
{
#include "utils.h"
}

static void SetState(I_GpioGroup_t *instance, GpioChannel_t channel, GpioState_t state)
{
    RECAST(testDouble, instance, GpioGroupTestDouble_t *);
    testDouble->gpioStateMap[(uint8_t)channel] = (uint8_t)state;
}

static GpioState_t GetState(I_GpioGroup_t *instance, GpioChannel_t channel)
{
    RECAST(testDouble, instance, GpioGroupTestDouble_t *);
    return (GpioState_t)testDouble->gpioStateMap[(uint8_t)channel];
}

static const GpioGroupApi_t api =
    { SetState, GetState };

void GpioGroupTestDouble_Init(
        GpioGroupTestDouble_t *instance,
        uint8_t mappedGpioSize)
{
    instance->gpioStateMap = (uint8_t *)malloc((size_t)mappedGpioSize);
    instance->interface.api = &api;
}
