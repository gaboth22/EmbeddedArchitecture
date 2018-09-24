#include "ImageForwardingController.h"
#include "utils.h"

#include "msp.h"

enum
{
    ImageMessageCommand = 0xA1
};

static void FlagTrxDone(void *context, void *args)
{
    IGNORE(args);
    RECAST(instance, context, ImageForwardingController_t *);

    instance->dmaTxDone = true;
}

I_Event_t * GetOnImageForwardedEvent(ImageForwardingController_t *instance)
{
    return &instance->onImgFwdDoneEvent.interface;
}

void ImageForwardingController_Run(ImageForwardingController_t *instance)
{
    if(instance->dmaTxDone)
    {
        instance->dmaTxDone = false;
        Event_Publish(&instance->onImgFwdDoneEvent.interface, NULL);
    }
}

static void ForwardImageToUart(void *context, void *args)
{
    RECAST(instance, context, ImageForwardingController_t *);
    RECAST(image, args, CameraImage_t *);

    //Uart_EnableRx(instance->wifiChipUart);

    Uart_SendByte(instance->wifiChipUart, ImageMessageCommand);
    uint16_t imgSizeHighByte = ((image->imageSize >> 8) & 0x00FF);
    uint16_t imgSizeLowByte = (image->imageSize & 0x00FF);
    Uart_SendByte(instance->wifiChipUart, imgSizeHighByte);
    Uart_SendByte(instance->wifiChipUart, imgSizeLowByte);


    //Uart_DisableRx(instance->wifiChipUart);
    UCA3IFG |= EUSCI_A_IFG_TXIFG;
    instance->dmaTxDone = false;

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

void ImageForwardingController_Init(
        ImageForwardingController_t *instance,
        I_Event_t *onImageCaptureDoneEvent,
        I_Uart_t *wifiUart,
        I_DmaController_t *dmaController,
        uint32_t imageTrxDmaChannel,
        void *outputUartTxBufferAddress)
{
    instance->wifiChipUart = wifiUart;
    instance->dmaController = dmaController;
    instance->imageTrxDmaChannel = imageTrxDmaChannel;
    instance->outputUartTxBufferAddress = outputUartTxBufferAddress;
    instance->dmaTxDone = false;

    Event_Synchronous_Init(&instance->onImgFwdDoneEvent);

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
    Event_Subscribe(onImageCaptureDoneEvent, &instance->imageCaptureDoneSub.interface);
}
