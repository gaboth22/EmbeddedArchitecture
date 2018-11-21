#ifndef I_MOTORCONTROLLER_H
#define I_MOTORCONTROLLER_H

#include "types.h"

typedef uint16_t MotorEncoderTicks_t;

typedef struct _MotorControllerApi_t MotorControllerApi_t;

typedef struct
{
    const MotorControllerApi_t *api;
} I_MotorController_t;

struct _MotorControllerApi_t
{
    /*
     * Run method, to be placed in a fast running loop
     */
    void (*Run)(I_MotorController_t *instance);

    /*
     * Move Motor controller forward
     */
    void (*Forward)(I_MotorController_t *instance, MotorEncoderTicks_t numberOfTicksToMove);

    /*
     * Move motors to make a right turn
     */
    void (*TurnRight)(I_MotorController_t *instance, MotorEncoderTicks_t numberOfTicksToMove);

    /*
     * Move motors to make a left turn
     */
    void (*TurnLeft)(I_MotorController_t *instance, MotorEncoderTicks_t numberOfTicksToMove);

    /*
     * Move motors to make a left turn
     */
    void (*Backward)(I_MotorController_t *instance, MotorEncoderTicks_t numberOfTicksToMove);

    /*
     * Returns true if the controller is moving the motors, false otherwise
     */
    bool (*Busy)(I_MotorController_t *instance);

    /*
     * Clears the state of the motor controller, so that a new goal can be set safely
     */
    void (*ClearState)(I_MotorController_t *instance);
};

#define MotorController_Run(_instance) \
        (_instance)->api->Run(_instance) \

#define MotorController_Forward(_instance, _numTicks) \
        (_instance)->api->Forward(_instance, _numTicks) \

#define MotorController_TurnRight(_instance, _numTicks) \
        (_instance)->api->TurnRight(_instance, _numTicks) \

#define MotorController_TurnLeft(_instance, _numTicks) \
        (_instance)->api->TurnLeft(_instance, _numTicks) \

#define MotorController_Backward(_instance, _numTicks) \
        (_instance)->api->Backward(_instance, _numTicks) \

#define MotorController_Busy(_instance) \
        (_instance)->api->Busy(_instance) \

#define MotorController_ClearState(_instance) \
        (_instance)->api->ClearState(_instance) \

#endif
