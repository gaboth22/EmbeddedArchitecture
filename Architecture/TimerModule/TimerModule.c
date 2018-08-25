#include "TimerModule.h"
#include "Event_Synchronous.h"
#include "EventSubscriber_Synchronous.h"
#include "utils.h"

static TimerModule_t timerModule;
static EventSubscriber_Synchronous_t timeSourceSubscriber;
static Event_Synchronous_t onTimePeriod;

static void UpdateCount(void *context, void *args)
{
    IGNORE(context);
    IGNORE(args);

    timerModule.currentTicks++;
}

I_Event_t * TimerModule_GetPeriodEvent(TimerModule_t *timerModule)
{
    return &onTimePeriod.interface;
}

void TimerModule_Run(TimerModule_t *timerModule)
{
    if(timerModule->currentTicks - timerModule->previousTicks >= 1)
    {
        timerModule->previousTicks = timerModule->currentTicks;
        Event_Publish(&onTimePeriod.interface, &timerModule->currentTicks);
    }
}

TimerModule_t * TimerModule_Init(I_TimeSource_t *timeSource)
{
    timerModule.currentTicks = 0;
    timerModule.previousTicks = 0;
    Event_Synchronous_Init(&onTimePeriod);
    EventSubscriber_Synchronous_Init(&timeSourceSubscriber, UpdateCount, NULL);
    Event_Subscribe(TimeSource_GetOnTimePeriodEvent(timeSource), &timeSourceSubscriber.interface);

    return &timerModule;
}
