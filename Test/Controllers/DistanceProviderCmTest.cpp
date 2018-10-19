#include "CppUTest/TestHarness.h"

extern "C"
{
#include "DistanceProviderCm.h"
#include "Event_Synchronous.h"
#include "types.h"
}

enum
{
    TickCountRepresenting5Cm = 10,
    TickCountRepresensting1Cm = 2
};

TEST_GROUP(DistanceProviderCmTests)
{
    DistanceProviderCm_t distanceProvider;
    Event_Synchronous_t onEncoderTick;

    void setup()
    {
        Event_Synchronous_Init(&onEncoderTick);

        DistanceProviderCm_Init(
            &distanceProvider,
            &onEncoderTick.interface,
            TickCountRepresenting5Cm,
            TickCountRepresensting1Cm);
    }

    void GivenTheseManyTicksPublish(uint16_t ticks)
    {
        for(int i = 0; i < ticks; i++)
        {
            Event_Publish(&onEncoderTick.interface, NULL);
        }
    }

    void ThenTheDistanceInCmShouldBe(DistanceInCm_t expectedDistance)
    {
        DistanceInCm_t actualDistace = DistanceProviderCm_GetDistance(&distanceProvider);
        CHECK_EQUAL(expectedDistance, actualDistace);
    }
};

TEST(DistanceProviderCmTests, ShouldReturnTheRightDistance)
{
    DistanceProviderCm_EnableDistanceTracking(&distanceProvider);
    GivenTheseManyTicksPublish(2 * TickCountRepresenting5Cm);
    ThenTheDistanceInCmShouldBe(10);
}

TEST(DistanceProviderCmTests, ShouldKeepOnAddingToTheDistanceWhenMoreTicksHappen)
{
    DistanceProviderCm_EnableDistanceTracking(&distanceProvider);
    GivenTheseManyTicksPublish(2 * TickCountRepresenting5Cm);
    ThenTheDistanceInCmShouldBe(10);
    GivenTheseManyTicksPublish(4 * TickCountRepresenting5Cm);
    ThenTheDistanceInCmShouldBe(30);
}

TEST(DistanceProviderCmTests, ShouldClearTheDistanceWhenInstructedToDoSo)
{
    DistanceProviderCm_EnableDistanceTracking(&distanceProvider);
    GivenTheseManyTicksPublish(2 * TickCountRepresenting5Cm);
    ThenTheDistanceInCmShouldBe(10);
    DistanceProviderCm_ClearDistance(&distanceProvider);
    ThenTheDistanceInCmShouldBe(0);
}

TEST(DistanceProviderCmTests, ShouldNotKeepDistanceWhenItIsDisabled)
{
    DistanceProviderCm_EnableDistanceTracking(&distanceProvider);
    GivenTheseManyTicksPublish(2 * TickCountRepresenting5Cm);
    ThenTheDistanceInCmShouldBe(10);
    DistanceProviderCm_DisableDistanceTracking(&distanceProvider);
    GivenTheseManyTicksPublish(10 * TickCountRepresenting5Cm);
    ThenTheDistanceInCmShouldBe(10);
}

TEST(DistanceProviderCmTests, ShouldStartToKeepDistanceAgainAfterBeingDisabledAndReenabled)
{
    DistanceProviderCm_EnableDistanceTracking(&distanceProvider);
    GivenTheseManyTicksPublish(2 * TickCountRepresenting5Cm);
    ThenTheDistanceInCmShouldBe(10);
    DistanceProviderCm_DisableDistanceTracking(&distanceProvider);
    GivenTheseManyTicksPublish(10 * TickCountRepresenting5Cm);
    ThenTheDistanceInCmShouldBe(10);
    DistanceProviderCm_EnableDistanceTracking(&distanceProvider);
    GivenTheseManyTicksPublish(2 * TickCountRepresenting5Cm);
    ThenTheDistanceInCmShouldBe(20);
}
