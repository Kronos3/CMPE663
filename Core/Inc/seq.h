//
// Created by tumbar on 9/22/21.
//

#ifndef CMPE663_SEQ_H
#define CMPE663_SEQ_H

#include "mot.h"
#include "types.h"

// This lab only allows a single level of loops
#define LOOP_LEVELS (1)

typedef struct Sequence_prv Sequence;

typedef enum
{
    SEQ_STATUS_COMMAND_ERR      = -2,   //!< A command ran into an issue
    SEQ_STATUS_NESTED_LOOP_ERR  = -1,   //!< Too many nested loops detected
    SEQ_STATUS_PAUSED           = 0,    //!< Sequence paused by user
    SEQ_STATUS_RUNNING          = 1,    //!< Sequence currently running
    SEQ_STATUS_FINISHED         = 2,    //!< Sequence has reached end
} SeqStatus;

typedef enum
{
    OP_MOV          = 0x1, //!< Go to absolute position
    OP_WAIT         = 0x2, //!< Wait for number of tenths of seconds
    OP_LOOP         = 0x4, //!< Loop for N number of times
    OP_END_LOOP     = 0x5, //!< Mark end of loop block
    OP_ERROR_IF     = 0x6, //!< Exit with error if the other motor is at a position
    OP_RECIPE_END   = 0x0, //!< Mark the end of a recipe
} Opcode;

// Mask definitions
enum
{
    CMD_OP_CODE_MASK        = 0xE0,    //!< Upper 3 bits
    CMD_OP_CODE_SHIFT       = 5,
    CMD_PARAM_MASK          = 0x1F,    //!< Lower 5 bits
};

/**
 * Defines the data needed for sequence (recipe) execution
 * Keep track of program counter and loop return position
 */
struct Sequence_prv
{
    const U8* program;  //!< Points to beginning of code

    U8 loop_stack[LOOP_LEVELS * 2];
    U8 ls_idx;          //!< Index in the loop stack
    U8 wait_flag;       //!< Number of 100ms to wait for next command exec

    U32 pc;             //!< Program counter
    SeqStatus status;   //!< Sequence execution status
    MotorId mid;        //!< Selected motor id
};

/**
 * Sequence engine interrupt handler
 * Run a single system clock cycle for a servo
 * @param self Sequence engine to run cycle for
 */
void sequence_step(Sequence* self);

#endif //CMPE663_SEQ_H
