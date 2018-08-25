#ifndef TIMERONESHOT_H
#define TIMERONESHOT_H

#include "TimerModule.h"
#include "EventSubscriber_Synchronous.h"

typedef void (*TimerOneShotCallback_t)(void *context);

typedef struct
{
    EventSubscriber_Synchronous_t timerModuleSubscriber;
    TimerModule_t *timerModule;
    TimerTicks_t expiryTicks;
    TimerTicks_t startTicks;
    TimerOneShotCallback_t callback;
    void *context;
    bool running;
} TimerOneShot_t;


/*
 * Initializes the one-shot timer.
 * @param timer The timer instance
 * @param timerModule the timer module
 * @param expiryTicks ticks after which the timer will expire
 * @param callback to be called when ticks expire
 * @param context to be passed to callback
 */
void TimerOneShot_Init(
        TimerOneShot_t *timer,
        TimerModule_t *timerModule,
        TimerTicks_t expiryTicks,
        TimerOneShotCallback_t callback,
        void *context);

/*
 * Start the timer
 */
void TimerOneShot_Start(TimerOneShot_t *timer);

/*
 * Stop the timer
 */
void TimerOneShot_Stop(TimerOneShot_t *timer);

/*
 * Update expiry ticks for timer
 */
void TimerOneShot_UpdateExpiryTicks(TimerOneShot_t *timer, TimerTicks_t newExpiryTicks);

#endif
