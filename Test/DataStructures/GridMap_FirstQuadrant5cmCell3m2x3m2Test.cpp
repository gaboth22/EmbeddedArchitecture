#include "CppUTest/TestHarness.h"

extern "C"
{
#include "GridMap_FirstQuadrant5cmCell3m2x3m2.h"
#include "types.h"
#include "Uassert.h"
}

extern bool uassertAsserted;

TEST_GROUP(GridMapTests)
{
    GridMap_FirstQuadrant5cmCell3m2x3m2_t grid;

    void setup()
    {
        uassertAsserted = false;
        GridMap_FirstQuadrant5cmCell3m2x3m2_Init(&grid);
    }
};

TEST(GridMapTests, AllValuesInTheGidShouldBeInitializedToZero)
{
    for(uint8_t i = 0; i < 64; i++)
    {
        CHECK_EQUAL(0, grid.grid[i]);
    }
}

TEST(GridMapTests, SettingValueAtIndexShouldWork)
{
    GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(&grid, 0, 0);

    uint64_t expected = 1L << 63;
    CHECK_EQUAL(expected, grid.grid[0]);
}

TEST(GridMapTests, SettingARandomIndexShouldWork)
{
    GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(&grid, 10, 60);

    uint64_t expected = 0x8000000000000000 >> 10;
    CHECK_EQUAL(expected, grid.grid[60]);
}

TEST(GridMapTests, ShouldBeAbleToRetrieveOneValueAfterItBeingSet)
{
    GridMap_FirstQuadrant5cmCell3m2x3m2_SetCellValueAtIndex(&grid, 10, 60);
    uint8_t value = GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(&grid, 10, 60);
    CHECK_EQUAL(1, value);

    GridMap_FirstQuadrant5cmCell3m2x3m2_ClearCellValueAtIndex(&grid, 10, 60);
    value = GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(&grid, 10, 60);
    CHECK_EQUAL(0, value);
}

TEST(GridMapTests, ShouldLeadToAssertionFailureIfYIndexHigherThan63ArePassedToGet)
{
    GridMap_FirstQuadrant5cmCell3m2x3m2_GetCellValueAtIndex(&grid, 1, 64);
    CHECK_TRUE(uassertAsserted);
}

TEST(GridMapTests, ShouldReturnCoordinatesOfAdjacentsCorrectly)
{
    uint8_t indices[4][2] = { 0 };
    GridMap_FirstQuadrant5cmCell3m2x3m2_GetIndicesOfAdjacents(&grid, 10, 10, &indices[0][0]);

    /* N */
    CHECK_EQUAL(10, indices[0][0]);
    CHECK_EQUAL(11, indices[0][1]);

    /* W */
    CHECK_EQUAL(9, indices[1][0]);
    CHECK_EQUAL(10, indices[1][1]);

    /* S */
    CHECK_EQUAL(10, indices[2][0]);
    CHECK_EQUAL(9, indices[2][1]);

    /* E */
    CHECK_EQUAL(11, indices[3][0]);
    CHECK_EQUAL(10, indices[3][1]);
}
