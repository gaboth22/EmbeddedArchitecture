#include "Camera_OV264I2cAndSpiCom.h"
#include "CameraOV264I2cAndSpiComRegisterData.h"
#include "Assert.h"
#include "utils.h"
#include "types.h"

enum
{
    CameraModuleTestRegAddress = 0x00,
	CameraSensorWriteAddress = 0x60,
	CameraSensorCpldResetRegAddress = 0x07,
	DataTerminator = 0xFF,
	CamFifoAddress = 0x04,
	CamFifoClearMask = 0x01,
	CamFifoStartMask = 0x02,
	CamBurstFifoRead = 0x3C,
	CamCaptureRegisterAddress = 0x41,
	CamCaptureDoneMask = 0x08,
	CamFifoSizeRegAddress1 = 0x42,
	CamFifoSizeRegAddress2 = 0x43,
	CamFifoSizeRegAddress3 = 0x44,
	PollCameraForCaptureDoneTimeMs = 10,
	TimeToWaitInBetweenConfigurationsMs = 100,
	SpiWriteMask = 0x80,
	SpiReadMask = 0x7F,
	TimeToWaitBeforeResettingCameraFifoMs = 1000,
	MaxImageSize = 4096
};

enum CameraConfigState
{
    CameraConfigState_Uninitialized = 0,
    CameraConfigState_ResetCpld1,
    CameraConfigState_ResetCpld2,
    CameraConfigState_CheckSpiBus,
    CameraConfigState_InitialConfig,
    CameraConfigState_ConfigureJpeg,
    CameraConfigState_SetYuv422Format,
    CameraConfigState_SetJpegMode,
    CameraConfigState_SetJpegSize,
    CameraConfigState_ResetCameraFifo
};
typedef uint8_t CameraConfigState_t;

/*
 * Buffer to place image in
 */
static uint8_t receivedImage[MaxImageSize]; // Since images are hard-coded to 160x120 they won't be more than 4K
/*
 *
 */
static void StartImageCapture(I_Camera_t *_instance);

static void WriteToCameraSpiRegister(Camera_OV264I2cAndSpiCom_t *instance, uint8_t registerAddress, uint8_t data)
{
    Spi_SetChipSelectState(instance->dataSpi, ChipSelectState_Low);
    Spi_SendByte(instance->dataSpi, registerAddress | SpiWriteMask);
    Spi_SendByte(instance->dataSpi, data);
    while(Spi_IsBusy(instance->dataSpi));
    Spi_SetChipSelectState(instance->dataSpi, ChipSelectState_High);
}

static uint8_t ReadFromCameraSpiRegister(Camera_OV264I2cAndSpiCom_t *instance, uint8_t registerAddress)
{
    uint8_t byte = 0;
    Spi_SetChipSelectState(instance->dataSpi, ChipSelectState_Low);
    Spi_SendByte(instance->dataSpi, registerAddress & SpiReadMask);
    byte = Spi_GetByte(instance->dataSpi);
    while(Spi_IsBusy(instance->dataSpi));
    Spi_SetChipSelectState(instance->dataSpi, ChipSelectState_High);

    return byte;
}

static void PublishImageData(void *context, void *args)
{
	RECAST(instance, context, Camera_OV264I2cAndSpiCom_t *);
	RECAST(spiInfo, args, SpiBurstReceiveInfo_t *);

	instance->currentImageData.image = spiInfo->dataAddress;
	instance->currentImageData.imageSize = spiInfo->dataSize;

	WriteToCameraSpiRegister(instance, CamFifoAddress, CamFifoClearMask);

	Event_Unsubscribe(
		Spi_GetOnBurstReceiveDoneEvent(instance->dataSpi),
		&instance->spiBurstReceiveDoneSubscriber.interface);
	Event_Publish(&instance->onImageCaptureDoneEvent.interface, &instance->currentImageData);
}

static void PollCameraForCaptureDone(void *context)
{
	RECAST(instance, context, Camera_OV264I2cAndSpiCom_t *);
	uint8_t captureDone = ReadFromCameraSpiRegister(instance, CamCaptureRegisterAddress) & CamCaptureDoneMask;

	if(captureDone > 0)
	{
		TimerPeriodic_Command(&instance->configTimer, TimerPeriodicCommand_Stop);

		uint16_t len1 = ReadFromCameraSpiRegister(instance, CamFifoSizeRegAddress1);
		uint16_t len2 = ReadFromCameraSpiRegister(instance, CamFifoSizeRegAddress2);
		uint16_t totalLength = ((len2 << 8) | len1);

		if(totalLength <= MaxImageSize)
		{
            Event_Subscribe(
                Spi_GetOnBurstReceiveDoneEvent(instance->dataSpi),
                &instance->spiBurstReceiveDoneSubscriber.interface);

            Spi_SendByte(instance->dataSpi, CamBurstFifoRead);
            Spi_GetByte(instance->dataSpi); // Get first bogus byte from camera
            //Spi_StartBurstReceive(instance->dataSpi, receivedImage, totalLength);
		}
		else
		{
		    while(Spi_IsBusy(instance->dataSpi));
		    StartImageCapture(&instance->interface);
		}
	}
}

