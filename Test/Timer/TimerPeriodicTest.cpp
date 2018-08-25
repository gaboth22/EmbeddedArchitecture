#include "CppUTest/TestHarness.h"
#include "TimerModuleTestDouble.h"

extern "C"
{
#include "TimerPeriodic.h"
#include "utils.h"
}

static bool callbackWasCalled;

static void Callback(void *context)
{
	IGNORE(context);
	callbackWasCalled = true;
}

TEST_GROUP(TimerPeriodicTests)
{
	TimerPeriodic_t timerToTest;
	TimerModuleTestDouble timerModuleTestDouble;
	TimerTicks_t expectedExpiry;

	void setup()
	{
		callbackWasCalled = false;
		expectedExpiry = 100;
		TimerPeriodic_Init(&timerToTest, timerModuleTestDouble.timerModule, expectedExpiry, Callback, NULL);
		TimerPeriodic_Start(&timerToTest);
	}
};

TEST(TimerPeriodicTests, TimerShouldCallCallbackAfterTimeExpires)
{
	timerModuleTestDouble.ElapseTicks(99);
	CHECK_FALSE(callbackWasCalled);
	timerModuleTestDouble.ElapseTicks(1);
	CHECK_TRUE(callbackWasCalled);
}

TEST(TimerPeriodicTests, CallbackShouldBeCalledEveryTimeAfterThePeriodExpires)
{
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
	callbackWasCalled = false;
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
	callbackWasCalled = false;
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
}

TEST(TimerPeriodicTests, CallbackShouldNotBeCalledWhenTimerIsPaused)
{
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
	callbackWasCalled = false;
	TimerPeriodic_Command(&timerToTest, TimerPeriodicCommand_Pause);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_FALSE(callbackWasCalled);
}

TEST(TimerPeriodicTests, CallbackShouldBeCalledAgainWhenTimerResumes)
{
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
	callbackWasCalled = false;
	TimerPeriodic_Command(&timerToTest, TimerPeriodicCommand_Pause);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_FALSE(callbackWasCalled);
	TimerPeriodic_Command(&timerToTest, TimerPeriodicCommand_Resume);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
}

TEST(TimerPeriodicTests, CallbackShouldNotBeCalledAgainAfterTimerIsStopped)
{
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
	callbackWasCalled = false;
	TimerPeriodic_Command(&timerToTest, TimerPeriodicCommand_Stop);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_FALSE(callbackWasCalled);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_FALSE(callbackWasCalled);
}

TEST(TimerPeriodicTests, CallbackShouldBeCalledAgainAfterTimerIsStoppedAndRestarted)
{
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
	callbackWasCalled = false;
	TimerPeriodic_Command(&timerToTest, TimerPeriodicCommand_Stop);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_FALSE(callbackWasCalled);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_FALSE(callbackWasCalled);
	TimerPeriodic_Start(&timerToTest);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
}

TEST(TimerPeriodicTests, ShouldBeAbleToCallStartAndStopSeveralTimesWithoutIssue)
{
	TimerPeriodic_Command(&timerToTest, TimerPeriodicCommand_Stop);
	TimerPeriodic_Command(&timerToTest, TimerPeriodicCommand_Stop);
	TimerPeriodic_Command(&timerToTest, TimerPeriodicCommand_Stop);
	TimerPeriodic_Start(&timerToTest);
	TimerPeriodic_Start(&timerToTest);
	TimerPeriodic_Start(&timerToTest);
	timerModuleTestDouble.ElapseTicks(100);
	CHECK_TRUE(callbackWasCalled);
}
