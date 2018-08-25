#ifndef INPUT_BUTTON_H
#define INPUT_BUTTON_H

#include "../Timer/TimerPeriodic.h"
#include "I_Input.h"
#include "I_GpioGroup.h"
#include "TimerModule.h"
#include "Event_Synchronous.h"

typedef struct
{
    I_Input_t interface;
    TimerPeriodic_t pollInputTimer;
    GpioChannel_t inputChannel;
    GpioState_t currentState;
    GpioState_t previousState;
    Event_Synchronous_t onInputChange;
    I_GpioGroup_t *gpioGoup;
} Input_Button_t;

void Input_Button_Init(
        Input_Button_t *instance,
        I_GpioGroup_t *gpioGroup,
        GpioChannel_t inputChannel,
        TimerModule_t *timerModule,
        GpioState_t initialState);

#endif
