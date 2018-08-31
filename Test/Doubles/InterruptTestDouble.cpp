#include "InterruptTestDouble.h"

extern "C"
{
#include "I_Event.h"
#include "utils.h"
#include "types.h"
}

static Event_Synchronous_t onInterrupt;

static I_Event_t * GetOnInterruptEvent(I_Interrupt_t *instance)
{
	IGNORE(instance);
	return &onInterrupt.interface;
}

static const InterruptApi_t interruptApi =
	{ GetOnInterruptEvent };

InterruptTestDouble::InterruptTestDouble()
{
	this->interrupt.api = &interruptApi;
	Event_Synchronous_Init(&onInterrupt);
}

void InterruptTestDouble::GenerateInterrupt(int numberOfTimes)
{
	for(int i = 0; i < numberOfTimes; i++)
	{
		Event_Publish(&onInterrupt.interface, NULL);
	}
}
