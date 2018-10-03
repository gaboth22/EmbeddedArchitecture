#include "RemoteMotionController.h"
#include "I_Event.h"
#include "utils.h"

enum
{
    MotionMessageCommand = 0xA2,
    Ack_ReturnMotionCommand = 0xA2,
    ForwardTicksToMove = 5 * 75,
    LeftTicksToMove = 95,
    RightTicksToMove = 95
};

static void ReceiveUartByte(void *context, void *args)
{
    RECAST(instance, context, RemoteMotionController_t *);
    RECAST(byte, args, uint8_t *);

    if(*byte == Ack_ReturnMotionCommand)
    {
        instance->ackCount++;
    }
    else
    {
        instance->newCommand = true;
        instance->currentCommand = *byte;
    }
}

void RemoteMotionController_DoMotion(RemoteMotionController_t *instance)
{
    if(!instance->busy)
    {
        instance->busy = true;
        Uart_EnableRx(instance->wifiUart);
        Event_Subscribe(Uart_GetOnByteReceivedEvent(instance->wifiUart), &instance->uartSub.interface);
        Uart_SendByte(instance->wifiUart, MotionMessageCommand);
    }
}

void RemoteMotionController_Init(
    RemoteMotionController_t *instance,
    MotorController_t *motorController,
    I_Uart_t *wifiUart)
{
    instance->motorController = motorController;
    instance->currentCommand = MotionCommand_Uninitialized;
    instance->wifiUart = wifiUart;
    instance->busy = false;
    instance->newCommand = false;
    instance->ackCount = 0;
    EventSubscriber_Synchronous_Init(&instance->uartSub, ReceiveUartByte, instance);
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
                MotorController_Forward(instance->motorController, ForwardTicksToMove);
                break;

            case MotionCommand_Left:
                MotorController_TurnLeft(instance->motorController, LeftTicksToMove);
                break;

            case MotionCommand_Right:
                MotorController_TurnRight(instance->motorController, RightTicksToMove);
                break;

            default:
                break;
        }

        instance->currentCommand = MotionCommand_Uninitialized;
    }
    else if(instance->ackCount >= 2)
    {
        instance->ackCount = 0;
        Event_Unsubscribe(Uart_GetOnByteReceivedEvent(instance->wifiUart), &instance->uartSub.interface);
        instance->busy = false;
    }
}
