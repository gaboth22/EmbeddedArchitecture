#ifndef COMMUNICATIONARBITER_H
#define COMMUNICATIONARBITER_H

#include "I_Camera.h"
#include "I_Event.h"
#include "I_Uart.h"
#include "EventSubscriber_Synchronous.h"
#include "RemoteMotionController.h"
#include "ImageForwardingController.h"
#include "TimerOneShot.h"
#include "TimerModule.h"

typedef struct
{
    I_Uart_t *wifiChipUart;
    RemoteMotionController_t *remoteMotionController;
    ImageForwardingController_t *imageForwardingController;
    EventSubscriber_Synchronous_t imageForwardedSub;
    EventSubscriber_Synchronous_t motionAcknowledgedSub;
    uint8_t state;
    uint8_t ack;
    uint8_t motionAckCount;
    TimerOneShot_t checkStateTimer;
    TimerOneShot_t delayCaptureTimer;
    TimerModule_t *timerModule;
} CommunicationArbiter_t;

/*
 * Initialize the communication arbiter
 */
void CommunicationArbiter_Init(
    CommunicationArbiter_t *instance,
    RemoteMotionController_t *remoteMotionController,
    ImageForwardingController_t *imageForwardingController,
    TimerModule_t *timerModule);

/*
 * Run the arbiter
 */
void CommunicationArbiter_Run(CommunicationArbiter_t *instance);

#endif
