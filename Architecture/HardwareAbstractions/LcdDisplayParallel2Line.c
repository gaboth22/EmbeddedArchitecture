#include <string.h>
#include "LcdDisplayParallel2Line.h"
#include "utils.h"

static LcdDisplayParallel2Line_t instance;

static bool Busy(void)
{
    if(instance.doneSettingUp)
    {
        GpioState_t busyFlagState;
        GpioGroup_SetState(instance.gpioGroup, instance.D7Channel, GpioState_Low);
        GpioGroup_SetState(instance.gpioGroup, instance.RSChannel, GpioState_Low);
        GpioGroup_SetState(instance.gpioGroup, instance.RWChannel, GpioState_High);
        GpioGroup_SetState(instance.gpioGroup, instance.EnableChannel, GpioState_High);
        busyFlagState = GpioGroup_GetState(instance.gpioGroup, instance.BusyBitChannel);
        GpioGroup_SetState(instance.gpioGroup, instance.EnableChannel, GpioState_Low);

        return (bool)busyFlagState || instance.busy;
    }
    else
    {
        // We can't read the busy flag until we're done setting, so just return false
        return false;
    }
}

static void SendByteOut(void  *data)
{
    IGNORE(data);
    uint8_t byte;

    if(instance.dataIndex == instance.dataLength)
    {
        instance.doneSettingUp = true;
        instance.busy = false;
    }
    else
    {
        if(instance.writingCommand)
        {
            GpioGroup_SetState(instance.gpioGroup, instance.RSChannel, GpioState_Low);
            GpioGroup_SetState(instance.gpioGroup, instance.RWChannel, GpioState_Low);
        }
        else
        {
            GpioGroup_SetState(instance.gpioGroup, instance.RSChannel, GpioState_High);
            GpioGroup_SetState(instance.gpioGroup, instance.RWChannel, GpioState_Low);
        }

        byte = instance.data[instance.dataIndex++];
        GpioGroup_SetState(instance.gpioGroup, instance.EnableChannel, GpioState_High);
        GpioGroup_SetState(instance.gpioGroup, instance.D7Channel, (GpioState_t)((byte >> 7) & 0x01));
        GpioGroup_SetState(instance.gpioGroup, instance.D6Channel, (GpioState_t)((byte >> 6) & 0x01));
        GpioGroup_SetState(instance.gpioGroup, instance.D5Channel, (GpioState_t)((byte >> 5) & 0x01));
        GpioGroup_SetState(instance.gpioGroup, instance.D4Channel, (GpioState_t)((byte >> 4) & 0x01));
        GpioGroup_SetState(instance.gpioGroup, instance.EnableChannel, GpioState_Low);

        GpioGroup_SetState(instance.gpioGroup, instance.EnableChannel, GpioState_High);
        GpioGroup_SetState(instance.gpioGroup, instance.D7Channel, (GpioState_t)((byte >> 3) & 0x01));
        GpioGroup_SetState(instance.gpioGroup, instance.D6Channel, (GpioState_t)((byte >> 2) & 0x01));
        GpioGroup_SetState(instance.gpioGroup, instance.D5Channel, (GpioState_t)((byte >> 1) & 0x01));
        GpioGroup_SetState(instance.gpioGroup, instance.D4Channel, (GpioState_t)(byte & 0x01));
        GpioGroup_SetState(instance.gpioGroup, instance.EnableChannel, GpioState_Low);

        TimerOneShot_Init(
                &instance.writeByteTimer,
                instance.timerModule,
                1,
                SendByteOut,
                NULL);
        TimerOneShot_Start(&instance.writeByteTimer);
    }
}

static void WriteCommand(I_LcdDisplayParallel2Line_t *_instance, const uint8_t *commands, uint8_t commandsSize)
{
    IGNORE(_instance);

    if(!instance.busy)
    {
        instance.busy = true;
        memcpy(instance.data, commands, commandsSize);
        instance.dataLength = commandsSize;
        instance.writingCommand = true;
        instance.dataIndex = 0;
        SendByteOut(NULL);
    }
}

static void WriteData(I_LcdDisplayParallel2Line_t *_instance, const void *_data, uint8_t dataSize)
{
    IGNORE(_instance);

    if(!instance.busy)
    {
        instance.busy = true;
        memcpy(instance.data, _data, dataSize);
        instance.dataLength = dataSize;
        instance.writingCommand = false;
        instance.dataIndex = 0;
        SendByteOut(NULL);
    }
}

static bool IsBusy(I_LcdDisplayParallel2Line_t *_instance)
{
    IGNORE(_instance);
    return Busy() || instance.busy;
}

static const LcdDisplayParallel2LineApi_t lcdDisplayParallel2LineAlarmClockApi =
    { WriteCommand, WriteData, IsBusy };

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
        TimerModule_t *timerModule)
{
    instance.interface.api = &lcdDisplayParallel2LineAlarmClockApi;
    instance.doneSettingUp = false;
    instance.busy = false;
    instance.gpioGroup = gpioGroup;
    instance.D7Channel = D7Channel;
    instance.D6Channel = D6Channel;
    instance.D5Channel = D5Channel;
    instance.D4Channel = D4Channel;
    instance.EnableChannel = EnableChannel;
    instance.RWChannel = RWChannel;
    instance.RSChannel = RSChannel;
    instance.BusyBitChannel = BusyBitChannel;
    instance.timerModule = timerModule;
    static const uint8_t initCommands[] = { 0x33, 0x32, 0x28, 0x0C, 0x01 };
    WriteCommand(NULL, initCommands, 5);

    return &instance;
}
