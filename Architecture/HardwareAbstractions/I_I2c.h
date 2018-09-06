#ifndef I_I2C_H
#define I_I2C_H

#include "types.h"

typedef void (*I2cReadByteCallbackFunction_t)(void *context, uint8_t receivedByte);
typedef void (*I2cWriteByteCallbackFunction_t)(void *context);
typedef struct _I2cApi_t I2cApi_t;

typedef struct
{
    const I2cApi_t *api;
} I_I2c_t;

struct _I2cApi_t
{
    /*
     * Send one byte via I2C peripheral
     * @param instance The I2C instance
     * @param deviceAddress Address of device to write to
     * @param writeAddress If device has internal addresses, specify address to write to
     * @param byteToWrite Byte to send to address
     * @param callback Callback to invoke when writing is done
     * @param context Context to pass to callback
     */
    void (*WriteByte)(
            I_I2c_t *instance,
            uint8_t deviceAddress,
            uint8_t writeAddres,
            uint8_t byteToWrite,
            I2cWriteByteCallbackFunction_t callback,
            void *context);

    /*
     * Receive one byte from I2C peripheral
     * @param instance The I2C instance
     * @param deviceAddress Address of device to read from
     * @param readAddress If device has internal addresses, specify address to read from
     * @param callback Callback to invoke when read data is available
     * @param context Context to pass to callback function
     */
    void (*ReadByte)(
            I_I2c_t *instance,
            uint8_t deviceAddress,
            uint8_t readAddress,
            I2cReadByteCallbackFunction_t callback,
            void *context);
};

#define I2c_WriteByte(_instance, _deviceAddress, _writeAddress, _byteToWrite, _callback, _context) \
        (_instance)->api->WriteByte(_instance, _deviceAddress, _writeAddress, _byteToWrite, _callback, _context) \

#define I2c_ReadByte(_instance, _deviceAddress, _readAddress, _callback, _context) \
        (_instance)->api->ReadByte(_instance, _deviceAddress, _readAddress, _callback, _context) \

#endif
