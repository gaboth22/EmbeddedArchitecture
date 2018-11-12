#include "ImageForwardingController.h"
#include "utils.h"
#include "msp.h"

enum
{
    ImageMessageCommand = 0xA1,
    ImageMessageAck = 0xA1
};

static void ForwardImageToUart(void *context, void *args);

static void InterpretAck(void *context, void *args)
{
    RECAST(instance, context, ImageForwardingController_t *);
    RECAST(uartByte, args, uint8_t *);

    if((*uartByte) == ImageMessageAck)
    {
        instance->receivedAck = true;
    }
}

static void FlagTrxDone(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, ImageForwardingController_t *);
    instance->dmaTxDone = true;
}

I_Event_t * ImageForwardingController_GetOnImageForwardedEvent(ImageForwardingController_t *instance)
{
    return &instance->onImgFwdDoneEvent.interface;
}

void ImageForwardingController_Run(ImageForwardingController_t *instance)
{
    if(instance->receivedAck && instance->dmaTxDone)
    {
        instance->dmaTxDone = false;
        instance->receivedAck = false;
        Event_Unsubscribe(
            Uart_GetOnByteReceivedEvent(instance->wifiChipUart),
            &instance->uartByteReceivedSub.interface);
        instance->busy = false;
        TimerOneShot_Stop(&instance->timerToResetModule);
        Event_Publish(&instance->onImgFwdDoneEvent.interface, NULL);
        Uart_Release(instance->wifiChipUart);
    }
}

static void ActuallyForwardImage(ImageForwardingController_t *instance, CameraImage_t *image)
{
    Event_Subscribe(
        Uart_GetOnByteReceivedEvent(instance->wifiChipUart),
        &instance->uartByteReceivedSub.interface);

    Uart_SendByte(instance->wifiChipUart, ImageMessageCommand);
    uint16_t imgSizeHighByte = ((image->imageSize >> 8) & 0x00FF);
    uint16_t imgSizeLowByte = (image->imageSize & 0x00FF);
    Uart_SendByte(instance->wifiChipUart, imgSizeHighByte);
    Uart_SendByte(instance->wifiChipUart, imgSizeLowByte);

    instance->dmaTxDone = false;
    instance->receivedAck = false;

    DmaController_SetChannelSourceTrigger(
            instance->dmaController,
            instance->imageTrxDmaChannel,
            NULL);
    DmaController_SetChannelTransferConfig(
            instance->dmaController,
            instance->imageTrxDmaChannel,
            NULL);
    DmaController_SetAndStartChannelTrasfer(
            instance->dmaController,
            instance->imageTrxDmaChannel,
            NULL,
            image->image,
            instance->outputUartTxBufferAddress,
            image->imageSize);
}

static void CheckUartAcquire(void *context)
{
    RECAST(instance, context, ImageForwardingController_t *);

    if(Uart_Acquire(instance->wifiChipUart))
    {
        TimerPeriodic_Command(&instance->periodicForwardImageTimer, TimerPeriodicCommand_Stop);
        ActuallyForwardImage(instance, instance->image);
    }
}

static void ForwardImageToUart(void *context, void *args)
{
    RECAST(instance, context, ImageForwardingController_t *);
    RECAST(image, args, CameraImage_t *);

    instance->image = image;

    if(Uart_Acquire(instance->wifiChipUart))
    {
        ActuallyForwardImage(instance, instance->image);
    }
    else
    {
        TimerPeriodic_Start(&instance->periodicForwardImageTimer);
    }
}

void ImageForwardingController_ForwardOneImage(ImageForwardingController_t *instance)
{
    if(!instance->busy)
    {
        if(Camera_StartImageCapture(instance->camera))
        {
            instance->busy = true;
            TimerOneShot_Start(&instance->timerToResetModule);
        }
    }
}

static void ResetModule(void *context)
{
    RECAST(instance, context, ImageForwardingController_t *);
    TimerPeriodic_Command(&instance->periodicForwardImageTimer, TimerPeriodicCommand_Stop);
    instance->busy = false;
}

void ImageForwardingController_Init(
        ImageForwardingController_t *instance,
        I_Camera_t *cam,
        I_Uart_t *wifiUart,
        I_DmaController_t *dmaController,
        uint32_t imageTrxDmaChannel,
        void *outputUartTxBufferAddress,
        TimerModule_t *timerModule)
{
    instance->wifiChipUart = wifiUart;
    instance->dmaController = dmaController;
    instance->camera = cam;
    instance->imageTrxDmaChannel = imageTrxDmaChannel;
    instance->outputUartTxBufferAddress = outputUartTxBufferAddress;
    instance->dmaTxDone = false;
    instance->receivedAck = false;
    instance->busy = false;

    Event_Synchronous_Init(&instance->onImgFwdDoneEvent);

    EventSubscriber_Synchronous_Init(&instance->uartByteReceivedSub, InterpretAck, instance);

    TimerPeriodic_Init(
        &instance->periodicForwardImageTimer,
        timerModule,
        5,
        CheckUartAcquire,
        instance);

    TimerOneShot_Init(
        &instance->timerToResetModule,
        timerModule,
        1000,
        ResetModule,
        instance);

    EventSubscriber_Synchronous_Init(
        &instance->dmaTrxDoneSub,
        FlagTrxDone,
        instance);
    Event_Subscribe(
        DmaController_GetOnChannelTransferDoneEvent(dmaController, imageTrxDmaChannel),
        &instance->dmaTrxDoneSub.interface);

    EventSubscriber_Synchronous_Init(
        &instance->imageCaptureDoneSub,
        ForwardImageToUart,
        instance);
    Event_Subscribe(Camera_GetOnImageCaptureDoneEvent(cam), &instance->imageCaptureDoneSub.interface);
}
