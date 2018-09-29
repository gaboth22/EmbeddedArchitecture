#ifndef COMMUNICATIONARBITER_H
#define COMMUNICATIONARBITER_H

#include "I_Camera.h"
#include "I_Event.h"
#include "I_Uart.h"
#include "EventSubscriber_Synchronous.h"

typedef struct
{
    I_Camera_t *camera;
    I_Uart_t *wifiChipUart;
    EventSubscriber_Synchronous_t imageTrxDoneSub;
    EventSubscriber_Synchronous_t uartByteReceivedSub;
    uint8_t state;
    uint8_t ack;
} CommunicationArbiter_t;

/*
 * Initialize the communication arbiter
 */
void CommunicationArbiter_Init(CommunicationArbiter_t *instance, I_Camera_t *camera, I_Uart_t *wifiChipUart, I_Event_t *onImageTrxDone);

/*
 * Run the arbiter
 */
void CommunicationArbiter_Run(CommunicationArbiter_t *instance);

#endif
