#ifndef EVENTSUBSCRIBER_SYNCHRONOUS_H
#define EVENTSUBSCRIBER_SYNCHRONOUS_H

#include "I_EventSubscriber.h"
#include "LinkedListNode.h"

typedef struct
{
    I_EventSubscriber_t interface;
    LinkedListNode_t node;
} EventSubscriber_Synchronous_t;

/*
 * Initialize event synchronous
 * @param instance The event instance
 * @param callback The callback function to be called when the event we subscribe to publishes
 * @param context to pass to callback when event publishes
 */
void EventSubscriber_Synchronous_Init(EventSubscriber_Synchronous_t *instance, EventSubscriberCallback_t callback, void *context);

#endif
