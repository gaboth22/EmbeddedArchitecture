#ifndef I_SPI_H
#define I_SPI_H

#include "I_Event.h"
#include "types.h"

typedef struct
{
	void *dataAddress;
	uint16_t dataSize;
} SpiBurstReceiveInfo_t;

typedef struct _SpiApi_t SpiApi_t;

typedef struct
{
    const SpiApi_t *api;
} I_Spi_t;

struct _SpiApi_t
{
    /*
     * Send one byte via SPI peripheral
     * @param instance The SPI instance
     * @param bytetoSend byte to send out
     */
    void (*SendByte)(I_Spi_t *instance, uint8_t byteToSend);

    /*
     * Get one byte via SPI peripheral
     * @param instance The SPI instance
     * @param byteToPutOnBus byte to put on bus when in order to receive one byte
     */
    uint8_t (*GetByte)(I_Spi_t *instance, uint8_t byteToPutOnBus);

    /*
     * Start burst receive operation
     * @param instance The SPI object
     * @param sizeOfBurst size of the burst of data to transfer in
     * @param destination Pointer start of block of memory to put received data at
     */
    void (*StartBurstReceive)(I_Spi_t *instance, void *destination, uint16_t sizeOfBurst);

    /*
     * If the peripheral is set up with DMA, or burst receives are handled internally,
     * this method can be implemented to get the event that is generated when
     * a burst transfer data reception is completed.
     *
     * @param instance The SPI instance
     */
    I_Event_t * (*GetOnBurstReceiveDoneEvent)(I_Spi_t *instance);
};

#define Spi_SendByte(_instance, _byteToWrite) \
        (_instance)->api->SendByte(_instance, _byteToWrite) \

#define Spi_GetByte(_instance, _byteToPutOnBus) \
        (_instance)->api->GetByte(_instance, _byteToPutOnBus) \

#define Spi_StartBurstReceive(_instance, _dest, _size) \
        (_instance)->api->StartBurstReceive(_instance, _dest, _size) \

#define Spi_GetOnBurstReceiveDoneEvent(_instance) \
        (_instance)->api->GetOnBurstReceiveDoneEvent(_instance) \

#endif
