#ifndef LINKEDLISTNODE_H
#define LINKEDLISTNODE_H

typedef struct Node_t LinkedListNode_t;

struct Node_t
{
    LinkedListNode_t *next;
    void *data;
};

/*
 * Initialize linked list node
 * @param node The node instance
 * @param data The data to store
 */
void LinkedListNode_Init(LinkedListNode_t *node, void *data);

#endif
