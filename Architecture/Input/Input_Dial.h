#ifndef INPUT_DIAL_H
#define INPUT_DIAL_H

#include "../Timer/TimerPeriodic.h"
#include "I_Input.h"
#include "I_Adc.h"
#include "TimerModule.h"
#include "Event_Synchronous.h"

typedef struct
{
    I_Input_t interface;
    I_Adc_t *dialAdc;
    TimerPeriodic_t timerToPollAdc;
    AdcCounts_t previousCounts;
    AdcCounts_t currentCounts;
    Event_Synchronous_t onAdcCountsChange;
} Input_Dial_t;

void Input_Dial_Init(Input_Dial_t *instance, I_Adc_t *dialAdc, TimerModule_t *timerModule);

#endif
