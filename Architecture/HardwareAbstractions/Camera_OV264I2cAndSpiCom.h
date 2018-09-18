#ifndef CAMERA_OV264I2CANDSPICOM_H
#define CAMERA_OV264I2CANDSPICOM_H

#include "I_Camera.h"
#include "I_GpioGroup.h"
#include "I_Spi.h"
#include "I_I2c.h"
#include "Event_Synchronous.h"
#include "EventSubscriber_Synchronous.h"
#include "TimerModule.h"
#include "TimerPeriodic.h"
#include "types.h"

typedef struct
{
	I_Camera_t interface;
	GpioChannel_t spiCsChannel;
	Event_Synchronous_t onImageCaptureDoneEvent;
	TimerPeriodic_t configTimer;
	TimerPeriodic_t pollTimer;
	EventSubscriber_Synchronous_t spiBurstReceiveDoneSubscriber;
	CameraImage_t currentImageData;
    I_I2c_t *configI2c;
    I_Spi_t *dataSpi;
    TimerModule_t *timerModule;
	uint8_t registerDataIndex;
	uint8_t cameraConfigState;
	bool busy;
} Camera_OV264I2cAndSpiCom_t;

/*
 * Initialize an instance of the OV264 camera sensor which requires a working SPI and I2C
 * Images are hard-coded to be received as 160x120 JPEGs.
 *
 * @param instance The object representing the camera
 * @param configI2c I2C instance needed by the module to set config registers
 * @param dataSpi SPI instance needed by the module to get image data
 * @param spiCsChannel GPIO channel to be used for SPI chip select
 * @param gpioGroup GPIO group abstraction instance
 * @param timerModule the timer module
 */
void Camera_OV264I2cAndSpi_Init(
	Camera_OV264I2cAndSpiCom_t *instance,
	I_I2c_t *configI2c,
	I_Spi_t *dataSpi,
	GpioChannel_t spiCsChannel,
	TimerModule_t *timerModule);

#endif
