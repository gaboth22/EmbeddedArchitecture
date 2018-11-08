#ifndef SCOUTINGCONTROLLER_H
#define SCOUTINGCONTROLLER_H

#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"
#include "I_MotorController.h"
#include "I_PathFinder.h"
#include "I_WayPointProvider.h"
#include "DistanceProviderCm.h"
#include "Stack.h"
#include "LcdDisplayController.h"
#include "MapBuilder.h"

typedef struct
{
    GridMap_FirstQuadrant5cmCell3m2x3m2_t visitedAreasGrid;
    GridMap_FirstQuadrant5cmCell3m2x3m2_t blockedAreasGrid;
    I_DistanceSensor_t *frontSensor;
    I_DistanceSensor_t *leftSensor;
    I_DistanceSensor_t *rightSensor;
    I_MotorController_t *motorController;
    DistanceProviderCm_t *distanceProvider;
    I_WayPointProvider_t *waypointProvider;
    I_PathFinder_t *pathFinder;
    Stack_t *path;
    DistanceInCm_t runningDistance;
    uint8_t state;
    bool start;
    uint8_t xpos;
    uint8_t ypos;
    uint8_t currentHeading;
    LcdDisplayController_t *lcd;
    MapBuilder_t *mapBuilder;
} ScoutingController_t;

/*
 * Start running the scouting controller, to be called once
 */
void ScoutingController_Start(ScoutingController_t *instance);

/*
 * Run method, to be placed in a fast running loop
 */
void ScoutingController_Run(ScoutingController_t *instance);

/*
 * Initialize the scouting controller
 * @param instance The object
 * @param frontSensor front distance sensor
 * @param leftSensor left distance sensor
 * @param rightSensor right distance sensor
 * @param motorController motor controller instance
 * @param distanceProvider module that provides how much we have moved
 * @param unscoutedWaypointProvider module to that provides the next unscouted point
 * @param pathFinder module that provides the path from a source point to a destination
 */
void ScoutingController_Init(
    ScoutingController_t *instance,
    I_DistanceSensor_t *frontSensor,
    I_DistanceSensor_t *leftSensor,
    I_DistanceSensor_t *rightSensor,
    I_MotorController_t *motorController,
    DistanceProviderCm_t *distanceProvider,
    I_WayPointProvider_t *unscoutedWaypointProvider,
    I_PathFinder_t *pathFinder,
    uint8_t startX,
    uint8_t startY,
    LcdDisplayController_t *lcd,
    MapBuilder_t *mapBuilder);

#endif
