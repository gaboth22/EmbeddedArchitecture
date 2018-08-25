#include "TimerModuleTestDouble.h"

extern "C"
{
#include "I_Event.h"
#include "Event_Synchronous.h"
#include "TimerModule.h"
#include "utils.h"
}

static Event_Synchronous_t onTimePeriodEvent;

static I_Event_t * GetOnTimePeriodEvent(I_TimeSource_t *instance)
{
	IGNORE(instance);
	return &onTimePeriodEvent.interface;
}

static const TimeSourceApi_t timeSourceApi =
	{ GetOnTimePeriodEvent };

TimerModuleTestDouble::TimerModuleTestDouble()
{
	Event_Synchronous_Init(&onTimePeriodEvent);
	this->timeSource.api = &timeSourceApi;
	this->timerModule = TimerModule_Init(&this->timeSource);
}

void TimerModuleTestDouble::ElapseTicks(TimerTicks_t ticksToElapse)
{
	for(TimerTicks_t i = 0; i < ticksToElapse; i++)
	{
		Event_Publish(&onTimePeriodEvent.interface, NULL);
		TimerModule_Run(this->timerModule);
	}
}
