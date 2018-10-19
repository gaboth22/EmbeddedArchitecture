#ifndef DIRECTIONTURNDATA_H
#define DIRECTIONTURNDATA_H

#include "I_DistanceSensor.h"
#include "types.h"

enum NewDirection
{
    NewDirection_Forward = 0,
    NewDirecton_Right,
    NewDirection_Left
};
typedef uint8_t NewDirection_t;

typedef struct
{
    NewDirection_t direction;
    DistanceInCm_t distanceSinceLastTurn;
} TurnDirection_t;

#endif
