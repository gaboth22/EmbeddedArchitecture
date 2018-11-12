#include "CommunicationArbiter.h"
#include "utils.h"

enum Ack
{
    Ack_ReceiveImageCommand = 0xA1,
    Ack_ReturnMotionCommand = 0xA2
};
typedef uint8_t Ack_t;

enum RunningState
{
    RunningState_Unitialized = 0,
    RunningState_DoImageCaptureCycle,
    RunningState_DoMotionCommandCycle,
};
typedef uint8_t RunningSate_t;

static void SetRunningStateToMotion(void *context, void *args)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    IGNORE(args);
    instance->runningState = RunningState_DoMotionCommandCycle;
}

static void SetRunningStateToImage(void *context, void *args)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    IGNORE(args);
    instance->runningState = RunningState_DoImageCaptureCycle;
}

void CommunicationArbiter_Init(
    CommunicationArbiter_t *instance,
    RemoteMotionController_t *remoteMotionController,
    ImageForwardingController_t *imageForwardingController,
    TimerModule_t *timerModule)
{
    instance->remoteMotionController = remoteMotionController;
    instance->imageForwardingController = imageForwardingController;
    instance->state = RunningState_Unitialized;
    instance->motionAckCount = 0;
    instance->timerModule = timerModule;
    instance->runningState = RunningState_DoImageCaptureCycle;

    EventSubscriber_Synchronous_Init(&instance->imageForwardedSub, SetRunningStateToMotion, instance);
    EventSubscriber_Synchronous_Init(&instance->motionAcknowledgedSub, SetRunningStateToImage, instance);
    Event_Subscribe(ImageForwardingController_GetOnImageForwardedEvent(imageForwardingController), &instance->imageForwardedSub.interface);
    Event_Subscribe(RemoteMotionController_GetOnMotionAcknowledgedEvent(remoteMotionController), &instance->motionAcknowledgedSub.interface);
}

void CommunicationArbiter_Run(CommunicationArbiter_t *instance)
{
    switch(instance->runningState)
    {
        case RunningState_DoImageCaptureCycle:
            ImageForwardingController_ForwardOneImage(instance->imageForwardingController);
            ImageForwardingController_Run(instance->imageForwardingController);
            break;

        case RunningState_DoMotionCommandCycle:
            RemoteMotionController_DoMotion(instance->remoteMotionController);
            RemoteMotionController_Run(instance->remoteMotionController);
            break;
    }
}
