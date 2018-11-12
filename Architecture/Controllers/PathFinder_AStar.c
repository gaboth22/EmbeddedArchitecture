#include <string.h>
#include "PathFinder_AStar.h"
#include "utils.h"
#include "Queue.h"

enum
{
    NonVisitedCell = 0,
    NonBlockedCell = 0,
    MemContentsAreEqual = 0
};

static void ResetCellInfo(PathFinder_AStar_t *instance)
{
    uint8_t i, j;
    for(i = 0 ; i < MaxGridSize; i++)
    {
        for(j = 0; j < MaxGridSize; j++)
        {
            instance->aStarGridInfo[i][j].costToMoveFromSrcToThisCell = UINT8_MAX;
            instance->aStarGridInfo[i][j].costToMoveFromThisCellToDst = UINT8_MAX;
            instance->aStarGridInfo[i][j].parentLocation.x = UINT8_MAX;
            instance->aStarGridInfo[i][j].parentLocation.y = UINT8_MAX;
        }
    }
}

static void RebuildPathWithStraightLinesAndOnlyStartAndEndingPoints(PathFinder_AStar_t *instance)
{
    Stack_t stack;
    XYCoordinate_t queueBuffer[255];
    Stack_Init(&stack, &queueBuffer[0], sizeof(XYCoordinate_t));

    XYCoordinate_t current;
    XYCoordinate_t next;
    Stack_Pop(&instance->finalPath, &current);

    while(!Stack_Empty(&instance->finalPath))
    {
        Stack_Pop(&instance->finalPath, &next);

        if(current.x == next.x)
        {
            while(!Stack_Empty(&instance->finalPath) && current.x == next.x)
            {
                Stack_Pop(&instance->finalPath, &next);
            }

            XYCoordinate_t turningPoint = { current.x, next.y };
            Stack_Push(&stack, &turningPoint);
            if(MemContentsAreEqual != memcmp(&turningPoint, &next, sizeof(XYCoordinate_t)))
            {
                Stack_Push(&stack, &next);
            }

            current = next;
        }
        else if(current.y == next.y)
        {
            while(!Stack_Empty(&instance->finalPath) && current.y == next.y)
            {
                Stack_Pop(&instance->finalPath, &next);
            }

            XYCoordinate_t turningPoint = { next.x, current.y };
            Stack_Push(&stack, &turningPoint);
            if(MemContentsAreEqual != memcmp(&turningPoint, &next, sizeof(XYCoordinate_t)))
            {
                Stack_Push(&stack, &next);
            }

            current = next;
        }
    }

    while(!Stack_Empty(&stack))
    {
        XYCoordinate_t wayPoint;
        Stack_Pop(&stack, &wayPoint);
        Stack_Push(&instance->finalPath, &wayPoint);
    }
}

static void BuildCompletePath(PathFinder_AStar_t *instance, XYCoordinate_t dst)
{
    uint8_t x = dst.x;
    uint8_t y = dst.y;
    XYCoordinate_t parent;

    while(!(instance->aStarGridInfo[x][y].parentLocation.x == x &&
            instance->aStarGridInfo[x][y].parentLocation.y == y))
    {
        XYCoordinate_t c =
            { x, y };
        Stack_Push(&instance->finalPath, &c);
        parent = instance->aStarGridInfo[x][y].parentLocation;
        x = parent.x;
        y = parent.y;
    }
}

static bool DestinationFound(uint8_t x, uint8_t y, XYCoordinate_t dst)
{
    return dst.x == x && dst.y == y;
}

static uint8_t GetCostToMoveFromThisCellToTheDestination(uint8_t x, uint8_t y, XYCoordinate_t dst)
{
    /*
     * Using Manhattan distance as the heuristic, since we only move in four directions
     */
    return TRUNCATE_U8_ADDITION(abs(x - dst.x), abs(y - dst.y));
}

