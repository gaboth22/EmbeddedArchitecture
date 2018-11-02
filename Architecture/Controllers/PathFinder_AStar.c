#include "PathFinder_AStar.h"
#include "utils.h"

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

static CoordinateStack_t * GetPath(
    I_PathFinder_t *_instance,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blockedMap,
    XYCoordinate_t src,
    XYCoordinate_t dst)
{
    RECAST(instance, _instance, PathFinder_AStar_t *);

    Stack_Init(&instance->finalPath, &instance->finalPathDataBuffer[0], sizeof(XYCoordinate_t));
    Queue_Init(&instance->nonVisited, &instance->nonVisitedDataBuffer[0], sizeof(CellWithCost_t));
    GridMap_FirstQuadrant5cmCell3m2x3m2_Init(&instance->visited);
    ResetCellInfo(instance);

    uint8_t x = src.x;
    uint8_t y = src.y;

    instance->aStarGridInfo[x][y].costToMoveFromSrcToThisCell = 0;
    instance->aStarGridInfo[x][y].costToMoveFromThisCellToDst = 0;
    instance->aStarGridInfo[x][y].parentLocation.x = x;
    instance->aStarGridInfo[x][y].parentLocation.y = y;

    CellWithCost_t cell =
        { { x, y }, 0 };
    Queue_Push(&instance->nonVisited, &cell);

    while(!Queue_Empty(&instance->nonVisited))
    {
        Queue_Pop(&instance->nonVisited, &cell);

        x = cell.location.x;
        y = cell.location.y;
        GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(&instance->visited, x, y);
    }

    return &instance->finalPath;
}

static const PathFinderApi_t api =
    { GetPath };

void PathFinder_AStar_Init(PathFinder_AStar_t *instance)
{
    instance->interface.api = &api;
}
