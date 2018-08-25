#ifndef I_TIMESOURCE_H
#define I_TIMESOURCE_H

#include "I_Event.h"

typedef struct _TimeSourceApi_t TimeSourceApi_t;

typedef struct
{
	const TimeSourceApi_t *api;
} I_TimeSource_t;

struct _TimeSourceApi_t
{
	/*
	 * Get event generated when the time source elapses a time period
	 */
	I_Event_t * (*GetOnTimePeriodEvent)(I_TimeSource_t *instance);
};

#define TimeSource_GetOnTimePeriodEvent(_instance) \
        (_instance)->api->GetOnTimePeriodEvent(_instance) \

#endif
