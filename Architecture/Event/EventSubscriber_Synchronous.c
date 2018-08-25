#include "EventSubscriber_Synchronous.h"
#include "LinkedListNode.h"

void EventSubscriber_Synchronous_Init(EventSubscriber_Synchronous_t *instance, EventSubscriberCallback_t callback, void *context)
{
    instance->interface.data.callback = callback;
    instance->interface.data.context = context;
    LinkedListNode_Init(&instance->node, &instance->interface.data);
}
