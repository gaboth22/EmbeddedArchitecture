#ifndef MOTIONCOMMAND_H
#define MOTIONCOMMAND_H

enum MotionCommand
{
    MotionCommand_Uninitialized = 0,
    MotionCommand_Forward,
    MotionCommand_Right,
    MotionCommand_Left
};
typedef uint8_t MotionCommand_t;

#endif
