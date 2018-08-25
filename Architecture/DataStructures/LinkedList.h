#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "LinkedListNode.h"

typedef void (*ForEachFunction_t)(LinkedListNode_t *node, void *args);

typedef struct
{
    LinkedListNode_t *head;
} LinkedList_t;

/*
 * Initializes linked list
 * @param list The list instance
 */
void LinkedList_Init(LinkedList_t *list);

/*
 * Insert element into list
 */
void LinkedList_Insert(LinkedList_t *list, LinkedListNode_t *to_insert);

/*
 * Remove element from list
 */
void LinkedList_Remove(LinkedList_t *list, LinkedListNode_t *to_remove);

/*
 * Apply function to all nodes in list
 */
void LinkedList_ForEach(LinkedList_t *list, ForEachFunction_t function, void *args);

#endif
