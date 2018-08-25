#ifndef I_STEPPERMOTOR_H
#define I_STEPPERMOTOR_H

#include "types.h"
#include "I_Event.h"

enum
{
    StepDirection_Forward = 0,
    StepDirection_Backward = 1
};
typedef uint8_t StepDirection_t;

typedef struct _StepperMotorApi_t StepperMotorApi_t;

typedef struct
{
    const StepperMotorApi_t *api;
} I_StepperMotor_t;

struct _StepperMotorApi_t
{
    /*
     * Have stepper motor move a number of final steps
     * @param direction direction to step
     * @param numberOfSteps number of steps to move
     */
    void (*DoStep)(I_StepperMotor_t *instance, StepDirection_t direction, uint16_t numberOfSteps);

    /*
     * Stop motor regardless off remaining steps
     */
    void (*Stop)(I_StepperMotor_t *instance);

    /*
     * Get event generated when stepping is done
     */
    I_Event_t * (*GetOnStepsDoneEvent)(I_StepperMotor_t *instance);

    /*
     * Poll whether stepper is busy
     */
    bool (*IsBusy)(I_StepperMotor_t *instance);
};

#define StepperMotor_DoStep(_instance, _direction, _numberOfSteps) \
        (_instance)->api->DoStep(_instance, _direction, _numberOfSteps) \

#define StepperMotor_Stop(_instance) \
        (_instance)->api->Stop(_instance) \

#define StepperMotor_GetOnStepsDoneEvent(_instance) \
        (_instance)->api->GetOnStepsDoneEvent(_instance) \

#define StepperMotor_IsBusy(_instance) \
        (_instance)->api->IsBusy(_instance) \

#endif
