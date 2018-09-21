#include "PidController.h"
#include "utils.h"
#include "types.h"

uint64_t PidController_Run(PidController_t *instance, uint64_t currentReading, uint64_t goal)
{
    instance->pidOutput = 0;
    uint64_t error = goal - currentReading;
    uint64_t derivative = error - instance->lastError;
    instance->integral = instance->integral + error;

    instance->pidOutput = (instance->kp * error) + (instance->ki * instance->integral) + (instance->kd * derivative);

    if(instance->pidOutput > 0)
    {
        instance->pidOutput += instance->basePidOutput;
    }

    if(instance->pidOutput > instance->pidOutputCap)
    {
        instance->pidOutput = instance->pidOutputCap;
    }

    instance->lastError = error;

    return instance->pidOutput;
}

void PidController_ClearState(PidController_t *instance)
{
    instance->lastError = 0;
    instance->integral = 0;
}

bool PidController_GoalAchieved(PidController_t *instance)
{
    return (instance->pidOutput == 0);
}

void PidController_Init(
    PidController_t *instance,
    float kp,
    float ki,
    float kd,
    uint64_t basePidOutput,
    uint64_t pidOutputCap)
{
    instance->kp = kp;
    instance->ki = ki;
    instance->kd = kd;
    instance->basePidOutput = basePidOutput;
    instance->pidOutputCap = pidOutputCap;
    instance->lastError = 0;
    instance->integral = 0;
}
