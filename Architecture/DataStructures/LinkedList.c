#include "LinkedList.h"
#include "LinkedListNode.h"
#include "types.h"
#include "utils.h"

void LinkedList_Init(LinkedList_t *list)
{
    list->head = NULL;
}

void LinkedList_Insert(LinkedList_t *list, LinkedListNode_t *toInsert)
{
    if(list->head == NULL)
    {
        list->head = toInsert;
        return;
    }

    LinkedListNode_t *temp = list->head;

    while(temp->next != NULL)
    {
        temp = temp->next;
    }

    toInsert->next = NULL;
    temp->next = toInsert;
}

void LinkedList_Remove(LinkedList_t *list, LinkedListNode_t *toRemove)
{
    if(list->head != NULL && toRemove == list->head)
    {
        list->head = list->head->next;
        return;
    }

    LinkedListNode_t *temp = list->head;

    while(temp != NULL)
    {
        if(temp->next != NULL && temp->next == toRemove)
        {
            temp->next = toRemove->next;
            break;
        }
        temp = temp->next;
    }
}

void LinkedList_ForEach(LinkedList_t *list, ForEachFunction_t function, void *args)
{
    LinkedListNode_t *temp = list->head;

    while(temp != NULL)
    {
        function(temp, args);
        temp = temp->next;
    }
}
