//
// Created by tumbar on 9/22/21.
//

#include "seq.h"

#define VALIDATE_RANGE(_x, _min, _max) do { \
    if ((_x) < (_min) || (_x) > (_max))     \
        return (STATUS_FAILURE); \
} while(0)

static Status mov_handler(const Sequence* self, U8 target_position)
{
    FW_ASSERT(self);
    VALIDATE_RANGE(target_position, 0, 5);

    // Tell the motor driver to set the motor position
    mot_set_position(self->mid, target_position);
    return STATUS_SUCCESS;
}

static Status wait_handler(Sequence* self, U8 tenth_seconds)
{
    FW_ASSERT(self);
    FW_ASSERT(!self->wait_flag);    // We should not be inside a wait
    VALIDATE_RANGE(tenth_seconds, 0, 31);

    // Tell the sequencer to wait for this amount of time
    self->wait_flag = tenth_seconds;

    return STATUS_SUCCESS;
}

static Status loop_handler(Sequence* self, U8 n)
{
    FW_ASSERT(self);
    VALIDATE_RANGE(n, 0, 31);

    // Push the loop index and the loop address to the stack
    self->loop_stack[self->ls_idx++] = self->pc; // loop address
    self->loop_stack[self->ls_idx++] = n; // i = n

    return STATUS_SUCCESS;
}

static Status loop_end_handler(Sequence* self)
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

    return STATUS_SUCCESS;
}

static Status recipe_end_handler(Sequence* self)
{
    FW_ASSERT(self);
    self->status = SEQ_STATUS_FINISHED;
    return STATUS_SUCCESS;
}

static Status command_dispatch(Sequence* self, U8 command)
{
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
        default:
            return STATUS_INVALID_OPCODE;
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
        switch(command_dispatch(self, self->program[self->pc++]))
        {
            case STATUS_FAILURE:
            case STATUS_INVALID_OPCODE:
                self->status = SEQ_STATUS_COMMAND_ERR;
                break;
            case STATUS_SUCCESS:
                // Do nothing
                break;
        }
    }
}
