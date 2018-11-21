#include "ImageRecognitionController.h"
#include "utils.h"

enum
{
    PeriodToTryAndAcquireUartMs = 5,
    PeriodToResetModuleMs = 1000,
    RequestImageRecognitionCommand = 0xA4,
    RequestImageRecognitionAck = 0xA4,
};

static void ActuallySendRecognitionRequest(ImageRecognitionController_t *instance)
{
    Event_Subscribe(
        Uart_GetOnByteReceivedEvent(instance->wifiUart),
        &instance->onByteReceivedSub.interface);

    Uart_SendByte(instance->wifiUart, RequestImageRecognitionCommand);
    Uart_SendByte(instance->wifiUart, instance->xpos);
    Uart_SendByte(instance->wifiUart, instance->ypos);

    TimerOneShot_Stop(&instance->timerToResetModule);
    TimerOneShot_Start(&instance->timerToResetModule);
}

void ImageRecognitionController_RequestRecognition(
    ImageRecognitionController_t *instance,
    uint8_t xpos,
    uint8_t ypos)
{
    instance->xpos = xpos;
    instance->ypos = ypos;

    if(!instance->alreadySent)
    {
        instance->alreadySent = true;

        if(Uart_Acquire(instance->wifiUart))
        {
            ActuallySendRecognitionRequest(instance);
        }
        else
        {
            TimerPeriodic_Command(&instance->timerToAcquireUart, TimerPeriodicCommand_Stop);
            TimerPeriodic_Start(&instance->timerToAcquireUart);
        }
    }
    else
    {
        Event_Publish(&instance->onImageRecognized.interface, NULL);
    }
}

I_Event_t * ImageRecognitionController_GetOnImageRecognizedEvent(
    ImageRecognitionController_t *instance)
{
    return &instance->onImageRecognized.interface;
}

void ImageRecognitionController_Run(ImageRecognitionController_t *instance)
{
    if(instance->uartAcquiredAllgood)
    {
        instance->uartAcquiredAllgood = false;
        TimerPeriodic_Command(&instance->timerToAcquireUart, TimerPeriodicCommand_Stop);
    }

    if(instance->done)
    {
        instance->done = false;
        TimerOneShot_Stop(&instance->timerToResetModule);
        Event_Publish(&instance->onImageRecognized.interface, NULL);
        Uart_Release(instance->wifiUart);
    }
    else
    {
        Event_Publish(&instance->onImageRecognized.interface, NULL);
    }
}

static void TryToAcquireUart(void *context)
{
    RECAST(instance, context, ImageRecognitionController_t *);

    if(Uart_Acquire(instance->wifiUart))
    {
        TimerPeriodic_Command(&instance->timerToAcquireUart, TimerPeriodicCommand_Pause);
        instance->uartAcquiredAllgood = true;
        ActuallySendRecognitionRequest(instance);
    }
}

static void ResetModule(void *context)
{
    RECAST(instance, context, ImageRecognitionController_t *);
    TimerPeriodic_Command(&instance->timerToAcquireUart, TimerPeriodicCommand_Stop);
    Uart_Release(instance->wifiUart);
}

static void InterpretAck(void *context, void *args)
{
    RECAST(instance, context, ImageRecognitionController_t *);
    RECAST(byte, args, uint8_t *);

    if(*byte == RequestImageRecognitionAck)
    {
        instance->done = true;
    }
}

void ImageRecognitionController_Init(
    ImageRecognitionController_t *instance,
    I_Uart_t *wifiUart,
    TimerModule_t *timerModule)
{
    instance->wifiUart = wifiUart;
    instance->uartAcquiredAllgood = false;
    instance->done = false;
    instance->alreadySent = false;

    Event_Synchronous_Init(&instance->onImageRecognized);

    EventSubscriber_Synchronous_Init(
        &instance->onByteReceivedSub,
        InterpretAck,
        instance);

    TimerPeriodic_Init(
        &instance->timerToAcquireUart,
        timerModule,
        PeriodToTryAndAcquireUartMs,
        TryToAcquireUart,
        instance);

    TimerOneShot_Init(
        &instance->timerToResetModule,
        timerModule,
        PeriodToResetModuleMs,
        ResetModule,
        instance);
}
