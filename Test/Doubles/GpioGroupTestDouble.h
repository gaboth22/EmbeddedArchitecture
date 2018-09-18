#ifndef GPIOGROUPTESTDOUBLE_H
#define GPIOGROUPTESTDOUBLE_H

extern "C"
{
#include "I_GpioGroup.h"
#include "types.h"
}

typedef struct
{
    I_GpioGroup_t interface;
    uint8_t *gpioStateMap;
} GpioGroupTestDouble_t;

/*
 * Initialize the test double
 * @param instance The test double to initialize
 * @param mappedGpioSize The number of mapped gpio
 */
void GpioGroupTestDouble_Init(
        GpioGroupTestDouble_t *instance,
        uint8_t mappedGpioSize);

#endif
