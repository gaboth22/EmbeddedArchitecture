#ifndef I_INTERRUPT_H
#define I_INTERRUPT_H

#include "I_Event.h"

typedef struct _InterruptApi_t InterruptApi_t;

typedef struct
{
    const InterruptApi_t *api;
} I_Interrupt_t;

struct _InterruptApi_t
{
	/*
	 * Gets the event that fires when the interrupt is generated
	 */
	I_Event_t * (*GetOnInterruptEvent)(I_Interrupt_t *instance);
};

#define Interrupt_GetOnInterruptEvent(_instance) \
        (_instance)->api->GetOnInterruptEvent(_instance) \

#endif
