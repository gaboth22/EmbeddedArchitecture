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
    uint64_t basePidOutput;
    uint64_t pidOutputCap;
    uint64_t lastError;
    uint64_t integral;
} PidController_t;

/*
 * Get the value of one iteration of the PID loop
 * must be placed in the main loop,
 * or a fast-running loop
 */
uint64_t PidController_Run(
        PidController_t *instance,
        uint64_t currentReading,
        uint64_t goal);

/*
 * Clear the state of the PID to start a new loop run
 */
void PidController_ClearState(PidController_t *instance);

/*
 * Initializes PID controller
 */
void PidController_Init(
    PidController_t *instance,
    float kp,
    float ki,
    float kd,
    uint8_t basePidOutput,
    uint8_t pidOutputCap);

#endif
