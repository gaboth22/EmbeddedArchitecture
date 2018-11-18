#ifndef I_WAYPOINTPROVIDER_H
#define I_WAYPOINTPROVIDER_H

#include "types.h"
#include "XYCoordinate.h"

typedef enum
{
    Heading_N = 0,
    Heading_S = 1,
    Heading_W = 2,
    Heading_E = 3,
} Heading_t;

typedef struct _WayPointProviderApi_t WayPointProviderApi_t;

typedef struct
{
    const WayPointProviderApi_t *api;
} I_WayPointProvider_t;

struct _WayPointProviderApi_t
{
    XYCoordinate_t (*GetWayPoint)(
        I_WayPointProvider_t *instance,
        XYCoordinate_t currentPosition,
        Heading_t heading);
};

#define WayPointProvider_GetWayPoint(_instance, _currentPos, _heading) \
    (_instance)->api->GetWayPoint(_instance, _currentPos, _heading) \

#endif
