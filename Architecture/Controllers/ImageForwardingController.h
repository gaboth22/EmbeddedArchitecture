#ifndef IMAGEFORWARDINGCONTROLLER_H
#define IMAGEFORWARDINGCONTROLLER_H

#include "I_Uart.h"
#include "I_Camera.h"
#include "I_DmaController.h"
#include "EventSubscriber_Synchronous.h"
#include "Event_Synchronous.h"
#include "types.h"

typedef struct
{
    I_Event_t *onImageCaptureDoneEvent;
    I_Uart_t *wifiChipUart;
    I_DmaController_t *dmaController;
    EventSubscriber_Synchronous_t imageCaptureDoneSub;
    EventSubscriber_Synchronous_t dmaTrxDoneSub;
    Event_Synchronous_t onImgFwdDoneEvent;
    uint32_t imageTrxDmaChannel;
    void *outputUartTxBufferAddress;
    bool dmaTxDone;
} ImageForwardingController_t;

/*
 * Initialize the controller
 * @param instance the object
 * @param camera a camera instance
 * @param wifiUart UART to talk to WIFI chip
 * @param dmaController the DMA controller instance
 * @param imagetTrxDmaChannel DMA channel to transfer image through (UART)
 * @param outputUartTxBufferAddress address of UART TRX buffer for DMA trx
 */
void ImageForwardingController_Init(
        ImageForwardingController_t *instance,
        I_Event_t *onImageCaptureDoneEvent,
        I_Uart_t *wifiUart,
        I_DmaController_t *dmaController,
        uint32_t imageTrxDmaChannel,
        void *outputUartTxBufferAddress);

/*
 * Get the event that fires when an image is forwarded
 */
I_Event_t * ImageForwardingController_GetOnImageForwardedEvent(ImageForwardingController_t *instance);

/*
 * Run method, to be placed in a fast running loop
 * Used to signal state changes
 */
void ImageForwardingController_Run(ImageForwardingController_t *instance);

#endif
