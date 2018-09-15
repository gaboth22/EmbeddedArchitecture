#ifndef I_PWM_H
#define I_PWM_H

#include "I_GpioGroup.h"
#include "types.h"

typedef struct _PwmApi_t PwmApi_t;

typedef struct
{
    const PwmApi_t *api;
} I_Pwm_t;

struct _PwmApi_t
{
    /*
     * Set the duty cycle of a PWM channel 0 to 100;
     */
    void (*SetDutyCycle)(I_Pwm_t *instance, uint8_t dutyCucle);

    /*
     * Change the pin out of which the PWM makes it out
     */
    void (*ChangePortMap)(I_Pwm_t *instance, GpioChannel_t channelToOutputPwm);
};

#define Pwm_SetDutyCycle(_instance, _duty) \
    (_instance)->api->SetDutyCycle(_instance, _duty) \

#define Pwm_ChangePortMap(_instance, _channel) \
    (_instance)->api->ChangePortMap(_instance, _channel) \

#endif
