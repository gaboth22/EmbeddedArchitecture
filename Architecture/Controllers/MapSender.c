#include "MapSender.h"

enum
{
    SendMapUartCommand = 0xA3
};

void MapSender_SendMaps(
    MapSender_t *instance,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visited,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blocked)
{
    Uart_SendByte(instance->wifiUart, SendMapUartCommand);

    uint8_t i = 0;
    int8_t j = 0;
    for(i = 0; i < 64; i++)
    {
        uint64_t datum = visited->grid[i];

        for(j = 7; j >= 0; j--)
        {
            Uart_SendByte(instance->wifiUart, *((uint8_t *)&datum + j));
        }
    }

    for(i = 0; i < 64; i++)
    {
        uint64_t datum = blocked->grid[i];

        for(j = 7; j >= 0; j--)
        {
            Uart_SendByte(instance->wifiUart, *((uint8_t *)&datum + j));
        }
    }

    Event_Publish(&instance->onMapSent.interface, NULL);
}

I_Event_t * MapSender_GetOnMapsSentEvent(MapSender_t *instance)
{
    return &instance->onMapSent.interface;
}

void MapSender_Init(MapSender_t *instance, I_Uart_t *wifiUart)
{
    instance->wifiUart = wifiUart;
    Event_Synchronous_Init(&instance->onMapSent);
}
