#ifndef I_GPIOGROUP_H
#define I_GPIOGROUP_H

#include "types.h"

typedef struct _GpioGroupApi_t GpioGroupApi_t;
typedef uint8_t GpioChannel_t;

enum GpioState
{
    GpioState_Low = 0,
    GpioState_High = 1
};
typedef uint8_t GpioState_t;

typedef struct
{
    const GpioGroupApi_t *api;
} I_GpioGroup_t;

struct _GpioGroupApi_t
{
    /*
     * Set the state of a GPIO channel when it's an output
     */
    void (*SetState)(I_GpioGroup_t *instance, GpioChannel_t channel, GpioState_t state);

    /*
     * Get the state of a GPIO when it's an input
     */
    GpioState_t (*GetState)(I_GpioGroup_t *instance, GpioChannel_t channel);
};

#define GpioGroup_SetState(_instance, _channel, _state) \
        (_instance)->api->SetState(_instance, _channel, _state) \

#define GpioGroup_GetState(_instance, _channel) \
        (_instance)->api->GetState(_instance, _channel) \

#endif
