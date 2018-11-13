#include "DataRouter.h"

enum
{
    MaxDataTransferChunkSize = 256
};

void DataRouter_Init(
    DataRouter_t *instance,
    I_DmaController_t *dmaController,
    TimerModule_t *timerModule)
{
    instance->dmaController = dmaController;
    instance->timerModule = timerModule;
}
