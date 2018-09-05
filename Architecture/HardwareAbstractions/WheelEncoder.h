#ifndef WHEELENCODER_H
#define WHEELENCODER_H

#include "I_WheelEncoder.h"
#include "I_Interrupt.h"
#include "EventSubscriber_Synchronous.h"
#include "TimerPeriodic.h"

typedef struct
{
	I_WheelEncoder_t interface;
	EventSubscriber_Synchronous_t interruptSubscriber;
	uint32_t encoderTicks;
	uint32_t angularVelocityInRadiansPerSecondTimes10K;
	TimerPeriodic_t angularVelocitySamplingTimer;
	uint32_t radiansPerEncoderHoleOverSamplingPeriodTimes10K;
} WheelEncoder_t;


/*
 * Initializes an instance of a wheel encoder
 * @param instance The object
 * @param encoderInterrupt Interrupt generated when there is an encoder reading
 */
void WheelEncoder_Init(
		WheelEncoder_t *instance,
		TimerModule_t *timerModule,
		I_Interrupt_t *encoderInterrupt,
		uint8_t encoderDiskHoles,
		TimerTicks_t angularVelocitySamplingRateMs);

#endif
