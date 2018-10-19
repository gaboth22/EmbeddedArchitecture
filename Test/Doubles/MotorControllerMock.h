#include "CppUTestExt/MockSupport.h"

extern "C"
{
#include "I_MotorController.h"
#include "utils.h"
}

class MotorControllerMock
{
public:
    I_MotorController_t motorController;
    MotorControllerMock();
    void SetBusyState(bool newState);
};
