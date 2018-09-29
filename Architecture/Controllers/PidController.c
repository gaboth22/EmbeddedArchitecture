#include "PidController.h"
#include "utils.h"
#include "types.h"

int64_t PidController_Run(PidController_t *instance, int64_t currentReading, int64_t goal)
{
    instance->pidOutput = 0;

    int64_t error = (goal - currentReading);
    int64_t derivative = (error - instance->lastError);
    instance->integral = instance->integral + error;

    instance->pidOutput = (instance->kp * error) + (instance->ki * instance->integral) + (instance->kd * derivative);

    if(instance->pidOutput > 0)
    {
        instance->pidOutput += instance->basePidOutput;
    }

    if(instance->pidOutput < 0)
    {
        instance->pidOutput -= instance->basePidOutput;
    }

    if(instance->pidOutput > instance->pidOutputCap)
    {
        instance->pidOutput = instance->pidOutputCap;
    }

    if(instance->pidOutput < -1 * instance->pidOutputCap)
    {
        instance->pidOutput = -1 * instance->pidOutputCap;
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
    int64_t pidOutputCap)
{
    instance->kp = kp;
    instance->ki = ki;
    instance->kd = kd;
    instance->basePidOutput = basePidOutput;
    instance->pidOutputCap = pidOutputCap;
    instance->lastError = 0;
    instance->integral = 0;
}
