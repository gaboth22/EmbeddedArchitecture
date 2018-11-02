#ifndef I_PATHFINDER_H
#define I_PATHFINDER_H

#include "CoordinateStack.h"
#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"

typedef struct _PathFinderApi_t PathFinderApi_t;

typedef struct
{
    const PathFinderApi_t *api;
} I_PathFinder_t;

struct _PathFinderApi_t
{
    /*
     * Get a stack containing the waypoints leading from a source to a destination point
     * @param instance the object
     * @param blockedMap map containing which cells are blocked
     * @param src the source point
     * @param dst the destination point
     */
    CoordinateStack_t * (*GetPath)(
        I_PathFinder_t *instance,
        GridMap_FirstQuadrant5cmCell3m2x3m2_t *blockedMap,
        XYCoordinate_t src,
        XYCoordinate_t dst);
};

#define PathFinder_GetPath(_instance, _blockedMap, _src, _dst) \
    (_instance)->api->GetPath(_instance, _blockedMap, _src, _dst) \

#endif
