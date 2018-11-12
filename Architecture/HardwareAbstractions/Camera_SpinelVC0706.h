#ifndef CAMERA_SPINELVC0706_H
#define CAMERA_SPINELVC0706_H

#include "I_Camera.h"
#include "I_Uart.h"
#include "I_Event.h"
#include "I_DmaController.h"
#include "Event_Synchronous.h"
#include "EventSubscriber_Synchronous.h"
#include "TimerOneShot.h"
#include "TimerModule.h"
#include "types.h"

typedef uint8_t CameraState_t;

enum
{
    MaxBufferSize = 10,
};

typedef enum
{
    CameraType_0MP3 = 0,
    CameraType_5MP
} CameraType_t;

typedef struct
{
    I_Camera_t interface;
    I_Uart_t *uart;
    I_DmaController_t *dmaController;
    TimerModule_t *timerModule;
    Event_Synchronous_t onImageCaptureDone;
    EventSubscriber_Synchronous_t uartSub;
    EventSubscriber_Synchronous_t dmaSub;
    TimerOneShot_t waitForCameraTimer;
    void *imageBuffer;
    void *addressOfUartRxBuffer;
    uint8_t receiveBuffer[MaxBufferSize];
    uint8_t bufferIndex;
    uint8_t currentImageLengthHighByte;
    uint8_t currentImageLengthLowByte;
    uint32_t dmaChannel;
    CameraState_t state;
    CameraImage_t image;
    bool busy;
    bool dmaRxDone;
    CameraType_t cameraType;
    TimerOneShot_t resetModuleTimer;
} Camera_SpinelVC0706_t;

/*
 * To be placed in a fast running loop
 * Carries out camera I/O decisions
 */
void Camera_SpinelVC076_Run(Camera_SpinelVC0706_t *instance);

/*
 * Initialize camera
 * @param instance the camera instance
 * @param uart the UART to use for camera communication
 * @param dmaController the DMA controller
 * @param timerModule the timer module
 * @param imageBuffer address to put received image at
 * @param camera type being used
 */
void Camera_SpinelVC076_Init(
    Camera_SpinelVC0706_t *instance,
    I_Uart_t *uart,
    I_DmaController_t *dmaController,
    uint32_t dmaChannel,
    TimerModule_t *timerModule,
    void *addressOfUartRxBuffer,
    void *imageBuffer,
    CameraType_t cameraType);

#endif
