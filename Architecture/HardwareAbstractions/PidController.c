#include "msp.h"
#include "PidController.h"
#include "utils.h"

static long error;
static long integral;
static long derivative;

int PidController_Run(PidController_t *instance, int goal)
{
    int pidOutput = 0;

    pidOutput = (instance->kp * error) + (instance->ki * integral) + (instance->kd * derivative);

    if(pidOutput < 0)
    {
//      forward = false;
      pidOutput -= instance->basePidOutput;
    }

    if(pidOutput > 0)
    {
//      forward = true;
      pidOutput += instance->basePidOutput;
    }

    if(pidOutput > instance->pidOutputCap)
    {
      pidOutput = 100;
    }

    if(pidOutput < -1*(instance->pidOutputCap))
    {
      pidOutput = -100;
    }

    return pidOutput;
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
}
