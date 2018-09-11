#include "Camera_OV264I2cAndSpiCom.h"
#include "CameraOV264I2cAndSpiComRegisterData.h"
#include "utils.h"

enum
{
	CameraSensorWriteAddress = 0x60,
	CameraSensorReadAddress = 0x61,
	DataTerminator = 0xFF,
	CamFifoAddress = 0x04,
	CamFifoClearMask = 0x01,
	CamFifoStartMask = 0x02,
	CamCaptureRegisterAddress = 0x41,
	CamCaptureDoneMask = 0x08,
	CamFifoSizeRegAddress1 = 0x42,
	CamFifoSizeRegAddress2 = 0x43,
	CamFifoSizeRegAddress3 = 0x44,
	PollCameraForCaptureDoneTimeMs = 10
};

/*
 * Buffer to place image in
 */
static uint8_t receivedImage[2048]; // Since images are hard-coded to 160x120 they won't be more than 2K
/*
 *
 */

static void PublishImageData(void *context, void *args)
{
	RECAST(instance, context, Camera_OV264I2cAndSpiCom_t *);
	RECAST(spiInfo, args, SpiBurstReceiveInfo_t *);

	instance->currentImageData.image = spiInfo->dataAddress;
	instance->currentImageData.imageSize = spiInfo->dataSize;

	Event_Unsubscribe(
		Spi_GetOnBurstReceiveDoneEvent(instance->dataSpi),
		&instance->spiBurstReceiveDoneSubscriber.interface);
	Event_Publish(&instance->onImageCaptureDoneEvent.interface, &instance->currentImageData);
}

static void PollCameraForCaptureDone(void *context)
{
	RECAST(instance, context, Camera_OV264I2cAndSpiCom_t *);

	Spi_SendByte(instance->dataSpi, CamCaptureRegisterAddress & 0x7F);
	uint8_t captureDone = Spi_GetByte(instance->dataSpi, 0xFF) & CamCaptureDoneMask;

	if((bool)captureDone)
	{
		TimerPeriodic_Command(&instance->checkIfCameraCaptureIsDoneTimer, TimerPeriodicCommand_Stop);

		uint16_t len1;
		uint16_t len2;
		uint16_t totalLength = 0;

		Spi_SendByte(instance->dataSpi, CamFifoSizeRegAddress1 & 0x7F);
		len1 = Spi_GetByte(instance->dataSpi, 0xFF);
		Spi_SendByte(instance->dataSpi, CamFifoSizeRegAddress2 & 0x7F);
		len2 = Spi_GetByte(instance->dataSpi, 0xFF);

		totalLength = ((len2 << 8) | len1);

		Event_Subscribe(
				Spi_GetOnBurstReceiveDoneEvent(instance->dataSpi),
			&instance->spiBurstReceiveDoneSubscriber.interface);
		Spi_StartBurstReceive(instance->dataSpi, receivedImage, totalLength);
	}
}

static void WriteCameraConfig(void *context)
{
	RECAST(instance, context, Camera_OV264I2cAndSpiCom_t *);

	uint8_t regAddress = InitializeJpegRegData[instance->registerDataIndex].reg;
	uint8_t regData = InitializeJpegRegData[instance->registerDataIndex].data;

	if(regAddress == DataTerminator && regData == DataTerminator)
	{
		return;
	}
	else
	{
		I2c_WriteByte(
			instance->configI2c,
			CameraSensorWriteAddress,
			regAddress,
			regData,
			WriteCameraConfig,
			instance);
		instance->registerDataIndex++;
	}
}

static void SetConfiguration(I_Camera_t *_instance, const void *configuration)
{
	IGNORE(_instance);
	IGNORE(configuration);
}

void StartImageCapture(I_Camera_t *_instance)
{
	RECAST(instance, _instance, Camera_OV264I2cAndSpiCom_t *);

	Spi_SendByte(instance->dataSpi, CamFifoAddress | 0x80);
	Spi_SendByte(instance->dataSpi, CamFifoClearMask);
	Spi_SendByte(instance->dataSpi, CamFifoAddress | 0x80);
	Spi_SendByte(instance->dataSpi, CamFifoStartMask);

	TimerPeriodic_Start(&instance->checkIfCameraCaptureIsDoneTimer);
}

I_Event_t * GetOnImageCaptureDoneEvent(I_Camera_t *_instance)
{
	RECAST(instance, _instance, Camera_OV264I2cAndSpiCom_t *);
	return &instance->onImageCaptureDoneEvent.interface;
}

static const CameraApi_t api =
	{ SetConfiguration, StartImageCapture, GetOnImageCaptureDoneEvent };

void Camera_OV264I2cAndSpi_Init(
		Camera_OV264I2cAndSpiCom_t *instance,
		I_I2c_t *configI2c,
		I_Spi_t *dataSpi,
		GpioChannel_t spiCsChannel,
		I_GpioGroup_t *gpioGroup,
		TimerModule_t *timerModule)
{
	instance->interface.api = &api;
	instance->configI2c = configI2c;
	instance->dataSpi = dataSpi;
	instance->spiCsChannel = spiCsChannel;

	TimerPeriodic_Init(
		&instance->checkIfCameraCaptureIsDoneTimer,
		timerModule,
		PollCameraForCaptureDoneTimeMs,
		PollCameraForCaptureDone,
		instance);

	EventSubscriber_Synchronous_Init(
		&instance->spiBurstReceiveDoneSubscriber,
		PublishImageData,
		instance);

	Event_Synchronous_Init(&instance->onImageCaptureDoneEvent);

	instance->registerDataIndex = 0;
	WriteCameraConfig(instance);
}
