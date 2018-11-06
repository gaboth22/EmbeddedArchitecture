#include "LcdDisplayController.h"
#include "I_LcdDisplayParallel2Line.h"

void LcdDisplayController_SetCursorState(LcdDisplayController_t *instance, bool state)
{
    instance->lcdCommand[0] = state == true ? 0x0F : 0x0C;
    LcdDisplayParallel2Line_WriteCommand(instance->display, instance->lcdCommand, 1);
}

void LcdDisplayController_ClearDiplay(LcdDisplayController_t *instance)
{
    instance->lcdCommand[0] = 0x01;
    LcdDisplayParallel2Line_WriteCommand(instance->display, instance->lcdCommand, 1);
    instance->lcdCommand[0] = 0x0F;
    LcdDisplayParallel2Line_WriteCommand(instance->display, instance->lcdCommand, 1);
}

void LcdDisplayController_SetCursorIndex(LcdDisplayController_t *instance, uint8_t lineNumber, uint8_t index)
{
    instance->lcdCommand[0] = lineNumber == 1 ? 0x80 : 0xC0;
    instance->lcdCommand[0] |= index;
    LcdDisplayParallel2Line_WriteCommand(instance->display, instance->lcdCommand, 1);
}

void LcdDisplayController_WriteString(LcdDisplayController_t *instance, const char *string, uint8_t strLength)
{
    LcdDisplayParallel2Line_WriteData(instance->display, string, strLength);
}

void LcdDisplayController_Init(LcdDisplayController_t *instance, I_LcdDisplayParallel2Line_t *display)
{
    instance->display = display;
}
