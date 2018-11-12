#ifndef REMOTEMOTIONCONTROLLER_H
#define REMOTEMOTIONCONTROLLER_H

#include "I_MotorController.h"
#include "EventSubscriber_Synchronous.h"
#include "Event_Synchronous.h"
#include "MotionCommnad.h"
#include "I_Uart.h"
#include "TimerPeriodic.h"
#include "TimerOneShot.h"
#include "types.h"

typedef struct
{
    I_MotorController_t *motorController;
    MotionCommand_t currentCommand;
    I_Uart_t *wifiUart;
    Event_Synchronous_t onMotionAcknowledged;
    EventSubscriber_Synchronous_t uartSub;
    uint8_t ackCount;
    bool busy;
    bool newCommand;
    uint16_t fwdTicks;
    uint16_t leftTicks;
    uint16_t rightTicks;
    TimerPeriodic_t periodicMotionTimer;
    TimerOneShot_t resetModuleTimer;
} RemoteMotionController_t;

/*
 * Let the module run. To be placed in a fast-running loop
 */
void RemoteMotionController_Run(RemoteMotionController_t *instance);

/*
 * Carry out motion
 * @param instance The object
 */
void RemoteMotionController_DoMotion(RemoteMotionController_t *instance);

/*
 * Get event that fires when a motion command is acknowledged
 */
I_Event_t * RemoteMotionController_GetOnMotionAcknowledgedEvent(RemoteMotionController_t *instance);

/*
 * Initialize the remote motion controller
 * @param instance The object
 * @param motorController A motor controller
 */
void RemoteMotionController_Init(
    RemoteMotionController_t *instance,
    I_MotorController_t *motorController,
    I_Uart_t *wifiUart,
    uint16_t ticksToMoveWhenForward,
    uint16_t ticksToMoveWhenRight,
    uint16_t ticksToMoveWhenLeft,
    TimerModule_t *timerModule);

#endif
