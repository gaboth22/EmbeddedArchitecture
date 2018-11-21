#ifndef IMAGERECOGNITIONCONTROLLER_H
#define IMAGERECOGNITIONCONTROLLER_H

#include "I_Uart.h"
#include "EventSubscriber_Synchronous.h"
#include "Event_Synchronous.h"
#include "TimerOneShot.h"
#include "TimerPeriodic.h"
#include "types.h"

typedef struct
{
    bool alreadySent;
    TimerOneShot_t timerToResetModule;
    TimerPeriodic_t timerToAcquireUart;
    I_Uart_t *wifiUart;
    bool uartAcquiredAllgood;
    bool done;
    Event_Synchronous_t onImageRecognized;
    EventSubscriber_Synchronous_t onByteReceivedSub;
    uint8_t xpos;
    uint8_t ypos;
} ImageRecognitionController_t;

/*
 * Request recognition of the current image
 */
void ImageRecognitionController_RequestRecognition(
    ImageRecognitionController_t *instance,
    uint8_t xpos,
    uint8_t ypos);

/*
 * Returns true when the image recognition cycle is completed
 */
I_Event_t * ImageRecognitionController_GetOnImageRecognizedEvent(
    ImageRecognitionController_t *instance);

/*
 * Run method, to be placed in a loop
 */
void ImageRecognitionController_Run(ImageRecognitionController_t *instance);

/*
 * Initialize module
 */
void ImageRecognitionController_Init(
    ImageRecognitionController_t *instance,
    I_Uart_t *wifiUart,
    TimerModule_t *timerModule);

#endif
