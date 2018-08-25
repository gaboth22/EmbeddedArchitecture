#include "Input_TimedButtonPress.h"
#include "I_GpioGroup.h"
#include "utils.h"

static void PublishOnTimedPressEvent(void *context)
{
    RECAST(instance, context, Input_TimedButtonPress_t *);
    Event_Publish(&instance->onTimedPressEvent.interface, NULL);
}

static void StartTimedPressTimer(void *context, void *args)
{
    RECAST(instance, context, Input_TimedButtonPress_t *);
    RECAST(buttonState, args, GpioState_t *);

    if(instance->timerRunning)
    {
        TimerOneShot_Stop(&instance->timedPressTimer);
        instance->timerRunning = false;
    }
    else if(*buttonState == GpioState_Low)
    {
        instance->timerRunning = true;
        TimerOneShot_Start(&instance->timedPressTimer);
    }
}

static I_Event_t * GetOnChangeEvent(I_Input_t *_instance)
{
    RECAST(instance, _instance, Input_TimedButtonPress_t *);
    return &instance->onTimedPressEvent.interface;
}

static void Read(I_Input_t *instance, void *bufferToReadInto)
{
    IGNORE(instance);
    IGNORE(bufferToReadInto);
}

static const InputApi_t api =
    { GetOnChangeEvent, Read };

void Input_TimedButtonPress_Init(
        Input_TimedButtonPress_t *instance,
        I_Input_t *button,
        TimerTicks_t periodToCountPressAfter,
        TimerModule_t *timerModule)
{
    instance->interface.api = &api;
    instance->timerRunning = false;
    TimerOneShot_Init(
            &instance->timedPressTimer,
            timerModule,
            periodToCountPressAfter,
            PublishOnTimedPressEvent,
            instance);
    Event_Synchronous_Init(&instance->onTimedPressEvent);
    EventSubscriber_Synchronous_Init(
            &instance->inputButtonSub,
            StartTimedPressTimer,
            instance);
    Event_Subscribe(Input_GetOnChangeEvent(button), &instance->inputButtonSub.interface);
}
