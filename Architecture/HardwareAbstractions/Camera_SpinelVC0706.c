#include <string.h>
#include "Camera_SpinelVC0706.h"
#include "Uassert.h"
#include "utils.h"

enum
{
    WaitForCameraInitMs = 3000,
    SmallImageThreshold = 2048,
    ResetCameraCommandBytesSize = 4,
    GeneralCommandSize = 5,
    GeneralAckSize = 5,
    GetImageLengthAckSize = 9,
    ImageLengthHighByteIndex = 12,
    ImageLengthLowByteIndex = 13,
    GetImageDataCommandSize = 16,
    ExtraBytesForCameraAck = 10
};

enum
{
    CameraState_Uninitialized = 0,
    CameraState_IssueStopCaptureRequest,
    CameraState_StopCaptureRequestIssued,
    CameraState_IssueGetImageRequest,
    CameraState_GetImageRequestIssued,
    CameraState_IssueGetImageLengthRequest,
    CameraState_GetImageLengthRequestIssued,
    CameraState_ReadImageDataRequestIssued,
    CameraState_IssueImageDataTransferStartRequest,
    CameraState_ImageDataTransferRequestIssued
};

static const uint8_t SetResolutionTo160x120CommandBytes[] =
    { 0x56, 0x00, 0x54, 0x01, 0x22 };

static const uint8_t GetImageCommandBytes[] =
    { 0x56, 0x00, 0x36, 0x01, 0x00 };

static const uint8_t GetImageAckBytes[] =
    { 0x76, 0x00, 0x36, 0x00, 0x00 };

static const uint8_t GetImageLengthCommandBytes[] =
    { 0x56, 0x00, 0x34, 0x01, 0x00 };

static const uint8_t GetImageLengthAckBytes[] =
    { 0x76, 0x00, 0x34, 0x00, 0x04, 0x00, 0x00 };

static const uint8_t StopImageCaptureCommand[] =
    { 0x56, 0x00, 0x36, 0x01, 0x03 };

static const uint8_t StopImageCaptureAckBytes[] =
    { 0x076, 0x00, 0x36, 0x00, 0x00 };

static uint8_t ReadImageDataCommandBytes[] =
    { 0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A };

static void StartImageCapture(I_Camera_t *instance)
{
    RECAST(cam, instance, Camera_SpinelVC0706_t *);

    if(!cam->busy)
    {
        cam->busy = true;
        cam->bufferIndex = 0;
        Uart_EnableRx(cam->uart);
        cam->state = CameraState_IssueStopCaptureRequest;
    }
}

static I_Event_t * GetOnImageCaptureDoneEvent(I_Camera_t *instance)
{
    RECAST(cam, instance, Camera_SpinelVC0706_t *);
    return &cam->onImageCaptureDone.interface;
}

static uint16_t GetConcatenatedImageLength(uint8_t highByte, uint8_t lowByte)
{
    return (((0x00FF & highByte) << 8) | lowByte);
}

static void ReceiveByte(void *context, void *args)
{
    RECAST(cam, context, Camera_SpinelVC0706_t *);
    RECAST(byte, args, uint8_t *);

    cam->receiveBuffer[cam->bufferIndex] = *byte;
    cam->bufferIndex++;
    cam->bufferIndex = cam->bufferIndex % MaxBufferSize; // Wrap around if it grows past max size
}

static void DmaRxDoneCallback(void *context, void *args)
{
    IGNORE(args);
    RECAST(cam, context, Camera_SpinelVC0706_t *);
    cam->dmaRxDone = true;
}

static void MarkCamAsNotBusy(void *context)
{
    RECAST(cam, context, Camera_SpinelVC0706_t *);
    cam->busy = false;
}

static void SetResolution(void *context)
{
    RECAST(cam, context, Camera_SpinelVC0706_t *);

    uint8_t i = 0;
    for(i = 0; i < GeneralCommandSize; i++)
    {
        Uart_SendByte(cam->uart, SetResolutionTo160x120CommandBytes[i]);
    }

    TimerOneShot_Init(
            &cam->waitForCameraTimer,
            cam->timerModule,
            200,
            MarkCamAsNotBusy,
            cam);
    TimerOneShot_Start(&cam->waitForCameraTimer);
}

static const CameraApi_t api =
    { StartImageCapture, GetOnImageCaptureDoneEvent };

void Camera_SpinelVC076_Init(
    Camera_SpinelVC0706_t *instance,
    I_Uart_t *uart,
    I_DmaController_t *dmaController,
    uint32_t dmaChannel,
    TimerModule_t *timerModule,
    void *addressOfUartRxBuffer,
    void *imageBuffer)
{
    instance->busy = true;
    instance->uart = uart;
    instance->timerModule = timerModule;
    instance->interface.api = &api;
    instance->bufferIndex = 0;
    instance->currentImageLengthHighByte = 0;
    instance->currentImageLengthLowByte = 0;
    instance->dmaController = dmaController;
    instance->imageBuffer = imageBuffer;
    instance->addressOfUartRxBuffer = addressOfUartRxBuffer;
    instance->dmaChannel = dmaChannel;
    instance->state = CameraState_Uninitialized;

    EventSubscriber_Synchronous_Init(&instance->uartSub, ReceiveByte, instance);
    Event_Subscribe(Uart_GetOnByteReceivedEvent(uart), &instance->uartSub.interface);

    EventSubscriber_Synchronous_Init(&instance->dmaSub, DmaRxDoneCallback, instance);
    Event_Subscribe(
        DmaController_GetOnChannelTransferDoneEvent(instance->dmaController, instance->dmaChannel),
        &instance->dmaSub.interface);

    Event_Synchronous_Init(&instance->onImageCaptureDone);

    TimerOneShot_Init(&instance->waitForCameraTimer, timerModule, WaitForCameraInitMs, SetResolution, instance);
    TimerOneShot_Start(&instance->waitForCameraTimer);
}

