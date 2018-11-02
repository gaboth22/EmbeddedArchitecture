#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C"
{
#include <string.h>
#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"
#include "I_WayPointProvider.h"
#include "WayPointProvider_Unscouted.h"
#include "types.h"
}

TEST_GROUP(WayPointProvider_UnscoutedTests)
{
    WayPointProvider_Unscouted_t wayPointProvider;
    GridMap_FirstQuadrant5cmCell3m2x3m2_t visited;
    GridMap_FirstQuadrant5cmCell3m2x3m2_t blocked;

    void setup()
    {
        UnscoutedHeadingProvider_Init(&wayPointProvider, &visited, &blocked);
    }

    XYCoordinate_t GetWayPointBasedOnCurrentMapsAndcurrentPos(uint64_t *vis, uint64_t *blk, XYCoordinate_t curPos)
    {
        memcpy((void *)&visited.grid[0], (const void *)vis, (size_t)(64 * sizeof(uint64_t)));
        memcpy((void *)&blocked.grid[0], (const void *)blk, (size_t)(64 * sizeof(uint64_t)));

        return WayPointProvider_GetWayPoint(&wayPointProvider.interface, curPos);
    }
};

TEST(WayPointProvider_UnscoutedTests, ShouldGetTheRightWayPointWhenTheOnlyPossibilityIsARightTurn)
{
    uint64_t blockedMapWithTurn[] =
        {
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0100000000000000000000000000000000000000000000000000000000000000,
          0b0111000000000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    uint64_t visitedMap[] =
        {
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0000000000000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    XYCoordinate_t currentPos = { 2, 3 };
    XYCoordinate_t generatedHeading = GetWayPointBasedOnCurrentMapsAndcurrentPos(&visitedMap[0], &blockedMapWithTurn[0], currentPos);
    XYCoordinate_t expectedWayPoint = { 3, 3 };
    MEMCMP_EQUAL(&expectedWayPoint, &generatedHeading, sizeof(XYCoordinate_t));
}

TEST(WayPointProvider_UnscoutedTests, ShouldGetTheRightWayPointWhenTheOnlyPossibilityIsALeftTurn)
{
    uint64_t blockedMapWithTurn[] =
        {
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0001000000000000000000000000000000000000000000000000000000000000,
          0b0111000000000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    uint64_t visitedMap[] =
        {
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0000000000000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    XYCoordinate_t currentPos = { 2, 3 };
    XYCoordinate_t generatedHeading = GetWayPointBasedOnCurrentMapsAndcurrentPos(&visitedMap[0], &blockedMapWithTurn[0], currentPos);
    XYCoordinate_t expectedWayPoint = { 1, 3 };
    MEMCMP_EQUAL(&expectedWayPoint, &generatedHeading, sizeof(XYCoordinate_t));
}

TEST(WayPointProvider_UnscoutedTests, ShouldGetTheRightWayPointWhenTheOnlyPossibilityIsAUTurn)
{
    uint64_t blockedMapWithTurn[] =
        {
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0111000000000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    uint64_t visitedMap[] =
        {
          0b0000000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0000000000000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    XYCoordinate_t currentPos = { 2, 3 };
    XYCoordinate_t generatedHeading = GetWayPointBasedOnCurrentMapsAndcurrentPos(&visitedMap[0], &blockedMapWithTurn[0], currentPos);
    XYCoordinate_t expectedWayPoint = { 2, 0 };
    MEMCMP_EQUAL(&expectedWayPoint, &generatedHeading, sizeof(XYCoordinate_t));
}

TEST(WayPointProvider_UnscoutedTests, ShouldGetTheRightWayPointWhenItRequiresTurningAfterGoingBack)
{
    uint64_t blockedMapWithTurn[] =
        {
          0b0111000000000000000000000000000000000000000000000000000000000000,
          0b0100000000000000000000000000000000000000000000000000000000000000,
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0111000000000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    uint64_t visitedMap[] =
        {
          0b0000000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0010000000000000000000000000000000000000000000000000000000000000,
          0b0000000000000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    XYCoordinate_t currentPos = { 2, 3 };
    XYCoordinate_t generatedHeading = GetWayPointBasedOnCurrentMapsAndcurrentPos(&visitedMap[0], &blockedMapWithTurn[0], currentPos);
    XYCoordinate_t expectedWayPoint = { 3, 1 };
    MEMCMP_EQUAL(&expectedWayPoint, &generatedHeading, sizeof(XYCoordinate_t));
}

TEST(WayPointProvider_UnscoutedTests, ShouldGetTheRightWayPointWithASeeminglyComplicatedPath)
{
    uint64_t blockedMapWithTurn[] =
        {
          0b0111111100000000000000000000000000000000000000000000000000000000,
          0b0100000100000000000000000000000000000000000000000000000000000000,
          0b0101110110000000000000000000000000000000000000000000000000000000,
          0b0101010000000000000000000000000000000000000000000000000000000000,
          0b0111011110000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    uint64_t visitedMap[] =
        {
          0b0000000000000000000000000000000000000000000000000000000000000000,
          0b0011111000000000000000000000000000000000000000000000000000000000,
          0b0010001000000000000000000000000000000000000000000000000000000000,
          0b0010001000000000000000000000000000000000000000000000000000000000,
          0b0000000000000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    XYCoordinate_t currentPos = { 2, 3 };
    XYCoordinate_t generatedHeading = GetWayPointBasedOnCurrentMapsAndcurrentPos(&visitedMap[0], &blockedMapWithTurn[0], currentPos);
    XYCoordinate_t expectedWayPoint = { 7, 3 };
    MEMCMP_EQUAL(&expectedWayPoint, &generatedHeading, sizeof(XYCoordinate_t));
}
