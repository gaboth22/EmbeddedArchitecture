#include "Input_Dial.h"
#include "I_Event.h"
#include "utils.h"

enum
{
    PeriodToPollAdcMs = 100,
	AdcCountReadingThreshold = 10
};

static void PollAdc(void *context)
{
    RECAST(instance, context, Input_Dial_t *);
    instance->currentCounts = Adc_GetCounts(instance->dialAdc);

    uint16_t difference;
    if(instance->currentCounts > instance->previousCounts)
    {
        difference = instance->currentCounts - instance->previousCounts;
    }
    else
    {
        difference = instance->previousCounts - instance->currentCounts;
    }

    if(difference >= AdcCountReadingThreshold)
    {
        instance->previousCounts = instance->currentCounts;
        Event_Publish(&instance->onAdcCountsChange.interface, &instance->currentCounts);
    }
}

static void ReadCounts(I_Input_t *_instance, void *bufferToReadInto)
{
    RECAST(instance, _instance, Input_Dial_t *);
    *((AdcCounts_t *)(bufferToReadInto)) = Adc_GetCounts(instance->dialAdc);
}

static I_Event_t * GetOnChangeEvent(I_Input_t *_instance)
{
    RECAST(instance, _instance, Input_Dial_t *);
    return &instance->onAdcCountsChange.interface;
}

static const InputApi_t inputDialApi =
    { GetOnChangeEvent, ReadCounts };

void Input_Dial_Init(Input_Dial_t *instance, I_Adc_t *dialAdc, TimerModule_t *timerModule)
{
    instance->interface.api = &inputDialApi;
    instance->currentCounts = 0;
    instance->previousCounts = 0;
    instance->dialAdc = dialAdc;
    Event_Synchronous_Init(&instance->onAdcCountsChange);
    TimerPeriodic_Init(&instance->timerToPollAdc, timerModule, PeriodToPollAdcMs, PollAdc, instance);
    TimerPeriodic_Start(&instance->timerToPollAdc);
}
