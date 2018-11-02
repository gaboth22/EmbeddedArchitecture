#include "CppUTest/TestHarness.h"

extern "C"
{
#include "Stack.h"
#include "XYCoordinate.h"
}

TEST_GROUP(StackTests)
{
    Stack_t intStack;
    uint8_t intDataBuffer[255] = { 0 };

    Stack_t xyCoordStack;
    XYCoordinate_t xyCoordBuffer[255];

    void setup()
    {
        Stack_Init(&intStack, &intDataBuffer[0], sizeof(uint8_t));
        Stack_Init(&xyCoordStack, &xyCoordBuffer[0], sizeof(XYCoordinate_t));
    }
};

TEST(StackTests, PushShouldWorkJustFine)
{
    uint8_t toPush = 10;
    Stack_Push(&intStack, &toPush);
}

TEST(StackTests, PopShouldGetTheRightStuff)
{
    uint8_t pushed = 6;
    Stack_Push(&intStack, &pushed);
    uint8_t popped = 0;
    Stack_Pop(&intStack, &popped);

    CHECK_EQUAL(pushed, popped);
}

TEST(StackTests, PopShouldGetTheRightStuffAfterSeveralPushes)
{
    uint8_t pushed1 = 6;
    uint8_t pushed2 = 7;
    uint8_t pushed3 = 8;

    Stack_Push(&intStack, &pushed1);
    Stack_Push(&intStack, &pushed2);
    Stack_Push(&intStack, &pushed3);

    uint8_t popped = 0;

    Stack_Pop(&intStack, &popped);
    CHECK_EQUAL(pushed3, popped);
    Stack_Pop(&intStack, &popped);
    CHECK_EQUAL(pushed2, popped);
    Stack_Pop(&intStack, &popped);
    CHECK_EQUAL(pushed1, popped);
}

TEST(StackTests, PushShouldWorkJustFineWithAnotherType)
{
    XYCoordinate_t toPush = { 10, 0 };
    Stack_Push(&xyCoordStack, &toPush);
}

TEST(StackTests, PopShouldGetTheRightStuffWithAnotherType)
{
    XYCoordinate_t pushed = { 6, 10 };
    Stack_Push(&xyCoordStack, &pushed);
    XYCoordinate_t popped = { 0, 0 };
    Stack_Pop(&xyCoordStack, &popped);

    MEMCMP_EQUAL(&pushed, &popped, sizeof(XYCoordinate_t));
}

TEST(StackTests, PopShouldGetTheRightStuffWithAnotherTypeAfterSeveralPushes)
{
    XYCoordinate_t pushed1 = { 10, 11 };
    XYCoordinate_t pushed2 = { 12, 13 };
    XYCoordinate_t pushed3 = { 14, 15 };

    Stack_Push(&xyCoordStack, &pushed1);
    Stack_Push(&xyCoordStack, &pushed2);
    Stack_Push(&xyCoordStack, &pushed3);

    XYCoordinate_t popped = { 0, 0 };

    Stack_Pop(&xyCoordStack, &popped);
    MEMCMP_EQUAL(&pushed3, &popped, sizeof(XYCoordinate_t));
    Stack_Pop(&xyCoordStack, &popped);
    MEMCMP_EQUAL(&pushed2, &popped, sizeof(XYCoordinate_t));
    Stack_Pop(&xyCoordStack, &popped);
    MEMCMP_EQUAL(&pushed1, &popped, sizeof(XYCoordinate_t));
}

TEST(StackTests, SizeShouldAdaptAccordingly)
{
    CHECK_TRUE(Stack_Empty(&intStack));

    for(uint8_t i = 0; i < 10; i++)
    {
        CHECK_EQUAL(i, Stack_Size(&intStack));
        Stack_Push(&intStack, &i);
    }

    uint8_t popped;
    for(uint8_t i = 10; i > 0; i--)
    {
        CHECK_EQUAL(i, Stack_Size(&intStack));
        Stack_Pop(&intStack, &popped);
    }

    CHECK_TRUE(Stack_Empty(&intStack));
}
