#ifndef I_INPUT_H
#define I_INPUT_H

#include "I_Event.h"

typedef struct _InputApi_t InputApi_t;

typedef struct
{
    const InputApi_t *api;
} I_Input_t;

struct _InputApi_t
{
    I_Event_t * (*GetOnChangeEvent)(I_Input_t *instance);

    void (*Read)(I_Input_t *instance, void *bufferToReadInto);
};

#define Input_GetOnChangeEvent(_instance) \
        (_instance)->api->GetOnChangeEvent(_instance) \

#define Input_Read(_instance, _buffer) \
        (_instance)->api->Read(_instance, _buffer) \

#endif
