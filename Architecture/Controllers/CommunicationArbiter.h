#ifndef COMMUNICATIONARBITER_H
#define COMMUNICATIONARBITER_H

#include "I_Camera.h"
#include "I_Event.h"
#include "I_Uart.h"
#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"
#include "EventSubscriber_Synchronous.h"
#include "MapSender.h"
#include "RemoteMotionController.h"
#include "ImageForwardingController.h"
#include "TimerOneShot.h"
#include "TimerModule.h"

typedef struct
{
    I_Uart_t *wifiChipUart;
    RemoteMotionController_t *remoteMotionController;
    ImageForwardingController_t *imageForwardingController;
    MapSender_t *mapSender;
    EventSubscriber_Synchronous_t imageForwardedSub;
    EventSubscriber_Synchronous_t motionAcknowledgedSub;
    EventSubscriber_Synchronous_t sendMapsDoneSub;
    uint8_t state;
    uint8_t ack;
    uint8_t motionAckCount;
    TimerOneShot_t checkStateTimer;
    TimerOneShot_t delayCaptureTimer;
    TimerModule_t *timerModule;
    uint8_t runningState;
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visited;
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blocked;
} CommunicationArbiter_t;

/*
 * Initialize the communication arbiter
 */
void CommunicationArbiter_Init(
    CommunicationArbiter_t *instance,
    RemoteMotionController_t *remoteMotionController,
    ImageForwardingController_t *imageForwardingController,
    MapSender_t *mapSender,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visited,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blocked,
    TimerModule_t *timerModule);

/*
 * Run the arbiter
 */
void CommunicationArbiter_Run(CommunicationArbiter_t *instance);

#endif
