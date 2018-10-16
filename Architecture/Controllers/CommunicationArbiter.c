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
    RunningState_StartCommCheck,
    RunningState_DoImageCaptureCycle,
    RunningState_DoMotionCommandCycle,
    RunningState_WaitForAck
};
typedef uint8_t RunningSate_t;

enum
{
    PeriodToConsiderCommDeadMs = 5000,
    PeriodToRestartCaptureMs = 5000
};

static void MoveOnToImageCaptureCycle(void *context, void *args)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    IGNORE(args);

    TimerOneShot_Stop(&instance->checkStateTimer);
    instance->state = RunningState_DoImageCaptureCycle;
}

static void MoveOnToMotionCycle(void *context, void *args)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    IGNORE(args);

    TimerOneShot_Stop(&instance->checkStateTimer);
    instance->state = RunningState_DoMotionCommandCycle;
}

static void RestartCapture(void *context)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    instance->state = RunningState_DoImageCaptureCycle;
}

static void ClearCommState(void *context)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    instance->motionAckCount = 0;
    ImageForwardingController_ClearState(instance->imageForwardingController);
    TimerOneShot_Start(&instance->delayCaptureTimer);
}

void CommunicationArbiter_Init(
    CommunicationArbiter_t *instance,
    RemoteMotionController_t *remoteMotionController,
    ImageForwardingController_t *imageForwardingController,
    TimerModule_t *timerModule)
{
    instance->remoteMotionController = remoteMotionController;
    instance->imageForwardingController = imageForwardingController;
    instance->state = RunningState_StartCommCheck;
    instance->motionAckCount = 0;
    instance->timerModule = timerModule;

    TimerOneShot_Init(
        &instance->checkStateTimer,
        timerModule,
        PeriodToConsiderCommDeadMs,
        ClearCommState,
        instance);

    TimerOneShot_Init(
        &instance->delayCaptureTimer,
        timerModule,
        PeriodToRestartCaptureMs,
        RestartCapture,
        instance);

    EventSubscriber_Synchronous_Init(&instance->imageForwardedSub, MoveOnToMotionCycle, instance);
    Event_Subscribe(
        ImageForwardingController_GetOnImageForwardedEvent(imageForwardingController),
        &instance->imageForwardedSub.interface);

    EventSubscriber_Synchronous_Init(&instance->motionAcknowledgedSub, MoveOnToImageCaptureCycle, instance);
    Event_Subscribe(
        RemoteMotionController_GetOnMotionAcknowledgedEvent(remoteMotionController),
        &instance->motionAcknowledgedSub.interface);
}

void CommunicationArbiter_Run(CommunicationArbiter_t *instance)
{
    switch(instance->state)
    {
        case RunningState_StartCommCheck:
            TimerOneShot_Start(&instance->checkStateTimer);
            instance->state = RunningState_DoImageCaptureCycle;
            break;

        case RunningState_DoImageCaptureCycle:
            instance->state = RunningState_Unitialized;
            TimerOneShot_Start(&instance->checkStateTimer);
            ImageForwardingController_ForwardOneImage(instance->imageForwardingController);
            break;

        case RunningState_DoMotionCommandCycle:
            instance->state = RunningState_Unitialized;
            TimerOneShot_Start(&instance->checkStateTimer);
            RemoteMotionController_DoMotion(instance->remoteMotionController);
            break;

        default:
            break;
    }
}
