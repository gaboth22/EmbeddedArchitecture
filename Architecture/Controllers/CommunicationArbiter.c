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
}

void CommunicationArbiter_Run(CommunicationArbiter_t *instance)
{
    ImageForwardingController_ForwardOneImage(instance->imageForwardingController);
    ImageForwardingController_Run(instance->imageForwardingController);

    RemoteMotionController_DoMotion(instance->remoteMotionController);
    RemoteMotionController_Run(instance->remoteMotionController);
}
