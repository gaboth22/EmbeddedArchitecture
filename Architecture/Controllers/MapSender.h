#ifndef MAPSENDER_H
#define MAPSENDER_H

#include "I_Uart.h"
#include "Event_Synchronous.h"
#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"

typedef struct
{
    I_Uart_t *wifiUart;
    Event_Synchronous_t onMapSent;
} MapSender_t;

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
void MapSender_Init(MapSender_t *instance, I_Uart_t *wifiUart);

#endif
