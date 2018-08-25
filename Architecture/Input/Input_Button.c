#include "Input_Button.h"
#include "I_Event.h"
#include "utils.h"

enum
{
    PerioToPollInputMs = 100
};

static void PollInput(void *context)
{
    RECAST(instance, context, Input_Button_t *);
    instance->currentState = GpioGroup_GetState(instance->gpioGoup, instance->inputChannel);

    if(instance->currentState != instance->previousState)
    {
        instance->previousState = instance->currentState;
        Event_Publish(&instance->onInputChange.interface, &instance->currentState);
    }
}

static I_Event_t * GetOnChangeEvent(I_Input_t *_instance)
{
    RECAST(instance, _instance, Input_Button_t *);
    return &instance->onInputChange.interface;
}

static void Read(I_Input_t *_instance, void *buffer)
{
    RECAST(instance, _instance, Input_Button_t *);
    *((GpioState_t *)(buffer)) = GpioGroup_GetState(instance->gpioGoup, instance->inputChannel);
}

static const InputApi_t inputButtonApi =
    { GetOnChangeEvent, Read };

void Input_Button_Init(
        Input_Button_t *instance,
        I_GpioGroup_t *gpioGroup,
        GpioChannel_t inputChannel,
        TimerModule_t *timerModule,
        GpioState_t initialState)
{
    instance->currentState = initialState;
    instance->previousState = initialState;
    instance->interface.api = &inputButtonApi;
    instance->inputChannel = inputChannel;
    instance->gpioGoup = gpioGroup;
    Event_Synchronous_Init(&instance->onInputChange);
    TimerPeriodic_Init(&instance->pollInputTimer, timerModule, PerioToPollInputMs, PollInput, instance);
    TimerPeriodic_Start(&instance->pollInputTimer);
}
