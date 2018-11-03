#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C"
{
#include <stdio.h>
#include <string.h>
#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"
#include "I_PathFinder.h"
#include "PathFinder_AStar.h"
#include "Queue.h"
#include "types.h"
}

TEST_GROUP(PathFinder_AStartTests)
{
    PathFinder_AStar_t pathFinder;
    GridMap_FirstQuadrant5cmCell3m2x3m2_t blocked;

    void setup()
    {
        PathFinder_AStar_Init(&pathFinder);
    }

    void PopulateBlockedMap(uint64_t *blk)
    {
        memcpy((void *)&blocked.grid[0], (const void *)blk, (size_t)(64 * sizeof(uint64_t)));
    }

    void CheckThatFinalPathIncludesTheExpectedWaypoints(Stack_t *finalPath, XYCoordinate_t *expectedList, uint8_t size)
    {
        CHECK_EQUAL(size, Stack_Size(finalPath));

        uint8_t i = 0;
        while(!Stack_Empty(finalPath))
        {
            XYCoordinate_t actual;
            XYCoordinate_t expected = *(expectedList + i);

            Stack_Pop(finalPath, &actual);
            MEMCMP_EQUAL(&expected, &actual, sizeof(XYCoordinate_t));
            i++;
        }
    }

    void PrintPath(Stack_t *finalPath)
    {
        printf("\nConstructed Path");
        while(!Stack_Empty(finalPath))
        {
            XYCoordinate_t waypoint;
            Stack_Pop(finalPath, &waypoint);
            printf("\n(%d, %d)", waypoint.x, waypoint.y);
        }
    }
};

