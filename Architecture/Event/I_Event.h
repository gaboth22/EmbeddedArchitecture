#ifndef I_EVENT_H
#define I_EVENT_H

#include "I_EventSubscriber.h"

typedef struct _EventApi_t EventApi_t;

typedef struct
{
    const EventApi_t *api;
} I_Event_t;

struct _EventApi_t
{
    /*
     * Publish args to all subscribers
     */
    void (*Publish)(I_Event_t *instance, void *args);

    /*
     * Subscriber a certain subscriber to an event
     */
    void (*Subscribe)(I_Event_t *instance, I_EventSubscriber_t *subscriber);

    /*
     * Unsubscribe a subscriber from an event
     */
    void (*Unsubscribe)(I_Event_t *instance, I_EventSubscriber_t *subscriber);
};


#define Event_Publish(_instance, _args) \
        (_instance)->api->Publish(_instance, _args) \

#define Event_Subscribe(_instance, _subscriber) \
        (_instance)->api->Subscribe(_instance, _subscriber) \

#define Event_Unsubscribe(_instance, _subscriber) \
        (_instance)->api->Unsubscribe(_instance, _subscriber) \

#endif
