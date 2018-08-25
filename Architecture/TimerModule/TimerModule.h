#ifndef TIMERMODULE_H
#define TIMERMODULE_H

#include "I_TimeSource.h"
#include "types.h"

typedef uint32_t TimerTicks_t;

typedef struct
{
    TimerTicks_t currentTicks;
    TimerTicks_t previousTicks;
} TimerModule_t;

/*
 * Initializes singleton timer module
 */
TimerModule_t * TimerModule_Init(I_TimeSource_t *timeSource);

/*
 * Get periodic event
 */
I_Event_t * TimerModule_GetPeriodEvent(TimerModule_t *timerModule);

/*
 * Must be placed in main loop
 */
void TimerModule_Run(TimerModule_t *timerModule);

#endif
