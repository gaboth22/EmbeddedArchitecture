#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

#include "I_LcdDisplayParallel2Line.h"

typedef struct
{
    I_LcdDisplayParallel2Line_t *display;
    uint8_t lcdCommand[1];
} LcdDisplayController_t;

/*
 * Enable or disable cursor
 */
void LcdDisplayController_SetCursorState(LcdDisplayController_t *instance, bool state);

/*
 * Clear the display
 */
void LcdDisplayController_ClearDiplay(LcdDisplayController_t *instance);

/*
 * @param instance The display controller
 * @param line number, either one or two
 * @param index 0-15 (LCD shows 16 characters per line at a time)
 */
void LcdDisplayController_SetCursorIndex(LcdDisplayController_t *instance, uint8_t lineNumber, uint8_t index);

/*
 * Data will be written to the cursor index set with _SetCursorIndex
 * @param instance The display controller
 * @param string to display
 * @param strLength the length of the string
 */
void LcdDisplayController_WriteString(LcdDisplayController_t *instance, const char *string, uint8_t strLength);

/*
 * @param instance The display controller
 * @param display The display to use
 */
void LcdDisplayController_Init(LcdDisplayController_t *instance, I_LcdDisplayParallel2Line_t *display);

#endif
