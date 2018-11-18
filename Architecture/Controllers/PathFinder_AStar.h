#ifndef PATHFINDER_ASTAR_H
#define PATHFINDER_ASTAR_H

#include "I_PathFinder.h"
#include "XYCoordinate.h"
#include "Stack.h"
#include "Queue.h"

//enum
//{
//    MaxGridSize = 64,
//    MaxBuffSize = 4096
//};

enum
{
    MaxGridSize = 1,
    MaxBuffSize = 1
};

typedef struct
{
    uint8_t costToMoveFromSrcToThisCell;
    uint8_t costToMoveFromThisCellToDst;
    XYCoordinate_t parentLocation;
} CellWithInfo_t;

typedef struct
{
    I_PathFinder_t interface;
    CellWithInfo_t aStarGridInfo[MaxGridSize][MaxGridSize];
    GridMap_FirstQuadrant5cmCell3m2x3m2_t visited;
    XYCoordinate_t nonVisitedDataBuffer[MaxBuffSize];
    Queue_t nonVisited;
    XYCoordinate_t finalPathDataBuffer[MaxBuffSize];
    Stack_t finalPath;
} PathFinder_AStar_t;

/*
 * Initialize the module
 */
void PathFinder_AStar_Init(PathFinder_AStar_t *instance);

#endif
