#include "SensorDataController.h"
#include "utils.h"
#include "math.h"

enum
{
    distanceCap = 99
};

//static void Uint8To2LenghtString(uint8_t data, char *buffer)
//{
//    uint8_t byte1 = (data / 10);
//    uint8_t byte0 = (data % 10);
//
//    buffer[0] = (byte1 + '0');
//    buffer[1] = (byte0 + '0');
//}

static void SensorDataClearLcd(void *context)
{
    RECAST(instance, context, SensorDataController_t *);

    LcdDisplayController_WriteString(instance->lcdDisplayController, instance->leftDistanceIntToString, 2);
//    LcdDisplayController_WriteString(instance->lcdDisplayController, , 2);

    TimerOneShot_Stop(&instance->clearLcdOneTimeTimer);
}

static void SensorDataOutputToLcd(void *context)
{
    RECAST(instance, context, SensorDataController_t *);

    DistanceInCm_t rightDistance = DistanceSensor_GetDistanceInCm(&instance->ultraSonicRight->interface);
    DistanceInCm_t leftDistance = DistanceSensor_GetDistanceInCm(&instance->ultraSonicLeft->interface);

    LcdDisplayController_SetCursorIndex(instance->lcdDisplayController, 2, 0);

    uint8_t test1 = (uint8_t)leftDistance /10;
    uint8_t test2 = (uint8_t)leftDistance %10;

    if(leftDistance < distanceCap)
    {
        instance->leftDistanceIntToString[0] = (test1 + '0');
        instance->leftDistanceIntToString[1] = (test2 + '0');

        TimerOneShot_Start(&instance->clearLcdOneTimeTimer);
    }
}

void SensorDataController_Init(
        SensorDataController_t *instance,
        DistanceSensor_SharpGP2Y0A41SK0F_t *frontIrSensor,
        DistanceSensor_UltraSonicHCSR01_t *ultraSonicLeft,
        DistanceSensor_UltraSonicHCSR01_t *ultraSonicRight,
        TimerModule_t *timerModule,
        LcdDisplayController_t *lcdDisplayController)
{
    instance->frontIrSensor = frontIrSensor;
    instance->ultraSonicLeft = ultraSonicLeft;
    instance->ultraSonicRight = ultraSonicRight;

    instance->lcdDisplayController = lcdDisplayController;

    instance->clearLcdExpiryMs = 50;
    instance->sensorDataOutputToLcdExpiryMs = 250;

    TimerPeriodic_Init(
        &instance->doSensorDataOutputToLcd,
        timerModule,
        instance->sensorDataOutputToLcdExpiryMs,
        SensorDataOutputToLcd,
        instance);

    TimerOneShot_Init(
        &instance->clearLcdOneTimeTimer,
        timerModule,
        instance->clearLcdExpiryMs,
        SensorDataClearLcd,
        instance);

    TimerPeriodic_Start(&instance->doSensorDataOutputToLcd);
}