static Stack_t * GetPath(
    I_PathFinder_t *_instance,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blockedMap,
    XYCoordinate_t src,
    XYCoordinate_t dst)
{
    RECAST(instance, _instance, PathFinder_AStar_t *);

    Stack_Init(&instance->finalPath, &instance->finalPathDataBuffer[0], sizeof(XYCoordinate_t));
    Queue_Init(&instance->nonVisited, &instance->nonVisitedDataBuffer[0], sizeof(XYCoordinate_t));
    GridMap_FirstQuadrant5cmCell3m2x3m2_Init(&instance->visited);
    ResetCellInfo(instance);

    uint8_t x = src.x;
    uint8_t y = src.y;

    instance->aStarGridInfo[x][y].costToMoveFromSrcToThisCell = 0;
    instance->aStarGridInfo[x][y].costToMoveFromThisCellToDst = 0;
    instance->aStarGridInfo[x][y].parentLocation.x = x;
    instance->aStarGridInfo[x][y].parentLocation.y = y;

    XYCoordinate_t cell = { x, y };
    Queue_Push(&instance->nonVisited, &cell);

    while(!Queue_Empty(&instance->nonVisited))
    {
        Queue_Pop(&instance->nonVisited, &cell);

        x = cell.x;
        y = cell.y;
        GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(&instance->visited, x, y);

        uint8_t adjacents[8];
        GridMap_FirstQuadrant5cmCell3m2x3m2_GetIndicesOfAdjacents(&instance->visited, x, y, &adjacents[0]);

        uint8_t i;
        for(i = 1; i < 8; i += 2)
        {
            uint8_t currX, currY;
            currX = adjacents[i-1];
            currY = adjacents[i];

            if(currX == UINT8_MAX || currY == UINT8_MAX)
            {
                continue;
            }

            if(NonBlockedCell == GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(blockedMap, currX, currY) &&
               DestinationFound(currX, currY, dst))
            {
                instance->aStarGridInfo[currX][currY].parentLocation.x = x;
                instance->aStarGridInfo[currX][currY].parentLocation.y = y;
                BuildCompletePath(instance, dst);
                RebuildPathWithStraightLinesAndOnlyStartAndEndingPoints(instance);
                return &instance->finalPath;
            }
            else if(NonVisitedCell == GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(&instance->visited, currX, currY) &&
                    NonBlockedCell == GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(blockedMap, currX, currY))
            {
                // Let it overflow since it defaults to UINT8_MAX
                uint8_t newCostFromSrcToThiscell =
                    instance->aStarGridInfo[currX][currY].costToMoveFromSrcToThisCell + 1;
                uint8_t newCostFromThisCellToDst =
                    GetCostToMoveFromThisCellToTheDestination(currX, currY, dst);
                uint8_t newTotalCostOfCell =
                    TRUNCATE_U8_ADDITION(newCostFromSrcToThiscell, newCostFromThisCellToDst);
                uint8_t currentCostOfCell =
                    TRUNCATE_U8_ADDITION(
                        instance->aStarGridInfo[currX][currY].costToMoveFromSrcToThisCell,
                        instance->aStarGridInfo[currX][currY].costToMoveFromThisCellToDst);

                if(newTotalCostOfCell < currentCostOfCell)
                {
                    XYCoordinate_t newCell = {currX, currY};
                    Queue_Push(&instance->nonVisited, &newCell);

                    instance->aStarGridInfo[currX][currY].costToMoveFromSrcToThisCell =
                        newCostFromSrcToThiscell;
                    instance->aStarGridInfo[currX][currY].costToMoveFromThisCellToDst =
                        newCostFromThisCellToDst;
                    instance->aStarGridInfo[currX][currY].parentLocation.x = x;
                    instance->aStarGridInfo[currX][currY].parentLocation.y = y;
                }
            }
        }
    }

    // Clear stack so we return empty stack if the path is not found
    Stack_Init(&instance->finalPath, &instance->finalPathDataBuffer[0], sizeof(XYCoordinate_t));
    return &instance->finalPath;
}

static const PathFinderApi_t api =
    { GetPath };

void PathFinder_AStar_Init(PathFinder_AStar_t *instance)
{
    instance->interface.api = &api;
}
