#ifndef INTERRUPTTESTDOUBLE_H
#define INTERRUPTTESTDOUBLE_H

extern "C"
{
#include "I_Interrupt.h"
#include "Event_Synchronous.h"
}

class InterruptTestDouble
{
public:
	/*
	 * The actual interrupt
	 */
	I_Interrupt_t interrupt;

	/*
	 * Constructor
	 */
	InterruptTestDouble();

	/*
	 * Make interrupt "happen" as many times
	 */
	void GenerateInterrupt(int numberOfTimer);
};

#endif
