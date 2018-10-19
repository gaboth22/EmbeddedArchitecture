#include "CppUTest/TestHarness.h"

extern "C"
{
#include "CoordinateQueue.h"
#include "XYCoordinate.h"
}

TEST_GROUP(CoordinateQueueTests)
{
    CoordinateQueue_t queue;

    void setup()
    {
        CoordinateQueue_Init(&queue);
    }
};

TEST(CoordinateQueueTests, SizeShouldInitializeToZero)
{
    uint8_t size = CoordinateQueue_Size(&queue);
    CHECK_EQUAL(0, size);
    CHECK_TRUE(CoordinateQueue_Empty(&queue));
}

TEST(CoordinateQueueTests, PushShouldWorkCorrectly)
{
    XYCoordinate_t c =
       { 1, 2 };
    CoordinateQueue_Push(&queue, c);
}

TEST(CoordinateQueueTests, PopShouldGetTheRightElement)
{
    XYCoordinate_t toPush =
       { 1, 2 };
    CoordinateQueue_Push(&queue, toPush);
    XYCoordinate_t popped = CoordinateQueue_Pop(&queue);

    CHECK_EQUAL(toPush.x, popped.x);
    CHECK_EQUAL(toPush.y, popped.y);
}

TEST(CoordinateQueueTests, SizeShouldGrowAccordingly)
{
    XYCoordinate_t toPush =
       { 1, 2 };

    for(uint8_t i = 1; i <= 4; i++)
    {
        CoordinateQueue_Push(&queue, toPush);
        CHECK_EQUAL(i, CoordinateQueue_Size(&queue));
    }
}

TEST(CoordinateQueueTests, SizeShouldDecreaseAccordingly)
{
    XYCoordinate_t toPush =
       { 1, 2 };

    for(uint8_t i = 0; i < 4; i++)
    {
        CoordinateQueue_Push(&queue, toPush);
    }

    for(uint8_t i = 3; i >= 0; i--)
    {
        CoordinateQueue_Pop(&queue);
        CHECK_EQUAL(i, CoordinateQueue_Size(&queue));
    }
}
