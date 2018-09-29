#include "CommunicationArbiter.h"
#include "utils.h"

enum Ack
{
    Ack_ReceiveImageCommand = 0xA1
};
typedef uint8_t Ack_t;

enum RunningState
{
    RunningState_Unitialized = 0,
    RunningState_DoImageCaptureCycle,
    RunningState_WaitForAck
};
typedef uint8_t RunningSate_t;

static void InterpretAck(void *context, void *args)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    RECAST(ack, args, uint8_t *);

    switch(*ack)
    {
        case Ack_ReceiveImageCommand:
            instance->state = RunningState_DoImageCaptureCycle;
            break;

        default:
            break;
    }
}

static void ReEnableUartRxToWaitForAck(void *context, void *args)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    IGNORE(args);

    Uart_EnableRx(instance->wifiChipUart);
}

void CommunicationArbiter_Init(CommunicationArbiter_t *instance, I_Camera_t *camera, I_Uart_t *wifiChipUart, I_Event_t *onImageForwarded)
{
    instance->wifiChipUart = wifiChipUart;
    instance->camera = camera;
    instance->state = RunningState_DoImageCaptureCycle;

    EventSubscriber_Synchronous_Init(&instance->imageTrxDoneSub, ReEnableUartRxToWaitForAck, instance);
    Event_Subscribe(onImageForwarded, &instance->imageTrxDoneSub.interface);

    EventSubscriber_Synchronous_Init(&instance->uartByteReceivedSub, InterpretAck, instance);
    Event_Subscribe(Uart_GetOnByteReceivedEvent(wifiChipUart), &instance->uartByteReceivedSub.interface);
}

void CommunicationArbiter_Run(CommunicationArbiter_t *instance)
{
    switch(instance->state)
    {
        case RunningState_DoImageCaptureCycle:
            instance->state = RunningState_Unitialized;
            Camera_StartImageCapture(instance->camera);
            break;

        default:
            break;
    }
}
