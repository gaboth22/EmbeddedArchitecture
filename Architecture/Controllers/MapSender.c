#include "MapSender.h"
#include "utils.h"

enum
{
    SendMapUartCommand = 0xA3,
    SendMapUartAck = 0xA3,
    MapSizeInBytes = 512,
    PeriodToResetModuleMs = 500,
    PeriodToPollUartMs = 5
};

static void ActuallySendMaps(MapSender_t *instance);

static void TryAcquireUart(void *context)
{
    RECAST(instance, context, MapSender_t *);
    if(Uart_Acquire(instance->wifiUart))
    {
        TimerPeriodic_Command(&instance->timerToAcquireUart, TimerPeriodicCommand_Stop);
        ActuallySendMaps(instance);
    }
}

static void ResetModule(void *context)
{
    RECAST(instance, context, MapSender_t *);
    TimerPeriodic_Command(&instance->timerToAcquireUart, TimerPeriodicCommand_Stop);
    instance->dmaTrxDone = true;
    instance->gotAck = true;
    DmaController_ClearState(instance->dmaController);
}

static void InterpretAck(void *context, void *args)
{
    RECAST(instance, context, MapSender_t *);
    RECAST(byte, args, uint8_t *);

    if(*byte == SendMapUartAck)
    {
        instance->gotAck = true;
    }
}

static void FlagDmaTrxDone(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, MapSender_t *);
    instance->dmaTrxDone = true;
}

static void ActuallySendMaps(MapSender_t *instance)
{
    Event_Subscribe(
        DmaController_GetOnChannelTransferDoneEvent(instance->dmaController, instance->dmaChannel),
        &instance->onDmaTrxDoneSub.interface);

    Event_Subscribe(Uart_GetOnByteReceivedEvent(instance->wifiUart), &instance->onByteReceivedSub.interface);

    TimerOneShot_Stop(&instance->timerToResetModule);
    TimerOneShot_Start(&instance->timerToResetModule);

    Uart_SendByte(instance->wifiUart, SendMapUartCommand);

    DmaController_SetChannelSourceTrigger(
         instance->dmaController,
         instance->dmaChannel,
         NULL);
     DmaController_SetChannelTransferConfig(
         instance->dmaController,
         instance->dmaChannel,
         NULL);
     DmaController_SetAndStartChannelTrasfer(
         instance->dmaController,
         instance->dmaChannel,
         NULL,
         instance->blocked,
         instance->dmaOutputBufferAddress,
         MapSizeInBytes);
}

void MapSender_SendMaps(
    MapSender_t *instance,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visited,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blocked)
{
    if(!instance->busy)
    {
        instance->busy = true;
        instance->visited = visited;
        instance->blocked = blocked;

        if(Uart_Acquire(instance->wifiUart))
        {
            ActuallySendMaps(instance);
        }
        else
        {
            TimerPeriodic_Command(&instance->timerToAcquireUart, TimerPeriodicCommand_Stop);
            TimerPeriodic_Start(&instance->timerToAcquireUart);
        }
    }
}

void MapSender_Run(MapSender_t *instance)
{
    if(instance->dmaTrxDone && instance->gotAck)
    {
        instance->dmaTrxDone = false;
        instance->gotAck = false;
        TimerOneShot_Stop(&instance->timerToResetModule);
        Event_Unsubscribe(
            DmaController_GetOnChannelTransferDoneEvent(instance->dmaController, instance->dmaChannel),
            &instance->onDmaTrxDoneSub.interface);
        Event_Unsubscribe(Uart_GetOnByteReceivedEvent(instance->wifiUart), &instance->onByteReceivedSub.interface);

        Event_Publish(&instance->onMapSent.interface, NULL);
        Uart_Release(instance->wifiUart);
        instance->busy = false;
    }
}

I_Event_t * MapSender_GetOnMapsSentEvent(MapSender_t *instance)
{
    return &instance->onMapSent.interface;
}

void MapSender_Init(
    MapSender_t *instance,
    I_Uart_t *wifiUart,
    I_DmaController_t *dmaController,
    uint32_t dmaChannel,
    void *outputBufferAddress,
    TimerModule_t *timerModule)
{
    instance->wifiUart = wifiUart;
    instance->dmaController = dmaController;
    instance->dmaChannel = dmaChannel;
    instance->gotAck = false;
    instance->dmaOutputBufferAddress = outputBufferAddress;
    instance->busy = false;

    TimerPeriodic_Init(&instance->timerToAcquireUart, timerModule, PeriodToPollUartMs, TryAcquireUart, instance);
    TimerOneShot_Init(&instance->timerToResetModule, timerModule, PeriodToResetModuleMs, ResetModule, instance);

    Event_Synchronous_Init(&instance->onMapSent);
    EventSubscriber_Synchronous_Init(&instance->onDmaTrxDoneSub, FlagDmaTrxDone, instance);
    EventSubscriber_Synchronous_Init(&instance->onByteReceivedSub, InterpretAck, instance);
}
