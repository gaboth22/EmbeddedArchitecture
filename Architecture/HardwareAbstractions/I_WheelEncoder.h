#ifndef I_WHEELENCODER_H
#define I_WHEELENCODER_H

#include "types.h"

typedef struct _WheelEncoderApi_t WheelEncoderApi_t;

typedef struct
{
	const WheelEncoderApi_t *api;
} I_WheelEncoder_t;

struct _WheelEncoderApi_t
{
	/*
	 * Gets the angular velocity of a wheel through the wheel encoder
	 * and the return value is the angular velocity multiplied times 10,000.
	 *
	 * @param instance The implementation of the interface
	 */
	uint32_t (*GetAngularVelocityInRadiansPerSecondTimes10K)(I_WheelEncoder_t *instance);
};

#define WheelEncoder_GetAngularVelocityInRadiansPerSecondTimes10K(_instance) \
        (_instance)->api->GetAngularVelocityInRadiansPerSecondTimes10K(_instance) \

#endif