static void WriteCameraConfig(void *context)
{
	RECAST(instance, context, Camera_OV264I2cAndSpiCom_t *);

	uint8_t regAddress = 0x00;
	uint8_t regData = 0x00;

	switch(instance->cameraConfigState)
	{
	    case CameraConfigState_InitialConfig:
	        TimerPeriodic_Command(&instance->configTimer, TimerPeriodicCommand_Stop);
	        regAddress = InitialConfig[instance->registerDataIndex].reg;
            regData = InitialConfig[instance->registerDataIndex].data;

            if(regAddress == DataTerminator && regData == DataTerminator)
            {
                instance->registerDataIndex = 0;
                instance->cameraConfigState = CameraConfigState_ConfigureJpeg;

                TimerPeriodic_Init(
                    &instance->configTimer,
                    instance->timerModule,
                    TimeToWaitInBetweenConfigurationsMs,
                    WriteCameraConfig,
                    instance);

                TimerPeriodic_Start(&instance->configTimer);
            }
            else
            {
                instance->registerDataIndex++;
                I2c_WriteByte(
                    instance->configI2c,
                    CameraSensorWriteAddress,
                    regAddress,
                    regData,
                    WriteCameraConfig,
                    instance);
            }
	        break;

	    case CameraConfigState_ConfigureJpeg:
	        TimerPeriodic_Command(&instance->configTimer, TimerPeriodicCommand_Stop);
	        regAddress = ConfigureJpegFormat[instance->registerDataIndex].reg;
            regData = ConfigureJpegFormat[instance->registerDataIndex].data;

            if(regAddress == DataTerminator && regData == DataTerminator)
            {
                instance->registerDataIndex = 0;
                instance->cameraConfigState = CameraConfigState_SetYuv422Format;

                TimerPeriodic_Init(
                    &instance->configTimer,
                    instance->timerModule,
                    TimeToWaitInBetweenConfigurationsMs,
                    WriteCameraConfig,
                    instance);

                TimerPeriodic_Start(&instance->configTimer);
            }
            else
            {
                instance->registerDataIndex++;
                I2c_WriteByte(
                    instance->configI2c,
                    CameraSensorWriteAddress,
                    regAddress,
                    regData,
                    WriteCameraConfig,
                    instance);
            }

	        break;

	    case CameraConfigState_SetYuv422Format:
	        TimerPeriodic_Command(&instance->configTimer, TimerPeriodicCommand_Stop);
            regAddress = ConfigureYuv422Format[instance->registerDataIndex].reg;
            regData = ConfigureYuv422Format[instance->registerDataIndex].data;

            if(regAddress == DataTerminator && regData == DataTerminator)
            {
                instance->registerDataIndex = 0;
                instance->cameraConfigState = CameraConfigState_SetJpegMode;

                TimerPeriodic_Init(
                    &instance->configTimer,
                    instance->timerModule,
                    TimeToWaitInBetweenConfigurationsMs,
                    WriteCameraConfig,
                    instance);

                TimerPeriodic_Start(&instance->configTimer);
            }
            else
            {
                instance->registerDataIndex++;
                I2c_WriteByte(
                    instance->configI2c,
                    CameraSensorWriteAddress,
                    regAddress,
                    regData,
                    WriteCameraConfig,
                    instance);
            }
	        break;

	    case CameraConfigState_SetJpegMode:
	        TimerPeriodic_Command(&instance->configTimer, TimerPeriodicCommand_Stop);
            regAddress = SetJpegMode[instance->registerDataIndex].reg;
            regData = SetJpegMode[instance->registerDataIndex].data;

            if(regAddress == DataTerminator && regData == DataTerminator)
            {
                instance->registerDataIndex = 0;
                instance->cameraConfigState = CameraConfigState_SetJpegSize;

                TimerPeriodic_Init(
                    &instance->configTimer,
                    instance->timerModule,
                    TimeToWaitInBetweenConfigurationsMs,
                    WriteCameraConfig,
                    instance);

                TimerPeriodic_Start(&instance->configTimer);
            }
            else
            {
                instance->registerDataIndex++;
                I2c_WriteByte(
                    instance->configI2c,
                    CameraSensorWriteAddress,
                    regAddress,
                    regData,
                    WriteCameraConfig,
                    instance);
            }
	        break;

	    case CameraConfigState_SetJpegSize:
	        TimerPeriodic_Command(&instance->configTimer, TimerPeriodicCommand_Stop);
	        regAddress = ConfigureJpeg160x120[instance->registerDataIndex].reg;
	        regData = ConfigureJpeg160x120[instance->registerDataIndex].data;

            if(regAddress == DataTerminator && regData == DataTerminator)
            {
                instance->cameraConfigState = CameraConfigState_ResetCameraFifo;
                instance->registerDataIndex = 0;
                TimerPeriodic_Init(
                        &instance->configTimer,
                        instance->timerModule,
                        TimeToWaitBeforeResettingCameraFifoMs,
                        WriteCameraConfig,
                        instance);

                TimerPeriodic_Start(&instance->configTimer);
            }
            else
            {
                instance->registerDataIndex++;
                I2c_WriteByte(
                    instance->configI2c,
                    CameraSensorWriteAddress,
                    regAddress,
                    regData,
                    WriteCameraConfig,
                    instance);
            }
            break;

	    case CameraConfigState_ResetCameraFifo:
	        TimerPeriodic_Command(&instance->configTimer, TimerPeriodicCommand_Stop);
	        instance->registerDataIndex = 0;
	        instance->cameraConfigState = CameraConfigState_Uninitialized;

            WriteToCameraSpiRegister(instance, CamFifoAddress, CamFifoClearMask);
            WriteToCameraSpiRegister(instance, 0x01, 0x00);
            while(Spi_IsBusy(instance->dataSpi));
            instance->busy = false;
	        break;

	    default:
	        break;
	}
}

