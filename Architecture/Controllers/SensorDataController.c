#include "SensorDataController.h"
#include "utils.h"
#include "math.h"

enum
{
    distanceCap = 50
};

static void Uint8To2LengthString(uint8_t data, char *buffer)
{
    uint8_t byte1 = (data / 10);
    uint8_t byte0 = (data % 10);

    buffer[0] = (byte1 + '0');
    buffer[1] = (byte0 + '0');
}

static void SensorDataClearLcd(void *context)
{
    RECAST(instance, context, SensorDataController_t *);

    switch(instance->currentDistanceSensorOutputtingToLcd)
    {
        case DistanceSensor_Left:
            LcdDisplayController_WriteString(instance->lcdDisplayController, instance->leftDistanceIntToString, 2);
            instance->currentDistanceSensorOutputtingToLcd = DistanceSensor_Right;
            break;
        case DistanceSensor_Right:
            LcdDisplayController_WriteString(instance->lcdDisplayController, instance->rightDistanceIntToString, 2);
            instance->currentDistanceSensorOutputtingToLcd = DistanceSensor_Front;
            break;
        case DistanceSensor_Front:
            LcdDisplayController_WriteString(instance->lcdDisplayController, instance->frontDistanceIntToString, 2);
            instance->currentDistanceSensorOutputtingToLcd = DistanceSensor_Left;
            break;

        default:
              break;
    }

    TimerOneShot_Stop(&instance->clearLcdOneTimeTimer);
}

static void SensorDataOutputToLcd(void *context)
{
    RECAST(instance, context, SensorDataController_t *);

    uint8_t byte1;
    uint8_t byte0;

    DistanceInCm_t rightDistance = DistanceSensor_GetDistanceInCm(&instance->ultraSonicRight->interface);
    DistanceInCm_t leftDistance = DistanceSensor_GetDistanceInCm(&instance->ultraSonicLeft->interface);
    DistanceInCm_t frontDistance = DistanceSensor_GetDistanceInCm(&instance->frontIrSensor->interface);

    switch(instance->currentDistanceSensorOutputtingToLcd)
    {
        case DistanceSensor_Left:
            LcdDisplayController_SetCursorIndex(instance->lcdDisplayController, 2, 0);

            if(leftDistance < distanceCap)
            {
                byte1 = (uint8_t)leftDistance /10;
                byte0 = (uint8_t)leftDistance %10;
                instance->leftDistanceIntToString[0] = (byte1 + '0');
                instance->leftDistanceIntToString[1] = (byte0 + '0');
            }
            else{
                instance->leftDistanceIntToString[0] = 'n';
                instance->leftDistanceIntToString[1] = 'a';
            }
            break;

        case DistanceSensor_Right:
            LcdDisplayController_SetCursorIndex(instance->lcdDisplayController, 2, 14);

            if(rightDistance < distanceCap)
            {
                byte1 = (uint8_t)rightDistance /10;
                byte0 = (uint8_t)rightDistance %10;
                instance->rightDistanceIntToString[0] = (byte1 + '0');
                instance->rightDistanceIntToString[1] = (byte0 + '0');
            }
            else
            {
                instance->rightDistanceIntToString[0] = 'n';
                instance->rightDistanceIntToString[1] = 'a';
            }
            break;

        case DistanceSensor_Front:
            LcdDisplayController_SetCursorIndex(instance->lcdDisplayController, 1, 7);

            if(frontDistance < distanceCap)
            {
                byte1 = (uint8_t)frontDistance /10;
                byte0 = (uint8_t)frontDistance %10;
                instance->frontDistanceIntToString[0] = (byte1 + '0');
                instance->frontDistanceIntToString[1] = (byte0 + '0');
            }
            else
            {
                instance->frontDistanceIntToString[0] = 'n';
                instance->frontDistanceIntToString[1] = 'a';
            }
            break;

        default:
            break;
    }

    TimerOneShot_Start(&instance->clearLcdOneTimeTimer);
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

    instance->currentDistanceSensorOutputtingToLcd = DistanceSensor_Left;

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
