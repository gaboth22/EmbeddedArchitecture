#ifndef LCDDISPLAYPARALLEL2LINE_H
#define LCDDISPLAYPARALLEL2LINE_H

#include "../Timer/TimerOneShot.h"
#include "I_LcdDisplayParallel2Line.h"
#include "I_GpioGroup.h"

typedef struct
{
    I_LcdDisplayParallel2Line_t interface;
    I_GpioGroup_t *gpioGroup;
    GpioChannel_t D7Channel;
    GpioChannel_t D6Channel;
    GpioChannel_t D5Channel;
    GpioChannel_t D4Channel;
    GpioChannel_t EnableChannel;
    GpioChannel_t RWChannel;
    GpioChannel_t RSChannel;
    GpioChannel_t BusyBitChannel;
    TimerOneShot_t writeByteTimer;
    uint8_t data[16];
    uint8_t dataLength;
    uint8_t dataIndex;
    bool writingCommand;
    bool busy;
    bool doneSettingUp;
    TimerModule_t *timerModule;
} LcdDisplayParallel2Line_t;

/*
 * Initialize the LCD Display and get singleton handle
 */
LcdDisplayParallel2Line_t * LcdDisplayParallel2Line_Init(
        I_GpioGroup_t *gpioGroup,
        GpioChannel_t D7Channel,
        GpioChannel_t D6Channel,
        GpioChannel_t D5Channel,
        GpioChannel_t D4Channel,
        GpioChannel_t EnableChannel,
        GpioChannel_t RWChannel,
        GpioChannel_t RSChannel,
        GpioChannel_t BusyBitChannel,
        TimerModule_t *timerModule);

#endif
