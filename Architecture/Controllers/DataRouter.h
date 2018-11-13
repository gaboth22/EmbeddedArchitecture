#ifndef DATAROUTER_H
#define DATAROUTER_H

#include "I_DmaController.h"
#include "TimerModule.h"
#include "TimerPeriodic.h"

typedef struct
{
    I_DmaController_t *dmaController;
    TimerModule_t *timerModule;
    TimerPeriodic_t dataTrxTimer;
} DataRouter_t;

/*
 * Initialize the module
 * @param instance The object
 * @param dmaController The DMA controller
 * @param timerModule The system timer module
 */
void DataRouter_Init(
    DataRouter_t *instance,
    I_DmaController_t *dmaController,
    TimerModule_t *timerModule);

#endif
