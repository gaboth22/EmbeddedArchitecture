#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include "PidController.h"
#include "types.h"

typedef struct
{
    float kp;
    float ki;
    float kd;
    int64_t pidOutput;
    uint64_t basePidOutput;
    int64_t pidOutputCap;
    int64_t lastError;
    int64_t integral;
} PidController_t;

/*
 * Get the value of one iteration of the PID loop
 * must be placed in the main loop,
 * or a fast-running loop
 */
int64_t PidController_Run(
    PidController_t *instance,
    int64_t currentReading,
    int64_t goal);

/*
 * Clear the state of the PID to start a new loop run
 */
void PidController_ClearState(PidController_t *instance);

/*
 * Returns true when the goal has been achieved
 */
bool PidController_GoalAchieved(PidController_t *instance);

/*
 * Initializes PID controller
 */
void PidController_Init(
    PidController_t *instance,
    float kp,
    float ki,
    float kd,
    int64_t basePidOutput,
    int64_t pidOutputCap);

#endif
