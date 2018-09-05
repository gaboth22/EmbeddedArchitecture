#include "CppUTest/TestHarness.h"
#include "TimerModuleTestDouble.h"
#include "InterruptTestDouble.h"

extern "C"
{
#include <stdlib.h>
#include "WheelEncoder.h"
#include "types.h"
}

enum
{
	MsPerSecond = 1000
};

TEST_GROUP(WheelEncoderTests)
{
	WheelEncoder_t wheelEncoder;
	TimerModuleTestDouble timerModuleTestDouble;
	InterruptTestDouble interruptTestDouble;
	uint8_t encoderDiskHoles = 20;
	TimerTicks_t samplingPeriod;
	uint32_t radiansPerEncoderHoleOverSamplingPeriodTimes10K;

	void setup()
	{
		samplingPeriod = 50;
		float twoPi = 6.2831;
		radiansPerEncoderHoleOverSamplingPeriodTimes10K =
				(uint32_t)(((twoPi / encoderDiskHoles) / (samplingPeriod / MsPerSecond)) * 10000);
		WheelEncoder_Init(
				&wheelEncoder,
				timerModuleTestDouble.timerModule,
				&interruptTestDouble.interrupt,
				encoderDiskHoles,
				samplingPeriod);
	}
};

TEST(WheelEncoderTests, ShouldGetZeroRadiansPerSecondWhenInterruptHasntPublished)
{
	uint32_t angularFreq = WheelEncoder_GetAngularVelocityInRadiansPerSecondTimes10K(&wheelEncoder.interface);
	CHECK_EQUAL(0, angularFreq);
}

TEST(WheelEncoderTests, ShoulStillReturnZeroBeforeTheFirstSamplingPeriodElapses)
{
	interruptTestDouble.GenerateInterrupt(10);
	timerModuleTestDouble.ElapseTicks(samplingPeriod - 1);
	uint32_t angularFreq = WheelEncoder_GetAngularVelocityInRadiansPerSecondTimes10K(&wheelEncoder.interface);
	CHECK_EQUAL(0, angularFreq);
}

TEST(WheelEncoderTests, ShouldGetTheAngularVelocityAfterASamplingPeriod)
{
	int numberEncoderReadings = 10;
	interruptTestDouble.GenerateInterrupt(numberEncoderReadings);
	timerModuleTestDouble.ElapseTicks(samplingPeriod - 1);
	uint32_t angularFreq = WheelEncoder_GetAngularVelocityInRadiansPerSecondTimes10K(&wheelEncoder.interface);
	CHECK_EQUAL(0, angularFreq);
	timerModuleTestDouble.ElapseTicks(1);
	angularFreq = WheelEncoder_GetAngularVelocityInRadiansPerSecondTimes10K(&wheelEncoder.interface);
	CHECK_EQUAL(numberEncoderReadings * radiansPerEncoderHoleOverSamplingPeriodTimes10K , angularFreq);
}

TEST(WheelEncoderTests, ShouldReportZeroVelocityIfNoInterruptsAreRaisedAfterASamplingPeriod)
{
	int numberEncoderReadings = 10;
	interruptTestDouble.GenerateInterrupt(numberEncoderReadings);
	timerModuleTestDouble.ElapseTicks(samplingPeriod);
	uint32_t angularFreq = WheelEncoder_GetAngularVelocityInRadiansPerSecondTimes10K(&wheelEncoder.interface);
	CHECK_EQUAL(numberEncoderReadings * radiansPerEncoderHoleOverSamplingPeriodTimes10K , angularFreq);
	timerModuleTestDouble.ElapseTicks(samplingPeriod - 1);
	angularFreq = WheelEncoder_GetAngularVelocityInRadiansPerSecondTimes10K(&wheelEncoder.interface);
	CHECK_EQUAL(numberEncoderReadings * radiansPerEncoderHoleOverSamplingPeriodTimes10K , angularFreq);
	timerModuleTestDouble.ElapseTicks(1);
	angularFreq = WheelEncoder_GetAngularVelocityInRadiansPerSecondTimes10K(&wheelEncoder.interface);
	CHECK_EQUAL(0 , angularFreq);
}
