#include "CommunicationArbiter.h"
#include "utils.h"

enum RunningState
{
    RunningState_Unitialized = 0,
    RunningState_DoImageCaptureCycle,
    RunningState_DoMotionCommandCycle,
    RunningState_DoMapSendCycle,
    RunningState_DoImageRecognitionCycle
};
typedef uint8_t RunningSate_t;

//static void SetRunningStateToDoImageRecognition(void *context, void *args)
//{
//    IGNORE(args);
//    RECAST(instance, context, CommunicationArbiter_t *);
//    instance->runningState = RunningState_DoImageRecognitionCycle;
//}
//
//static void SetRunningStateToMotion(void *context, void *args)
//{
//    RECAST(instance, context, CommunicationArbiter_t *);
//    IGNORE(args);
//    instance->runningState = RunningState_DoMotionCommandCycle;
//}

static void SetRunningStateToImage(void *context, void *args)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    IGNORE(args);
    instance->runningState = RunningState_DoImageCaptureCycle;
}

static void SetRunningStateToSendMap(void *context, void *args)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    IGNORE(args);
    instance->runningState = RunningState_DoMapSendCycle;
}

void CommunicationArbiter_Init(
    CommunicationArbiter_t *instance,
    RemoteMotionController_t *remoteMotionController,
    ImageForwardingController_t *imageForwardingController,
    MapSender_t *mapSender,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *visited,
    GridMap_FirstQuadrant5cmCell3m2x3m2_t *blocked,
    TimerModule_t *timerModule,
    ImageRecognitionController_t *imgRecognitionController)
{
    instance->remoteMotionController = remoteMotionController;
    instance->imageForwardingController = imageForwardingController;
    instance->mapSender = mapSender;
    instance->imgRecognitionController = imgRecognitionController;
    instance->state = RunningState_Unitialized;
    instance->motionAckCount = 0;
    instance->timerModule = timerModule;
    instance->runningState = RunningState_DoImageCaptureCycle;
    instance->visited = visited;
    instance->blocked = blocked;

    EventSubscriber_Synchronous_Init(&instance->imageForwardedSub, SetRunningStateToSendMap, instance);
    EventSubscriber_Synchronous_Init(&instance->sendMapsDoneSub, SetRunningStateToImage, instance);
//    EventSubscriber_Synchronous_Init(&instance->imageRecognizedSub, SetRunningStateToMotion, instance);
//    EventSubscriber_Synchronous_Init(&instance->motionAcknowledgedSub, SetRunningStateToImage, instance);

    Event_Subscribe(ImageForwardingController_GetOnImageForwardedEvent(imageForwardingController), &instance->imageForwardedSub.interface);
//    Event_Subscribe(RemoteMotionController_GetOnMotionAcknowledgedEvent(remoteMotionController), &instance->motionAcknowledgedSub.interface);
    Event_Subscribe(MapSender_GetOnMapsSentEvent(instance->mapSender), &instance->sendMapsDoneSub.interface);
//    Event_Subscribe(ImageRecognitionController_GetOnImageRecognizedEvent(instance->imgRecognitionController), &instance->imageRecognizedSub.interface);
}

void CommunicationArbiter_Run(CommunicationArbiter_t *instance)
{
    switch(instance->runningState)
    {
        case RunningState_DoImageCaptureCycle:
            ImageForwardingController_ForwardOneImage(instance->imageForwardingController);
            ImageForwardingController_Run(instance->imageForwardingController);
            break;

//        case RunningState_DoMotionCommandCycle:
//            RemoteMotionController_DoMotion(instance->remoteMotionController);
//            RemoteMotionController_Run(instance->remoteMotionController);
//            break;
//
//        case RunningState_DoImageRecognitionCycle:
//            ImageRecognitionController_Run(instance->imgRecognitionController);
//            break;

        case RunningState_DoMapSendCycle:
            MapSender_SendMaps(instance->mapSender, instance->visited, instance->blocked);
            MapSender_Run(instance->mapSender);
            break;
    }
}
