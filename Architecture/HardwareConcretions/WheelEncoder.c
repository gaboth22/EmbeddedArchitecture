#include "I_Event.h"
#include "WheelEncoder.h"
#include "utils.h"

enum
{
	MsPerSecond = 1000
};

static void IncreaseEnconderTicks(void *context, void *args)
{
	RECAST(instance, context, WheelEncoder_t *);
	IGNORE(args);

	instance->encoderTicks++;
}

static void SampleAngularVelocity(void *context)
{
	RECAST(instance, context, WheelEncoder_t *);

	instance->angularVelocityInRadiansPerSecondTimes10K =
			instance->encoderTicks * instance->radiansPerEncoderHoleOverSamplingPeriodTimes10K;

	instance->encoderTicks = 0;
}

static uint32_t GetAngularVelocityInRadiansPerSecondTimes10K(I_WheelEncoder_t *_instance)
{
	RECAST(instance, _instance, WheelEncoder_t *);

	return instance->angularVelocityInRadiansPerSecondTimes10K;
}

static const WheelEncoderApi_t api =
	{ GetAngularVelocityInRadiansPerSecondTimes10K };

void WheelEncoder_Init(
		WheelEncoder_t *instance,
		TimerModule_t *timerModule,
		I_Interrupt_t *encoderInterrupt,
		uint8_t encoderDiskHoles,
		TimerTicks_t angularVelocitySamplingRateMs)
{
	float twoPi = 6.2831;
	instance->radiansPerEncoderHoleOverSamplingPeriodTimes10K =
			(uint32_t)(((twoPi / encoderDiskHoles) / (angularVelocitySamplingRateMs / MsPerSecond)) * 10000);

	instance->interface.api = &api;
	instance->angularVelocityInRadiansPerSecondTimes10K = 0;
	instance->encoderTicks = 0;

	EventSubscriber_Synchronous_Init(&instance->interruptSubscriber, IncreaseEnconderTicks, instance);
	Event_Subscribe(Interrupt_GetOnInterruptEvent(encoderInterrupt), &instance->interruptSubscriber.interface);

	TimerPeriodic_Init(
			&instance->angularVelocitySamplingTimer,
			timerModule,
			angularVelocitySamplingRateMs,
			SampleAngularVelocity,
			instance);

	TimerPeriodic_Start(&instance->angularVelocitySamplingTimer);
}
