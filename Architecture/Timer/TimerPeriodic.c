#include "TimerPeriodic.h"
#include "utils.h"

static void CallTimerCallback(void *context, void *args)
{
    RECAST(timer, context, TimerPeriodic_t *);
    RECAST(currentTime, args, TimerTicks_t *);

    if(*currentTime - timer->startTicks >= timer->expiryTicks)
    {
        timer->startTicks = *currentTime;
        if(!timer->paused)
        {
            timer->callback(timer->context);
        }
    }
}

void TimerPeriodic_Start(TimerPeriodic_t *timer)
{
	if(!timer->running)
	{
		timer->startTicks = timer->timerModule->currentTicks;
    	Event_Subscribe(TimerModule_GetPeriodEvent(timer->timerModule), &timer->timerModuleSubscriber.interface);
    	timer->running = true;
	}
}

void TimerPeriodic_Command(TimerPeriodic_t *timer, Timer_Periodic_Command_t commandToExecute)
{
    switch(commandToExecute)
    {
        case TimerPeriodicCommand_Pause:
            timer->paused = true;
            break;
        case TimerPeriodicCommand_Resume:
            timer->paused = false;
            break;
        case TimerPeriodicCommand_Stop:
        	if(timer->running)
        	{
        		Event_Unsubscribe(TimerModule_GetPeriodEvent(timer->timerModule), &timer->timerModuleSubscriber.interface);
        		timer->running = false;
        	}
        	break;
        default:
            break;
    }
}

void TimerPeriodic_UpdateExpiryTicks(TimerPeriodic_t *timer, TimerTicks_t newExpiryTicks)
{
    timer->expiryTicks = newExpiryTicks;
}

void TimerPeriodic_Init(
        TimerPeriodic_t *timer,
        TimerModule_t *timerModule,
        TimerTicks_t expiryTicks,
        TimerPeriodicCallback_t callback,
        void *context)
{
    timer->timerModule = timerModule;
    timer->expiryTicks = expiryTicks;
    timer->callback = callback;
    timer->context = context;
    timer->paused = false;
    timer->running = false;
    EventSubscriber_Synchronous_Init(&timer->timerModuleSubscriber, CallTimerCallback, timer);
}