TEST(PathFinder_AStartTests, ShouldFindPathWhenThereIsOnlyOne)
{
    uint64_t blockedMap[] =
        {
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0101000000000000000000000000000000000000000000000000000000000000,
          0b0101111110000000000000000000000000000000000000000000000000000000,
          0b0100000000000000000000000000000000000000000000000000000000000000,
          0b0111111110000000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    PopulateBlockedMap(&blockedMap[0]);
    XYCoordinate_t src = { 2, 0 };
    XYCoordinate_t dst = { 8, 3 };
    Stack_t *finalPath = PathFinder_GetPath(&pathFinder.interface, &blocked, src, dst);

    uint8_t size = 3;
    XYCoordinate_t expectedPath[] =
        { { 2, 3 }, { 3, 3 }, { 8, 3 } };

    CheckThatFinalPathIncludesTheExpectedWaypoints(finalPath, &expectedPath[0], size);
}

TEST(PathFinder_AStartTests, ShouldFindPathInAnyDirection)
{
    uint64_t blockedMap[] =
        {
          0b0101000000101000000000000000000000000000000000000000000000000000,
          0b0101000000101000000000000000000000000000000000000000000000000000,
          0b0101111111101000000000000000000000000000000000000000000000000000,
          0b0100000000001000000000000000000000000000000000000000000000000000,
          0b0111111111111000000000000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    PopulateBlockedMap(&blockedMap[0]);
    XYCoordinate_t src = { 11, 0 };
    XYCoordinate_t dst = { 2, 2 };
    Stack_t *finalPath = PathFinder_GetPath(&pathFinder.interface, &blocked, src, dst);

    uint8_t size = 4;
    XYCoordinate_t expectedPath[] =
        { { 11, 3 }, { 10, 3 }, { 2, 3 }, { 2, 2 } };

    CheckThatFinalPathIncludesTheExpectedWaypoints(finalPath, &expectedPath[0], size);
}

TEST(PathFinder_AStartTests, ShouldBeAbleToFindAMoreComplicatedPath)
{
    uint64_t blockedMap[] =
        {
          0b0101000000111111110000000000000000000000000000000000000000000000,
          0b0101000000100000010000000000000000000000000000000000000000000000,
          0b0101111111101111011110000000000000000000000000000000000000000000,
          0b0100000000001001000000000000000000000000000000000000000000000000,
          0b0111111111111001111110000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    PopulateBlockedMap(&blockedMap[0]);
    XYCoordinate_t src = { 2, 0 };
    XYCoordinate_t dst = { 16, 1 };
    Stack_t *finalPath = PathFinder_GetPath(&pathFinder.interface, &blocked, src, dst);

    uint8_t size = 7;
    XYCoordinate_t expectedPath[] =
        { { 2, 3 }, { 3, 3 }, { 11, 3 }, { 11, 2 }, { 11, 1 }, { 12, 1 }, { 16, 1 } };

    CheckThatFinalPathIncludesTheExpectedWaypoints(finalPath, &expectedPath[0], size);
}

TEST(PathFinder_AStartTests, ShouldReturnEmptyPathWhenDestinationIsNotAttainableBecauseItsBlocked)
{
    uint64_t blockedMap[] =
        {
          0b0101000000111111110000000000000000000000000000000000000000000000,
          0b0101000000100000010000000000000000000000000000000000000000000000,
          0b0101111111101111011110000000000000000000000000000000000000000000,
          0b0100000000001001000000000000000000000000000000000000000000000000,
          0b0111111111111001111110000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    PopulateBlockedMap(&blockedMap[0]);
    XYCoordinate_t src = { 2, 0 };
    XYCoordinate_t dst = { 3, 2 }; // Blocked location
    Stack_t *finalPath = PathFinder_GetPath(&pathFinder.interface, &blocked, src, dst);

    uint8_t size = 0;
    XYCoordinate_t expectedPath[] = { { 0, 0 } };

    CheckThatFinalPathIncludesTheExpectedWaypoints(finalPath, &expectedPath[0], size);
}

TEST(PathFinder_AStartTests, PathShouldContainOnlyOneLocationIfWithinAStraightLine)
{
    uint64_t blockedMap[] =
        {
          0b0101000000111111110000000000000000000000000000000000000000000000,
          0b0101000000100000010000000000000000000000000000000000000000000000,
          0b0101111111101111011110000000000000000000000000000000000000000000,
          0b0100000000001001000000000000000000000000000000000000000000000000,
          0b0111111111111001111110000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    PopulateBlockedMap(&blockedMap[0]);
    XYCoordinate_t src = { 2, 3 };
    XYCoordinate_t dst = { 11, 3 }; // Blocked location
    Stack_t *finalPath = PathFinder_GetPath(&pathFinder.interface, &blocked, src, dst);

    uint8_t size = 1;
    XYCoordinate_t expectedPath[] = { { 11, 3 } };

    CheckThatFinalPathIncludesTheExpectedWaypoints(finalPath, &expectedPath[0], size);
}

TEST(PathFinder_AStartTests, ShouldReturnEmptyPathWhenDestinationIsOutsideOfGatedArea)
{
    uint64_t blockedMap[] =
        {
          0b0101000000111111110000000000000000000000000000000000000000000000,
          0b0101000000100000010000000000000000000000000000000000000000000000,
          0b0101111111101111011110000000000000000000000000000000000000000000,
          0b0100000000001001000010000000000000000000000000000000000000000000,
          0b0111111111111001111110000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    PopulateBlockedMap(&blockedMap[0]);
    XYCoordinate_t src = { 2, 3 };
    XYCoordinate_t dst = { 18, 0 }; // Blocked location
    Stack_t *finalPath = PathFinder_GetPath(&pathFinder.interface, &blocked, src, dst);

    uint8_t size = 0;
    XYCoordinate_t expectedPath[] = { { 0, 0 } };

    CheckThatFinalPathIncludesTheExpectedWaypoints(finalPath, &expectedPath[0], size);
}

TEST(PathFinder_AStartTests, ShouldReturnEmptyPathWhenDestinationIsOutOfBounds)
{
    uint64_t blockedMap[] =
        {
          0b0101000000111111110000000000000000000000000000000000000000000000,
          0b0101000000100000010000000000000000000000000000000000000000000000,
          0b0101111111101111011110000000000000000000000000000000000000000000,
          0b0100000000001001000000000000000000000000000000000000000000000000,
          0b0111111111111001111110000000000000000000000000000000000000000000,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

    PopulateBlockedMap(&blockedMap[0]);
    XYCoordinate_t src = { 2, 3 };
    XYCoordinate_t dst = { 64, 0 }; // Blocked location
    Stack_t *finalPath = PathFinder_GetPath(&pathFinder.interface, &blocked, src, dst);

    uint8_t size = 0;
    XYCoordinate_t expectedPath[] = { { 0, 0 } };

    CheckThatFinalPathIncludesTheExpectedWaypoints(finalPath, &expectedPath[0], size);
}
