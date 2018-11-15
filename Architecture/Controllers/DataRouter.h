#ifndef DATAROUTER_H
#define DATAROUTER_H

#include "I_DmaController.h"
#include "TimerModule.h"
#include "TimerPeriodic.h"
#include "Event_Synchronous.h"
#include "types.h"
#include "Queue.h"

enum
{
    MaxPacketBufferSize = 256
};

typedef void (*DataRouterDataRoutedCallback_t)(void *context);


typedef enum
{
    DataRouterRoute_GetDataFromWifi = 0,
    DataRouterRoute_SendDataToWifi
} DataRouterRoute_t;

typedef struct
{
    uint16_t dataLength;
    uint8_t dataCheckSum;
    uint8_t *data;
    DataRouterDataRoutedCallback_t dataRouterCallback;
} DataRouterDataPacket_t;

typedef struct
{
    I_DmaController_t *dmaController;
    TimerModule_t *timerModule;
    TimerPeriodic_t dataTrxTimer;
    Queue_t outgoingPacketQueue;
    DataRouterDataPacket_t outgoingQueueBuffer[MaxPacketBufferSize];
    Queue_t incomingPacketQueue;
    DataRouterDataPacket_t incomingQueueBuffer[MaxPacketBufferSize];
} DataRouter_t;

/*
 * Enqueue an outgoing packet
 * @param instance The data router instance
 * @param packet The packet to enqueue
 */
void DataRouter_EnqueuePacket(
    DataRouter_t *instance,
    DataRouterDataPacket_t packet);

void DataRouter_RegisterIncomingPacketHanlder(
        );

/*
 * Initialize the module
 * @param instance The object
 * @param dmaController The DMA controller
 * @param timerModule The system timer module
 */
void DataRouter_Init(
    DataRouter_t *instance,
    I_DmaController_t *dmaController,
    TimerModule_t *timerModule);

#endif
