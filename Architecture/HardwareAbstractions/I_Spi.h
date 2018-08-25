#ifndef I_SPI_H
#define I_SPI_H

#include "types.h"

typedef struct _SpiApi_t SpiApi_t;

typedef struct
{
    const SpiApi_t *api;
} I_Spi_t;

struct _SpiApi_t
{
    /*
     * Send one byte via SPI peripheral
     */
    void (*SendByte)(I_Spi_t *instance, uint8_t byte);
};

#define Spi_SendByte(_instance, _byte) \
        (_instance)->api->SendByte(_instance, _byte) \

#endif
