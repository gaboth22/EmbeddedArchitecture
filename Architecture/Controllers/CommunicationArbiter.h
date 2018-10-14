#ifndef COMMUNICATIONARBITER_H
#define COMMUNICATIONARBITER_H

#include "I_Camera.h"
#include "I_Event.h"
#include "I_Uart.h"
#include "EventSubscriber_Synchronous.h"
#include "RemoteMotionController.h"
#include "TimerPeriodic.h"
#include "TimerModule.h"

typedef struct
{
    I_Camera_t *camera;
    I_Uart_t *wifiChipUart;
    RemoteMotionController_t *remoteMotionController;
    EventSubscriber_Synchronous_t imageTrxDoneSub;
    EventSubscriber_Synchronous_t uartByteReceivedSub;
    uint8_t state;
    uint8_t ack;
    uint8_t motionAckCount;
    TimerPeriodic_t checkStateTimer;
    TimerModule_t *timerModule;
    bool startCheck;
} CommunicationArbiter_t;

/*
 * Initialize the communication arbiter
 */
void CommunicationArbiter_Init(
    CommunicationArbiter_t *instance,
    I_Camera_t *camera,
    I_Uart_t *wifiChipUart,
    I_Event_t *onImageForwarded,
    RemoteMotionController_t *remoteMotionController,
    TimerModule_t *timerModule);

/*
 * Run the arbiter
 */
void CommunicationArbiter_Run(CommunicationArbiter_t *instance);

#endif
