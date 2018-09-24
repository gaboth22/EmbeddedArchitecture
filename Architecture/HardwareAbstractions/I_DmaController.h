#ifndef I_DMACONTROLLER_H
#define I_DMACONTROLLER_H

#include "I_Event.h"
#include "types.h"

typedef struct _DmaControllerApi_t DmaControllerApi_t;

typedef struct
{
    const DmaControllerApi_t *api;
} I_DmaController_t;

struct _DmaControllerApi_t
{
    /*
     * Configure DMA channel source trigger,
     * memory to memory, peripheral to memory, and so on,
     * depending on HW
     * @param instance the DMA controller instance
     * @param channel the channel to configure
     * @param sourceTrigger the source trigger config
     */
    void (*SetChannelSourceTrigger)(I_DmaController_t *instance, uint32_t channel, const void *sourceTriggerConfig);

    /*
     * Configure DMA channel transfer specifics
     * @param instance the DMA controller instance
     * @param channel the channel to configure
     * @param config the channel transfer configuration
     */
    void (*SetChannelTransferConfig)(I_DmaController_t *instance, uint32_t channel, const void *config);

    /*
     * Set and start channel transfer
     * @param instance the DMA controller instance
     * @param channel the channel to do the transfer on
     * @param transferConfig any particular individal transfer config
     * @param src the source address
     * @param dst the destination address
     * @param transferSize the size of the transfer
     */
    void (*SetAndStartChannelTrasfer)(
            I_DmaController_t *instance,
            uint32_t channel,
            const void *transferConfig,
            void *src,
            void *dst,
            uint32_t transferSize);

    /*
     * Gets the event that fires when a channel transfer completes
     * @param instance the DMA controller instance
     * @param channel the channel to get the event for
     */
    I_Event_t * (*GetOnChannelTransferDoneEvent)(I_DmaController_t *instance, uint32_t channel);
};

#define DmaController_SetChannelSourceTrigger(_instance, _channel, _config) \
        (_instance)->api->SetChannelSourceTrigger(_instance, _channel, _config) \

#define DmaController_SetChannelTransferConfig(_instance, _channel, _config) \
        (_instance)->api->SetChannelTransferConfig(_instance, _channel, _config) \

#define DmaController_SetAndStartChannelTrasfer(_instance, _channel, _config, _src, _dst, _trxSize) \
        (_instance)->api->SetAndStartChannelTrasfer(_instance, _channel, _config, _src, _dst, _trxSize) \

#define DmaController_GetOnChannelTransferDoneEvent(_instance, _channel) \
        (_instance)->api->GetOnChannelTransferDoneEvent(_instance, _channel) \

#endif