static void CheckCameraSpiBus(Camera_OV264I2cAndSpiCom_t *instance)
{
    WriteToCameraSpiRegister(instance, CameraModuleTestRegAddress, 0x55);
    uint8_t byte = ReadFromCameraSpiRegister(instance, CameraModuleTestRegAddress);
    Assert((byte == 0x55));

    instance->cameraConfigState = CameraConfigState_InitialConfig;
    WriteCameraConfig(instance);
}

static void ResetCameraCpldRegister(void *context)
{
    RECAST(instance, context, Camera_OV264I2cAndSpiCom_t *);
    switch(instance->cameraConfigState)
    {
        case CameraConfigState_ResetCpld1:
            instance->cameraConfigState = CameraConfigState_ResetCpld2;
            WriteToCameraSpiRegister(instance, 0x07, 0x80);
            break;
        case CameraConfigState_ResetCpld2:
            instance->cameraConfigState = CameraConfigState_CheckSpiBus;
            WriteToCameraSpiRegister(instance, 0x07, 0x00);
            break;
        default:
            TimerPeriodic_Command(&instance->configTimer, TimerPeriodicCommand_Stop);
            CheckCameraSpiBus(instance);
            break;
    }
}

static void ResetCameraCpld(Camera_OV264I2cAndSpiCom_t *instance)
{
    instance->cameraConfigState = CameraConfigState_ResetCpld1;

    TimerPeriodic_Init(
        &instance->configTimer,
        instance->timerModule,
        TimeToWaitInBetweenConfigurationsMs,
        ResetCameraCpldRegister,
        instance);

    TimerPeriodic_Start(&instance->configTimer);
}

static void SetConfiguration(I_Camera_t *_instance, const void *configuration)
{
	IGNORE(_instance);
	IGNORE(configuration);
}

static void CheckIfModuleBusy(void *context)
{
    RECAST(instance, context, Camera_OV264I2cAndSpiCom_t *);

    if(!instance->busy)
    {
        TimerPeriodic_Command(&instance->pollTimer, TimerPeriodicCommand_Stop);
        StartImageCapture(&instance->interface);
    }
}

static void StartImageCapture(I_Camera_t *_instance)
{
	RECAST(instance, _instance, Camera_OV264I2cAndSpiCom_t *);

	if(instance->busy)
	{
	    TimerPeriodic_Init(
            &instance->pollTimer,
            instance->timerModule,
            TimeToWaitInBetweenConfigurationsMs,
            CheckIfModuleBusy,
            instance);

        TimerPeriodic_Start(&instance->pollTimer);
	}
	else
	{
	    WriteToCameraSpiRegister(instance, CamFifoAddress, CamFifoClearMask);
	    WriteToCameraSpiRegister(instance, CamFifoAddress, 0x10);
	    WriteToCameraSpiRegister(instance, CamFifoAddress, CamFifoStartMask);

        TimerPeriodic_Init(
            &instance->configTimer,
            instance->timerModule,
            PollCameraForCaptureDoneTimeMs,
            PollCameraForCaptureDone,
            instance);

        TimerPeriodic_Start(&instance->configTimer);
	}
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
		TimerModule_t *timerModule)
{
    instance->busy = true;
	instance->interface.api = &api;
	instance->configI2c = configI2c;
	instance->dataSpi = dataSpi;
	instance->spiCsChannel = spiCsChannel;
	instance->timerModule = timerModule;
	instance->registerDataIndex = 0;
	instance->cameraConfigState = CameraConfigState_Uninitialized;

	EventSubscriber_Synchronous_Init(
		&instance->spiBurstReceiveDoneSubscriber,
		PublishImageData,
		instance);

	Event_Synchronous_Init(&instance->onImageCaptureDoneEvent);

    ResetCameraCpld(instance);
}
