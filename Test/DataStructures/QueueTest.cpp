#include "CppUTest/TestHarness.h"

extern "C"
{
#include "Queue.h"
#include "XYCoordinate.h"
}

TEST_GROUP(QueueTests)
{
    Queue_t intQueue;
    uint8_t intDataBuffer[255] = { 0 };

    Queue_t xyCoordQueue;
    XYCoordinate_t xyCoorBuffer[255];

    void setup()
    {
        Queue_Init(&intQueue, &intDataBuffer[0], sizeof(uint8_t));
        Queue_Init(&xyCoordQueue, &xyCoorBuffer[0], sizeof(XYCoordinate_t));
    }
};

TEST(QueueTests, PushShouldWorkJustFine)
{
    uint8_t toPush = 10;
    Queue_Push(&intQueue, &toPush);
}

TEST(QueueTests, PopShouldGetTheRightStuff)
{
    uint8_t pushed = 6;
    Queue_Push(&intQueue, &pushed);
    uint8_t popped = 0;
    Queue_Pop(&intQueue, &popped);

    CHECK_EQUAL(pushed, popped);
}

TEST(QueueTests, PopShouldGetTheRightStuffAfterSeveralPushes)
{
    uint8_t pushed1 = 6;
    uint8_t pushed2 = 7;
    uint8_t pushed3 = 8;

    Queue_Push(&intQueue, &pushed1);
    Queue_Push(&intQueue, &pushed2);
    Queue_Push(&intQueue, &pushed3);

    uint8_t popped = 0;

    Queue_Pop(&intQueue, &popped);
    CHECK_EQUAL(pushed1, popped);
    Queue_Pop(&intQueue, &popped);
    CHECK_EQUAL(pushed2, popped);
    Queue_Pop(&intQueue, &popped);
    CHECK_EQUAL(pushed3, popped);
}

TEST(QueueTests, PushShouldWorkJustFineWithAnotherType)
{
    XYCoordinate_t toPush = { 10, 0 };
    Queue_Push(&xyCoordQueue, &toPush);
}

TEST(QueueTests, PopShouldGetTheRightStuffWithAnotherType)
{
    XYCoordinate_t pushed = { 6, 10 };
    Queue_Push(&xyCoordQueue, &pushed);
    XYCoordinate_t popped = { 0, 0 };
    Queue_Pop(&xyCoordQueue, &popped);

    MEMCMP_EQUAL(&pushed, &popped, sizeof(XYCoordinate_t));
}

TEST(QueueTests, PopShouldGetTheRightStuffWithAnotherTypeAfterSeveralPushes)
{
    XYCoordinate_t pushed1 = { 10, 11 };
    XYCoordinate_t pushed2 = { 12, 13 };
    XYCoordinate_t pushed3 = { 14, 15 };

    Queue_Push(&xyCoordQueue, &pushed1);
    Queue_Push(&xyCoordQueue, &pushed2);
    Queue_Push(&xyCoordQueue, &pushed3);

    XYCoordinate_t popped = { 0, 0 };

    Queue_Pop(&xyCoordQueue, &popped);
    MEMCMP_EQUAL(&pushed1, &popped, sizeof(XYCoordinate_t));
    Queue_Pop(&xyCoordQueue, &popped);
    MEMCMP_EQUAL(&pushed2, &popped, sizeof(XYCoordinate_t));
    Queue_Pop(&xyCoordQueue, &popped);
    MEMCMP_EQUAL(&pushed3, &popped, sizeof(XYCoordinate_t));
}

TEST(QueueTests, SizeShouldAdaptAccordingly)
{
    CHECK_TRUE(Queue_Empty(&intQueue));

    for(uint8_t i = 0; i < 10; i++)
    {
        CHECK_EQUAL(i, Queue_Size(&intQueue));
        Queue_Push(&intQueue, &i);
    }

    uint8_t popped;
    for(uint8_t i = 10; i > 0; i--)
    {
        CHECK_EQUAL(i, Queue_Size(&intQueue));
        Queue_Pop(&intQueue, &popped);
    }

    CHECK_TRUE(Queue_Empty(&intQueue));
}
