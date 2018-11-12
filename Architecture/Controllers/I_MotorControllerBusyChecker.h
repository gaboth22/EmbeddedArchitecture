#ifndef I_MOTORCONTROLLERBUSYCHECKER_H
#define I_MOTORCONTROLLERBUSYCHECKER_H

#include "types.h"

typedef struct _MotorControllerBusyCheckerApi_t MotorControllerBusyCheckerApi_t;

typedef struct
{
    const MotorControllerBusyCheckerApi_t *api;
} I_MotorControllerBusyChecker_t;

struct _MotorControllerBusyCheckerApi_t
{
    /*
     * Check whether the motor controller is operating by monitors the wheel encoders
     */
    bool (*Busy)(I_MotorControllerBusyChecker_t *instance);
};

#define MotorControllerBusyChecker_Busy(_instance) \
    (_instance)->api->Busy(_instance) \

#endif
