#ifndef MAPSENDER_H
#define MAPSENDER_H

#include "I_Uart.h"
#include "I_DmaController.h"
#include "Event_Synchronous.h"
#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"
#include "EventSubscriber_Synchronous.h"
#include "types.h"
#include "TimerOneShot.h"
#include "TimerPeriodic.h"

typedef struct
{
    I_Uart_t *wifiUart;
    I_DmaController_t *dmaController;
    Event_Synchronous_t onMapSent;
    EventSubscriber_Synchronous_t onDmaTrxDoneSub;
    EventSubscriber_Synchronous_t onByteReceivedSub;
    bool dmaTrxDone;
    bool gotAck;
    bool uartAcquiredAllGood;
    uint32_t dmaChannel;
    void *dmaOutputBufferAddress;
    TimerOneShot_t timerToResetModule;
    TimerPeriodic_t timerToAcquireUart;
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visited;
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blocked;
    bool busy;
} MapSender_t;

/*
 * Run function - to be placed in a fast running loop
 */
void MapSender_Run(MapSender_t *instance);

/*
 * Send maps to computer
 */
void MapSender_SendMaps(
    MapSender_t *instance,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visited,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blocked);

/*
 * Get event that fires when the maps are forwarded through WIFI
 */
I_Event_t * MapSender_GetOnMapsSentEvent(MapSender_t *instance);

/*
 * Initialize the module
 */
void MapSender_Init(
    MapSender_t *instance,
    I_Uart_t *wifiUart,
    I_DmaController_t *dmaController,
    uint32_t dmaChannel,
    void *outputBufferAddress,
    TimerModule_t *timerModule);

#endif
