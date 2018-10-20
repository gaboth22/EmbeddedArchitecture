#include "ImageForwardingController.h"
#include "utils.h"

enum
{
    ImageMessageCommand = 0xA1,
    ImageMessageAck = 0xA1
};

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
        Event_Publish(&instance->onImgFwdDoneEvent.interface, NULL);
    }
}

static void ForwardImageToUart(void *context, void *args)
{
    RECAST(instance, context, ImageForwardingController_t *);
    RECAST(image, args, CameraImage_t *);

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

void ImageForwardingController_ForwardOneImage(ImageForwardingController_t *instance)
{
    Camera_StartImageCapture(instance->camera);
}

void ImageForwardingController_ClearState(ImageForwardingController_t *instance)
{
    Camera_ClearState(instance->camera);
    DmaController_ClearState(instance->dmaController);
}

void ImageForwardingController_Init(
        ImageForwardingController_t *instance,
        I_Camera_t *cam,
        I_Uart_t *wifiUart,
        I_DmaController_t *dmaController,
        uint32_t imageTrxDmaChannel,
        void *outputUartTxBufferAddress)
{
    instance->wifiChipUart = wifiUart;
    instance->dmaController = dmaController;
    instance->camera = cam;
    instance->imageTrxDmaChannel = imageTrxDmaChannel;
    instance->outputUartTxBufferAddress = outputUartTxBufferAddress;
    instance->dmaTxDone = false;
    instance->receivedAck = false;

    Event_Synchronous_Init(&instance->onImgFwdDoneEvent);

    EventSubscriber_Synchronous_Init(&instance->uartByteReceivedSub, InterpretAck, instance);

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
