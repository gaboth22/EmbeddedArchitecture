#include "CppUTest/TestHarness.h"
#include "TimerModuleTestDouble.h"

extern "C"
{
#include "TimerOneShot.h"
#include "utils.h"
}

static bool callbackWasCalled;

static void Callback(void *context)
{
	IGNORE(context);
	callbackWasCalled = true;
}

TEST_GROUP(TimerOneShotTests)
{
	TimerOneShot_t timerToTest;
	TimerModuleTestDouble timerModuleTestDouble;
	TimerTicks_t expectedExpiry;

	void setup()
	{
		callbackWasCalled = false;
		expectedExpiry = 100;
		TimerOneShot_Init(&timerToTest, timerModuleTestDouble.timerModule, expectedExpiry, Callback, NULL);
		TimerOneShot_Start(&timerToTest);
	}
};

TEST(TimerOneShotTests, TimerShouldCallCallbackAfterTimeExpires)
{
	timerModuleTestDouble.ElapseTicks(99);
	CHECK_FALSE(callbackWasCalled);
	timerModuleTestDouble.ElapseTicks(1);
	CHECK_TRUE(callbackWasCalled);
}

TEST(TimerOneShotTests, CallbackShouldOnlyBeCalledOneTimeAfterTheInitialPeriod)
{
	timerModuleTestDouble.ElapseTicks(99);
	CHECK_FALSE(callbackWasCalled);
	timerModuleTestDouble.ElapseTicks(1);
	CHECK_TRUE(callbackWasCalled);
	callbackWasCalled = false;
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_FALSE(callbackWasCalled);
}

TEST(TimerOneShotTests, CallbackShouldNotBeCalledAfterTimerIsStopped)
{
	timerModuleTestDouble.ElapseTicks(99);
	TimerOneShot_Stop(&timerToTest);
	timerModuleTestDouble.ElapseTicks(1);
	CHECK_FALSE(callbackWasCalled);
}

TEST(TimerOneShotTests, CallbackShouldBeCalledAfterTimerIsStoppedAndRestarted)
{
	timerModuleTestDouble.ElapseTicks(99);
	TimerOneShot_Stop(&timerToTest);
	timerModuleTestDouble.ElapseTicks(1);
	CHECK_FALSE(callbackWasCalled);
	TimerOneShot_Start(&timerToTest);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
}

TEST(TimerOneShotTests, ShouldBeAbleToCallStartAndOrStopSeveralTimesWithoutIssues)
{
	TimerOneShot_Stop(&timerToTest);
	TimerOneShot_Stop(&timerToTest);
	TimerOneShot_Stop(&timerToTest);
	TimerOneShot_Start(&timerToTest);
	TimerOneShot_Start(&timerToTest);
	TimerOneShot_Start(&timerToTest);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
}
