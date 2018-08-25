#ifndef TIMERMODULE_TESTDOUBLE_H
#define TIMERMODULE_TESTDOUBLE_H

extern "C"
{
#include "I_TimeSource.h"
#include "TimerModule.h"
}

class TimerModuleTestDouble
{
private:
	I_TimeSource_t timeSource;

public:
	TimerModuleTestDouble();

	/*
	 * Timer module to be plugged into C code
	 */
	TimerModule_t *timerModule;

	/*
	 * Make the timer module elapse a certain amount of ticks
	 */
	void ElapseTicks(TimerTicks_t ticksToElapse);
};

#endif
