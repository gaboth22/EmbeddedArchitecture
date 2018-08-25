#ifndef I_EVENTSUBSCRIBER_H
#define I_EVENTSUBSCRIBER_H

typedef void (*EventSubscriberCallback_t)(void *context, void *args);

typedef struct
{
    void *context;
    void *callback;
} EventSubscriberData_t;

typedef struct
{
    EventSubscriberData_t data;
} I_EventSubscriber_t;

#endif
