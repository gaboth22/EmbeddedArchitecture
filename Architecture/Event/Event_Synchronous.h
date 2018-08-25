#ifndef EVENT_SYNCHRONOUS_H
#define EVENT_SYNCHRONOUS_H

#include "I_Event.h"
#include "LinkedList.h"

typedef struct
{
    I_Event_t interface;
    LinkedList_t subscriberList;
} Event_Synchronous_t;

/*
 * Initialize event
 */
void Event_Synchronous_Init(Event_Synchronous_t *event);

#endif
