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
    RunningState_WaitForAck,
    PeriodToConsiderCommDeadMs = 1000
};
typedef uint8_t RunningSate_t;

static void InterpretAck(void *context, void *args)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    RECAST(ack, args, uint8_t *);

    TimerPeriodic_Command(&instance->checkStateTimer, TimerPeriodicCommand_Pause);

    switch(*ack)
    {
        case Ack_ReceiveImageCommand:
            instance->state = RunningState_DoMotionCommandCycle;
            break;

        case Ack_ReturnMotionCommand:
            instance->motionAckCount++;
            if(instance->motionAckCount >= 2)
            {
                instance->motionAckCount = 0;
                instance->state = RunningState_DoImageCaptureCycle;
            }

        default:
            break;
    }
}

static void ReEnableUartRxToWaitForAck(void *context, void *args)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    IGNORE(args);

    Uart_EnableRx(instance->wifiChipUart);
}

static void ResetState(void *context)
{
    RECAST(instance, context, CommunicationArbiter_t *);
    instance->motionAckCount = 0;
    instance->state = RunningState_DoImageCaptureCycle;
    Camera_ClearState(instance->camera);
}

void CommunicationArbiter_Init(
    CommunicationArbiter_t *instance,
    I_Camera_t *camera,
    I_Uart_t *wifiChipUart,
    I_Event_t *onImageForwarded,
    RemoteMotionController_t *remoteMotionController,
    TimerModule_t *timerModule)
{
    instance->remoteMotionController = remoteMotionController;
    instance->wifiChipUart = wifiChipUart;
    instance->camera = camera;
    instance->state = RunningState_DoImageCaptureCycle;
    instance->motionAckCount = 0;
    instance->timerModule = timerModule;

    TimerPeriodic_Init(
        &instance->checkStateTimer,
        timerModule,
        PeriodToConsiderCommDeadMs,
        ResetState,
        instance);

    EventSubscriber_Synchronous_Init(&instance->imageTrxDoneSub, ReEnableUartRxToWaitForAck, instance);
    Event_Subscribe(onImageForwarded, &instance->imageTrxDoneSub.interface);

    EventSubscriber_Synchronous_Init(&instance->uartByteReceivedSub, InterpretAck, instance);
    Event_Subscribe(Uart_GetOnByteReceivedEvent(instance->wifiChipUart), &instance->uartByteReceivedSub.interface);
}

void CommunicationArbiter_Run(CommunicationArbiter_t *instance)
{
    switch(instance->state)
    {
        case RunningState_DoImageCaptureCycle:
            instance->startCheck = true;
            instance->state = RunningState_Unitialized;
            Camera_StartImageCapture(instance->camera);
            break;

        case RunningState_DoMotionCommandCycle:
            instance->startCheck = true;
            instance->state = RunningState_Unitialized;
            RemoteMotionController_DoMotion(instance->remoteMotionController);
            break;

        default:
            break;
    }

    if(instance->startCheck)
    {
        instance->startCheck = false;
        TimerPeriodic_Command(&instance->checkStateTimer, TimerPeriodicCommand_Stop);
        TimerPeriodic_Start(&instance->checkStateTimer);
    }
}
