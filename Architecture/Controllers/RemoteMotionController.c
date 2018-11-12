#include "RemoteMotionController.h"
#include "I_Event.h"
#include "utils.h"

enum
{
    MotionMessageCommand = 0xA2,
    MotionMessageAck = 0xA2
};

static void ReceiveUartByte(void *context, void *args)
{
    RECAST(instance, context, RemoteMotionController_t *);
    RECAST(byte, args, uint8_t *);

    if(*byte == MotionMessageAck)
    {
        instance->ackCount++;
    }
    else
    {
        instance->newCommand = true;
        instance->currentCommand = *byte;
    }
}

static void DoMotion(RemoteMotionController_t *instance)
{
    TimerOneShot_Stop(&instance->resetModuleTimer);
    TimerOneShot_Start(&instance->resetModuleTimer);
    Event_Subscribe(Uart_GetOnByteReceivedEvent(instance->wifiUart), &instance->uartSub.interface);
    Uart_SendByte(instance->wifiUart, MotionMessageCommand);
}

static void CheckUartAcquire(void *context)
{
    RECAST(instance, context, RemoteMotionController_t *);

    if(Uart_Acquire(instance->wifiUart))
    {
        TimerPeriodic_Command(&instance->periodicMotionTimer, TimerPeriodicCommand_Stop);
        DoMotion(instance);
    }
}

void RemoteMotionController_DoMotion(RemoteMotionController_t *instance)
{
    if(!instance->busy)
    {
        instance->busy = true;
        if(Uart_Acquire(instance->wifiUart))
        {
            DoMotion(instance);
        }
        else
        {
            TimerPeriodic_Start(&instance->periodicMotionTimer);
        }
    }
}

I_Event_t * RemoteMotionController_GetOnMotionAcknowledgedEvent(RemoteMotionController_t *instance)
{
    return &instance->onMotionAcknowledged.interface;
}

static void ResetModule(void *context)
{
    RECAST(instance, context, RemoteMotionController_t *);
    Event_Unsubscribe(Uart_GetOnByteReceivedEvent(instance->wifiUart), &instance->uartSub.interface);
    TimerPeriodic_Command(&instance->periodicMotionTimer, TimerPeriodicCommand_Stop);
    Uart_Release(instance->wifiUart);
    instance->busy = false;
}

void RemoteMotionController_Init(
    RemoteMotionController_t *instance,
    I_MotorController_t *motorController,
    I_Uart_t *wifiUart,
    uint16_t ticksToMoveWhenForward,
    uint16_t ticksToMoveWhenRight,
    uint16_t ticksToMoveWhenLeft,
    TimerModule_t *timerModule)
{
    instance->motorController = motorController;
    instance->currentCommand = MotionCommand_Uninitialized;
    instance->wifiUart = wifiUart;
    instance->busy = false;
    instance->newCommand = false;
    instance->ackCount = 0;
    instance->fwdTicks = ticksToMoveWhenForward;
    instance->rightTicks = ticksToMoveWhenRight;
    instance->leftTicks = ticksToMoveWhenLeft;

    TimerOneShot_Init(&instance->resetModuleTimer, timerModule, 500, ResetModule, instance);

    Event_Synchronous_Init(&instance->onMotionAcknowledged);
    EventSubscriber_Synchronous_Init(&instance->uartSub, ReceiveUartByte, instance);

    TimerPeriodic_Init(
        &instance->periodicMotionTimer,
        timerModule,
        5,
        CheckUartAcquire,
        instance);
}

void RemoteMotionController_Run(RemoteMotionController_t *instance)
{
     if(instance->ackCount >= 2 && instance->newCommand)
    {
        Event_Unsubscribe(Uart_GetOnByteReceivedEvent(instance->wifiUart), &instance->uartSub.interface);
        instance->busy = false;
        instance->newCommand = false;
        instance->ackCount = 0;

        switch(instance->currentCommand)
        {
            case MotionCommand_Forward:
                MotorController_Forward(
                    instance->motorController,
                    instance->fwdTicks);
                break;

            case MotionCommand_Left:
                MotorController_TurnLeft(
                    instance->motorController,
                    instance->leftTicks);
                break;

            case MotionCommand_Right:
                MotorController_TurnRight(
                    instance->motorController,
                    instance->rightTicks);
                break;

            default:
                break;
        }

        instance->currentCommand = MotionCommand_Uninitialized;
        Event_Publish(&instance->onMotionAcknowledged.interface, NULL);
        Uart_Release(instance->wifiUart);
    }
    else if(instance->ackCount >= 2)
    {
        instance->ackCount = 0;
        Event_Unsubscribe(Uart_GetOnByteReceivedEvent(instance->wifiUart), &instance->uartSub.interface);
        instance->busy = false;
        Event_Publish(&instance->onMotionAcknowledged.interface, NULL);
        Uart_Release(instance->wifiUart);
    }
}
