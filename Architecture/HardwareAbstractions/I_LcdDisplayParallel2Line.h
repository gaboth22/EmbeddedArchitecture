#ifndef I_LCDDISPLAYPARALLEL2LINE_H
#define I_LCDDISPLAYPARALLEL2LINE_H

#include "types.h"

typedef void (*LcdWriteDoneCallback_t)(void *context);
typedef struct _LcdDisplayParallel2LineApi_t LcdDisplayParallel2LineApi_t;

typedef struct
{
    const LcdDisplayParallel2LineApi_t *api;
} I_LcdDisplayParallel2Line_t;

struct _LcdDisplayParallel2LineApi_t
{
    /*
     * Write command to LCD Display
     * @param instance The LCD instance
     * @param commandList The list of commands to send out
     * @param commandListSize The size of the list of commands
     */
    void (*WriteCommand)(I_LcdDisplayParallel2Line_t *instance, const uint8_t *commandList, uint8_t commandListSize);

    /*
     * Write LCD data to display
     * @param instance The LCD instance
     * @param data The data to display
     * @param dataSize size of data to send out
     */
    void (*WriteData)(I_LcdDisplayParallel2Line_t *instance, const void *data, uint8_t dataSize);

    /*
     * Check whether the LCD is busy
     */
    bool (*IsBusy)(I_LcdDisplayParallel2Line_t *instance);
};

#define LcdDisplayParallel2Line_WriteCommand(_instance, _commands, _commandsSize) \
        (_instance)->api->WriteCommand(_instance, _commands, _commandsSize) \

#define LcdDisplayParallel2Line_WriteData(_instance, _data, _dataSize) \
        (_instance)->api->WriteData(_instance, _data, _dataSize) \

#define LcdDisplayParallel2Line_IsBusy(_instance) \
        (_instance)->api->IsBusy(_instance) \

#endif
