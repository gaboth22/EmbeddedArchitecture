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

    /*
     * Start checking if the motors are busy
     */
    void (*StartChecking)(I_MotorControllerBusyChecker_t *instance);

    /*
     * Run method, to be placed in a fast running loop
     */
    void (*Run)(I_MotorControllerBusyChecker_t *instance);
};

#define MotorControllerBusyChecker_Busy(_instance) \
    (_instance)->api->Busy(_instance) \

#define MotorControllerBusyChecker_StartChecking(_instance) \
    (_instance)->api->StartChecking(_instance) \

#define MotorControllerBusyChecker_Run(_instance) \
    (_instance)->api->Run(_instance) \

#endif
