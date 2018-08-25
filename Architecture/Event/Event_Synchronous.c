#include "Event_Synchronous.h"
#include "I_Event.h"
#include "I_EventSubscriber.h"
#include "EventSubscriber_Synchronous.h"
#include "LinkedList.h"
#include "utils.h"

static void CallSubscriberCallback(LinkedListNode_t *node, void *data)
{
    RECAST(eventData, node->data, EventSubscriberData_t *);
    RECAST(subscriberCallback, eventData->callback, EventSubscriberCallback_t);
    subscriberCallback(eventData->context, data);
}

static void Publish(I_Event_t *event, void *args)
{
    RECAST(instance, event, Event_Synchronous_t *);
    LinkedList_ForEach(&instance->subscriberList, CallSubscriberCallback, args);
}

static void Subscribe(I_Event_t *instance, I_EventSubscriber_t *subscriber)
{
    RECAST(_instance, instance, Event_Synchronous_t *);
    RECAST(_subscriber, subscriber, EventSubscriber_Synchronous_t *);

    LinkedListNode_Init(&_subscriber->node, &_subscriber->interface.data);
    LinkedList_Insert(&_instance->subscriberList, &_subscriber->node);
}

static void Unsubscribe(I_Event_t *instance, I_EventSubscriber_t *subscriber)
{
    RECAST(_instance, instance, Event_Synchronous_t *);
    RECAST(_subscriber, subscriber, EventSubscriber_Synchronous_t *);

    LinkedList_Remove(&_instance->subscriberList, &_subscriber->node);
}

static const EventApi_t eventSynchronousApi =
    {
        Publish,
        Subscribe,
        Unsubscribe
    };

void Event_Synchronous_Init(Event_Synchronous_t *event)
{
    LinkedList_Init(&event->subscriberList);
    event->interface.api = &eventSynchronousApi;
}
