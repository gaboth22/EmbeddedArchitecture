#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include "I_DistanceSensor.h"
#include "DistanceProviderCm.h"
#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"
#include "types.h"
#include "XYCoordinate.h"

typedef struct
{
    DistanceProviderCm_t *distanceProvider;
    I_DistanceSensor_t *rightSensor;
    I_DistanceSensor_t *leftSensor;
} MapBuilder_t;

/*
 * Run the module
 */
XYCoordinate_t MapBuilder_Run(
    MapBuilder_t *instance,
    uint8_t xpos,
    uint8_t ypos,
    uint8_t currentHeading,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visitedAreasGrid,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blockedAreasGrid);

/*
 * Initialize the module
 */
void MapBuilder_Init(
    MapBuilder_t *instance,
    DistanceProviderCm_t *distanceProvider,
    I_DistanceSensor_t *leftSensor,
    I_DistanceSensor_t *rightSensor);

#endif
