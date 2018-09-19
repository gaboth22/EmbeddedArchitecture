#include "msp.h"
#include "PidController.h"
#include "utils.h"
#include "types.h"

uint64_t PidController_Run(PidController_t *instance, uint64_t currentReading, uint64_t goal)
{
    uint64_t pidOutput = 0;
    uint64_t error = goal - currentReading;
    uint64_t derivative = error - instance->lastError;
    instance->integral = instance->integral + error;

    pidOutput = (instance->kp * error) + (instance->ki * instance->integral) + (instance->kd * derivative);

    if(pidOutput < 0)
    {
      pidOutput -= instance->basePidOutput;
    }

    if(pidOutput > 0)
    {
      pidOutput += instance->basePidOutput;
    }

    if(pidOutput > instance->pidOutputCap)
    {
      pidOutput = instance->pidOutputCap;
    }

    if(pidOutput < -(instance->pidOutputCap))
    {
      pidOutput = -(instance->pidOutputCap);
    }

    instance->lastError = instance->error;

    return pidOutput;
}

void PidController_ClearState(PidController_t *instance)
{
    instance->lastError = 0;
    instance->integral = 0;
}

void PidController_Init(
    PidController_t *instance,
    float kp,
    float ki,
    float kd,
    uint8_t basePidOutput,
    uint8_t pidOutputCap)
{
    instance->kp = kp;
    instance->ki = ki;
    instance->kd = kd;
    instance->basePidOutput = basePidOutput;
    instance->pidOutputCap = pidOutputCap;
    instance->lastError = 0;
    instance->integral = 0;
}
