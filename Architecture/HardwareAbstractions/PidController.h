#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include "PidController.h"
#include "EventSubscriber_Synchronous.h"
#include "I_Event.h"

typedef struct
{
    float kp;
    float ki;
    float kd;
    uint8_t basePidOutput;
    uint8_t pidOutputCap;
} PidController_t;

/*
 * Initializes pid controller
 */
void PidController_Init(
    PidController_t *instance,
    float kp,
    float ki,
    float kd,
    uint8_t basePidOutput,
    uint8_t pidOutputCap);

#endif
