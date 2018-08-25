#ifndef I_ADC_H
#define I_ADC_H

#include "types.h"

typedef uint16_t AdcCounts_t;
typedef struct _AdcApi_t AdcApi_t;

typedef struct
{
    const AdcApi_t *api;
} I_Adc_t;

struct _AdcApi_t
{
	/*
	 * Get the current ADC Counts
	 * @param instance The ADC implementation
	 */
    AdcCounts_t (*GetCounts)(I_Adc_t *instance);
};

#define Adc_GetCounts(_instance) \
    (_instance)->api->GetCounts(_instance) \

#endif
