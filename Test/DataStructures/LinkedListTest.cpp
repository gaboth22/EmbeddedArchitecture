#include "CppUTest/TestHarness.h"

extern "C"
{
#include <stdlib.h>
#include "LinkedList.h"
#include "utils.h"
}

LinkedList_t list;
LinkedListNode_t node;

static void InitializeList()
{
	LinkedList_Init(&list);
}

TEST_GROUP(LinkedListTests)
{
};

TEST(LinkedListTests, ListShouldInitializeFine)
{
	InitializeList();
}

TEST(LinkedListTests, InsertWhenListIsEmptyShouldSucceed)
{
	InitializeList();
	LinkedListNode_Init(&node, NULL);
	LinkedList_Insert(&list, &node);
}

static void GetIntData(LinkedListNode_t *node, void *data)
{
	memcpy(data, node->data, sizeof(int));
}

TEST(LinkedListTests, ShouldBeAbleToGetDataAfterInsertion)
{
	InitializeList();
	int insertedData = 10;
	LinkedListNode_Init(&node, (void*)(&insertedData));
	LinkedList_Insert(&list, &node);
	int outputData;
	LinkedList_ForEach(&list, GetIntData, &outputData);
	CHECK_EQUAL(insertedData, outputData);
}

TEST(LinkedListTests, RemovalOfHeadShouldMakeHeadNull)
{
	InitializeList();
	LinkedListNode_Init(&node, NULL);
	LinkedList_Insert(&list, &node);
	LinkedList_Remove(&list, &node);
	CHECK_EQUAL(NULL, list.head);
}
