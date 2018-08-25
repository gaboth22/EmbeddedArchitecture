#include "LinkedListNode.h"
#include "types.h"

void LinkedListNode_Init(LinkedListNode_t *node, void *data)
{
  node->data = data;
  node->next = NULL;
}