void Camera_SpinelVC076_Run(Camera_SpinelVC0706_t *instance)
{
    uint8_t i = 0;

    switch(instance->state)
    {
        case CameraState_IssueStopCaptureRequest:
        {
            for(i = 0; i < GeneralCommandSize; i++)
            {
                Uart_SendByte(instance->uart, StopImageCaptureCommand[i]);
            }

            instance->state = CameraState_StopCaptureRequestIssued;
            break;
        }

        case CameraState_StopCaptureRequestIssued:
        {
            if(GeneralAckSize == instance->bufferIndex)
            {
                Uassert(0 == memcmp(&StopImageCaptureAckBytes[0], &instance->receiveBuffer[0], GeneralAckSize));
                instance->bufferIndex = 0;
                instance->state = CameraState_IssueGetImageRequest;
            }
            break;
        }

        case CameraState_IssueGetImageRequest:
        {
            for(i = 0; i < GeneralCommandSize; i++)
            {
                Uart_SendByte(instance->uart, GetImageCommandBytes[i]);
            }

            instance->state = CameraState_GetImageRequestIssued;
            break;
        }

        case CameraState_GetImageRequestIssued:
        {
            if(GeneralAckSize == instance->bufferIndex)
            {
                Uassert(0 == memcmp(&GetImageAckBytes[0], &instance->receiveBuffer[0], GeneralAckSize));
                instance->bufferIndex = 0;
                instance->state = CameraState_IssueGetImageLengthRequest;
            }
            break;
        }

        case CameraState_IssueGetImageLengthRequest:
        {
            for(i = 0; i < GeneralCommandSize; i++)
            {
                Uart_SendByte(instance->uart, GetImageLengthCommandBytes[i]);
            }

            instance->state = CameraState_GetImageLengthRequestIssued;
            break;
        }

        case CameraState_GetImageLengthRequestIssued:
        {
            if(GetImageLengthAckSize == instance->bufferIndex)
            {
                Uassert(0 == memcmp(&GetImageLengthAckBytes[0], &instance->receiveBuffer[0], GetImageLengthAckSize - 2));
                instance->currentImageLengthHighByte = instance->receiveBuffer[instance->bufferIndex - 2];
                instance->currentImageLengthLowByte = instance->receiveBuffer[instance->bufferIndex - 1];

                uint16_t imageLength = GetConcatenatedImageLength(
                    instance->currentImageLengthHighByte,
                    instance->currentImageLengthLowByte);

                instance->bufferIndex = 0;

                if(imageLength < SmallImageThreshold)
                {
                    instance->state = CameraState_IssueGetImageLengthRequest;
                }
                else
                {
                    Uart_DisableRx(instance->uart);
                    instance->state = CameraState_IssueImageDataTransferStartRequest;
                }
            }
            break;
        }

        case CameraState_IssueImageDataTransferStartRequest:
        {
            instance->dmaRxDone = false;
            instance->state = CameraState_ImageDataTransferRequestIssued;
            DmaController_SetChannelSourceTrigger(instance->dmaController, instance->dmaChannel, NULL);
            DmaController_SetChannelTransferConfig(instance->dmaController, instance->dmaChannel, NULL);

            DmaController_SetAndStartChannelTrasfer(
                    instance->dmaController,
                    instance->dmaChannel,
                    NULL,
                    instance->addressOfUartRxBuffer,
                    instance->imageBuffer,
                    GetConcatenatedImageLength(
                        instance->currentImageLengthHighByte,
                        instance->currentImageLengthLowByte) + ExtraBytesForCameraAck);

            ReadImageDataCommandBytes[ImageLengthHighByteIndex] = instance->currentImageLengthHighByte;
            ReadImageDataCommandBytes[ImageLengthLowByteIndex] = instance->currentImageLengthLowByte;

            for(i = 0; i < GetImageDataCommandSize; i++)
            {
                Uart_SendByte(instance->uart, ReadImageDataCommandBytes[i]);
            }
            break;
        }

        case CameraState_ImageDataTransferRequestIssued:
            break;

        default:
            break;
    }

    if(instance->dmaRxDone)
    {
        instance->dmaRxDone = false;
        instance->image.image = instance->imageBuffer;
        instance->image.imageSize = GetConcatenatedImageLength(
            instance->currentImageLengthHighByte,
            instance->currentImageLengthLowByte) + ExtraBytesForCameraAck;
        Event_Publish(&instance->onImageCaptureDone.interface, &instance->image);
    }
}
