#ifndef TIMER_PERIODIC_H
#define TIMER_PERIODIC_H

#include "TimerModule.h"
#include "EventSubscriber_Synchronous.h"
#include "types.h"

typedef void (*TimerPeriodicCallback_t)(void *context);

typedef enum
{
    TimerPeriodicCommand_Pause = 0,
    TimerPeriodicCommand_Resume,
    TimerPeriodicCommand_Stop
} Timer_Periodic_Command_t;

typedef struct
{
    EventSubscriber_Synchronous_t timerModuleSubscriber;
    TimerModule_t *timerModule;
    TimerTicks_t expiryTicks;
    TimerTicks_t startTicks;
    TimerPeriodicCallback_t callback;
    void *context;
    bool paused;
    bool running;
} TimerPeriodic_t;

/*
 * Initializes the periodic timer.
 * @param timer The timer instance
 * @param timerModule the timer module
 * @param expiryTicks ticks after which the timer will expire
 * @param callback to be called when ticks expire
 * @param context to be passed to callback
 */
void TimerPeriodic_Init(
        TimerPeriodic_t *timer,
        TimerModule_t *timerModule,
        TimerTicks_t expiryTicks,
        TimerPeriodicCallback_t callback,
        void *context);

/*
 * Start the timer
 */
void TimerPeriodic_Start(TimerPeriodic_t *timer);

/*
 * Have timer pause, resume or stop
 */
void TimerPeriodic_Command(TimerPeriodic_t *timer, Timer_Periodic_Command_t commandToExecute);

/*
 * Updates the expiry period of the timer
 */
void TimerPeriodic_UpdateExpiryTicks(TimerPeriodic_t *timer, TimerTicks_t newExpiryTicks);

#endif
