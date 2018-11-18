#include "WayPointProvider_Simple.h"
#include "utils.h"

enum
{
    Left = 1,
    Right = 2,
    ThreshHoldToDecideToTurnToThatSideCm = 40
};

static XYCoordinate_t GetWayPoint(
    I_WayPointProvider_t *_instance,
    XYCoordinate_t position,
    Heading_t heading)
{
    RECAST(instance, _instance, WayPointProvider_Simple_t *);

    uint8_t turn = 0;
    XYCoordinate_t wayPoint = { UINT8_MAX, UINT8_MAX };

    if(DistanceSensor_GetDistanceInCm(instance->left) > ThreshHoldToDecideToTurnToThatSideCm)
    {
        turn = Left;
    }

    if(DistanceSensor_GetDistanceInCm(instance->right) > ThreshHoldToDecideToTurnToThatSideCm)
    {
        turn = Right;
    }

    switch(heading)
    {
        case Heading_N:
            if(Right == turn)
            {
                wayPoint.x = position.x + 1;
                wayPoint.y = position.y;
            }
            else if(Left == turn)
            {
                wayPoint.x = position.x - 1;
                wayPoint.y = position.y;
            }
            break;

        case Heading_S:
            if(Right == turn)
            {
                wayPoint.x = position.x - 1;
                wayPoint.y = position.y;
            }
            else if(Left == turn)
            {
                wayPoint.x = position.x + 1;
                wayPoint.y = position.y;
            }
            break;

        case Heading_W:
            if(Right == turn)
            {
                wayPoint.x = position.x;
                wayPoint.y = position.y + 1;
            }
            else if(Left == turn)
            {
                wayPoint.x = position.x;
                wayPoint.y = position.y - 1;
            }
            break;

        case Heading_E:
            if(Right == turn)
            {
                wayPoint.x = position.x;
                wayPoint.y = position.y - 1;
            }
            else if(Left == turn)
            {
                wayPoint.x = position.x;
                wayPoint.y = position.y + 1;
            }
            break;

        default:
            break;
    }

    return wayPoint;
}

static const WayPointProviderApi_t api =
    { GetWayPoint };

void WayPointProvider_Simple(
    WayPointProvider_Simple_t *instance,
    I_DistanceSensor_t *left,
    I_DistanceSensor_t *front,
    I_DistanceSensor_t *right)
{
    instance->interface.api = &api;
    instance->left = left;
    instance->front = front;
    instance->right = right;
}
