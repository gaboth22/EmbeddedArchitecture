#include "EventSubscriber_Synchronous.h"
#include "I_Event.h"
#include "TimerModule.h"
#include "TimerOneShot.h"
#include "utils.h"

static void CallTimerCallback(void *context, void *args)
{
    RECAST(timer, context, TimerOneShot_t *);
    RECAST(currentTime, args, TimerTicks_t *);

    if(*currentTime - timer->startTicks >= timer->expiryTicks)
    {
        Event_Unsubscribe(TimerModule_GetPeriodEvent(timer->timerModule), &timer->timerModuleSubscriber.interface);
        timer->running = false;
        timer->callback(timer->context);
    }
}

void TimerOneShot_Start(TimerOneShot_t *timer)
{
	if(!timer->running)
	{
		timer->startTicks = timer->timerModule->currentTicks;
    	Event_Subscribe(TimerModule_GetPeriodEvent(timer->timerModule), &timer->timerModuleSubscriber.interface);
    	timer->running = true;
	}
}

void TimerOneShot_Stop(TimerOneShot_t *timer)
{
	if(timer->running)
	{
		Event_Unsubscribe(TimerModule_GetPeriodEvent(timer->timerModule), &timer->timerModuleSubscriber.interface);
		timer->running = false;
	}
}

void TimerOneShot_UpdateExpiryTicks(TimerOneShot_t *timer, TimerTicks_t newExpiryTicks)
{
    timer->expiryTicks = newExpiryTicks;
}

void TimerOneShot_Init(
        TimerOneShot_t *timer,
        TimerModule_t *timerModule,
        TimerTicks_t expiryTicks,
        TimerOneShotCallback_t callback,
        void *context)
{
    timer->timerModule = timerModule;
    timer->expiryTicks = expiryTicks;
    timer->callback = callback;
    timer->context = context;
    timer->running = false;
    EventSubscriber_Synchronous_Init(&timer->timerModuleSubscriber, CallTimerCallback, timer);
}
