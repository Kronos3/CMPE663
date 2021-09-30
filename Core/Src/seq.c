//
// Created by tumbar on 9/22/21.
//

#include "seq.h"

#define VALIDATE_RANGE(_x, _min, _max) do { \
    if ((_x) < (_min) || (_x) > (_max))     \
        return (SEQ_STATUS_COMMAND_ERR);    \
} while(0)

static SeqStatus mov_handler(const Sequence* self, U8 target_position)
{
    FW_ASSERT(self);
    VALIDATE_RANGE(target_position, 0, 5);

    // Tell the motor driver to set the motor position
    mot_set_position(self->mid, target_position);
    return SEQ_STATUS_RUNNING;
}

static SeqStatus wait_handler(Sequence* self, U8 tenth_seconds)
{
    FW_ASSERT(self);
    FW_ASSERT(!self->wait_flag);    // We should not be inside a wait
    VALIDATE_RANGE(tenth_seconds, 0, 31);

    // Tell the sequencer to wait for this amount of time
    self->wait_flag = tenth_seconds;
    return SEQ_STATUS_RUNNING;
}

static SeqStatus loop_handler(Sequence* self, U8 n)
{
    FW_ASSERT(self);
    VALIDATE_RANGE(n, 0, 31);

    if (self->ls_idx >= (sizeof(self->loop_stack) / sizeof(self->loop_stack[0])))
    {
        // No more space on loop stack
        return SEQ_STATUS_NESTED_LOOP_ERR;
    }

    // Push the loop index and the loop address to the stack
    self->loop_stack[self->ls_idx++] = self->pc; // loop address
    self->loop_stack[self->ls_idx++] = n; // i = n

    return SEQ_STATUS_RUNNING;
}

static SeqStatus loop_end_handler(Sequence* self)
{
    FW_ASSERT(self);

    // Make sure there is a loop running
    FW_ASSERT(self->ls_idx % 2 == 0 && self->ls_idx > 0);

    // Read the current information about the running loop
    if (self->loop_stack[self->ls_idx - 1])
    {
        // Decrement the iteration count, branch to the loop start
        self->loop_stack[self->ls_idx - 1]--;
        self->pc = self->loop_stack[self->ls_idx - 2];
    }
    else
    {
        // Stop the loop
        // Pop the information off the stack
        self->ls_idx -= 2;
    }

    return SEQ_STATUS_RUNNING;
}

static SeqStatus recipe_end_handler(Sequence* self)
{
    FW_ASSERT(self);
    return SEQ_STATUS_FINISHED;
}

static SeqStatus error_if_handler(Sequence* self, U8 pos)
{
    FW_ASSERT(self);
    VALIDATE_RANGE(pos, 0, 5);

    // Check the position of the other motor
    MotPos other_pos = mot_get_position((self->mid + 1) % MOT_N);
    if (other_pos == pos)
    {
        return SEQ_STATUS_COMMAND_ERR;
    }

    return SEQ_STATUS_RUNNING;
}

static SeqStatus command_dispatch(Sequence* self, U8 command)
{
    FW_ASSERT(self);

    switch((Opcode)((command & CMD_OP_CODE_MASK) >> CMD_OP_CODE_SHIFT))
    {
        case OP_MOV:
            return mov_handler(self, command & CMD_PARAM_MASK);
        case OP_WAIT:
            return wait_handler(self, command & CMD_PARAM_MASK);
        case OP_LOOP:
            return loop_handler(self, command & CMD_PARAM_MASK);
        case OP_END_LOOP:
            return loop_end_handler(self);
        case OP_RECIPE_END:
            return recipe_end_handler(self);
        case OP_ERROR_IF:
            return error_if_handler(self, command & CMD_PARAM_MASK);
        default:
            FW_ASSERT_N(0 && "Invalid opcode", (Opcode)((command & CMD_OP_CODE_MASK) >> CMD_OP_CODE_SHIFT));
    }

    FW_ASSERT(0);
}

void sequence_step(Sequence* self)
{
    FW_ASSERT(self);
    FW_ASSERT(self->program);

    switch (self->status)
    {
        case SEQ_STATUS_COMMAND_ERR:
        case SEQ_STATUS_NESTED_LOOP_ERR:
        case SEQ_STATUS_FINISHED:
        case SEQ_STATUS_PAUSED:
            // Don't execute anything
            return;
        case SEQ_STATUS_RUNNING:
            // Continue with execution
            break;
        default:
            // Invalid state
            FW_ASSERT(0);
    }

    if (self->wait_flag)
    {
        // Wait for the next 100ms interrupt
        self->wait_flag--;
    }
    else
    {
        // Read the next command and dispatch it
        // Handle the response codes
        self->status = command_dispatch(self, self->program[self->pc++]);
    }
}
