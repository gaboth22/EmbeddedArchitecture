#ifndef SENSORDATACONTROLLER_H
#define SENSORDATACONTROLLER_H

#include "TimerModule.h"
#include "TimerPeriodic.h"
#include "DistanceSensor_SharpGP2Y0A41SK0F.h"
#include "DistanceSensor_UltraSonicHCSR01.h"
#include "I_DistanceSensor.h"
#include "LcdDisplayController.h"
#include "TimerOneShot.h"

typedef struct
{
    DistanceSensor_SharpGP2Y0A41SK0F_t *frontIrSensor;
    DistanceSensor_UltraSonicHCSR01_t *ultraSonicLeft;
    DistanceSensor_UltraSonicHCSR01_t *ultraSonicRight;
    TimerPeriodic_t doSensorDataOutputToLcd;
    TimerOneShot_t clearLcdOneTimeTimer;
    TimerTicks_t clearLcdExpiryMs;
    TimerTicks_t sensorDataOutputToLcdExpiryMs;
    LcdDisplayController_t *lcdDisplayController;
    char leftDistanceIntToString[2];
    char rightDistanceIntToString;
} SensorDataController_t;

void SensorDataController_Init(
        SensorDataController_t *instance,
        DistanceSensor_SharpGP2Y0A41SK0F_t *frontIrSensor,
        DistanceSensor_UltraSonicHCSR01_t *ultraSonicLeft,
        DistanceSensor_UltraSonicHCSR01_t *ultraSonicRight,
        TimerModule_t *timerModule,
        LcdDisplayController_t *lcdDisplayController);

#endif
