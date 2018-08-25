#include "CppUTest/TestHarness.h"

extern "C"
{
#include <stdlib.h>
#include "LinkedList.h"
#include "utils.h"
}

TEST_GROUP(LinkedListTests)
{
	LinkedList_t list;
	LinkedListNode_t node;

	void setup()
	{
		LinkedList_Init(&list);
	}
};

TEST(LinkedListTests, InsertWhenListIsEmptyShouldSucceed)
{
	LinkedListNode_Init(&node, NULL);
	LinkedList_Insert(&list, &node);
}

static void GetIntData(LinkedListNode_t *node, void *data)
{
	memcpy(data, node->data, sizeof(int));
}

TEST(LinkedListTests, ShouldBeAbleToGetDataAfterInsertion)
{
	int insertedData = 10;
	LinkedListNode_Init(&node, (void*)(&insertedData));
	LinkedList_Insert(&list, &node);
	int outputData;
	LinkedList_ForEach(&list, GetIntData, &outputData);
	CHECK_EQUAL(insertedData, outputData);
}

TEST(LinkedListTests, RemovalOfHeadShouldMakeHeadNull)
{
	LinkedListNode_Init(&node, NULL);
	LinkedList_Insert(&list, &node);
	LinkedList_Remove(&list, &node);
	CHECK_EQUAL(NULL, list.head);
}
