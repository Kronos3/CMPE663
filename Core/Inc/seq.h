//
// Created by tumbar on 9/22/21.
//

#ifndef CMPE663_SEQ_H
#define CMPE663_SEQ_H

#include "mot.h"
#include "types.h"

typedef struct Sequence_prv Sequence;

typedef enum
{
    STATUS_FAILURE = -1,
    STATUS_INVALID_OPCODE = -2,
    STATUS_SUCCESS = 0,
} Status;

typedef enum
{
    SEQ_STATUS_COMMAND_ERR = -2,
    SEQ_STATUS_NESTED_LOOP_ERR = -1,
    SEQ_STATUS_PAUSED = 0,
    SEQ_STATUS_RUNNING = 1,
    SEQ_STATUS_FINISHED = 2,
} SeqStatus;

typedef enum
{
    OP_MOV = 0x1,
    OP_WAIT = 0x2,
    OP_LOOP = 0x4,
    OP_END_LOOP = 0x5,
    OP_RECIPE_END = 0x0,
} Opcode;

// Mask definitions
enum
{
    CMD_OP_CODE_MASK = 0xE0,    // Upper 3 bits
    CMD_OP_CODE_SHIFT = 5,
    CMD_PARAM_MASK = 0x1F,      // Lower 5 bits
};

/**
 * Defines the data needed for sequence (recipe) execution
 * Keep track of program counter and loop return position
 */
struct Sequence_prv
{
    const U8* program;  //!< Points to beginning of code

    // TODO(tumbar) Remove support loop nesting
    U8 loop_stack[16];  //!< 8 levels of nested loops allowed
    U8 ls_idx;          //!< Index in the loop stack

    U8 pc;              //!< Program counter (Sequences may only be 256 commands)

    U8 wait_flag;       //!< Number of 100ms to wait for next command exec
    SeqStatus status;   //!< Sequence execution status
    MotorId mid;        //!< Selected motor id
};

/**
 * Sequence engine interrupt handler
 * Run a single system clock cycle for a servo
 * @param self
 */
void sequence_step(Sequence* self);

#endif //CMPE663_SEQ_H
