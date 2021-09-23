//
// Created by tumbar on 9/23/21.
//

#include "recipe.h"
#include "seq.h"

#define OP(opcode, parameter) (((opcode) << CMD_OP_CODE_SHIFT) & CMD_OP_CODE_MASK) | (parameter & (CMD_PARAM_MASK))

const U8 servo_1_recipe[] = {
        OP(OP_MOV, 0),
        OP(OP_MOV, 5),
        OP(OP_MOV, 0),
        OP(OP_MOV, 3),
        OP(OP_LOOP, 0), // test default loop behaviour
        OP(OP_MOV, 1),
        OP(OP_MOV, 4),
        OP(OP_END_LOOP, 0),
        OP(OP_MOV, 0),
        OP(OP_MOV, 2),
        OP(OP_WAIT, 0),
        OP(OP_MOV, 2),
        OP(OP_MOV, 3),
        OP(OP_WAIT, 31),
        OP(OP_WAIT, 31),
        OP(OP_WAIT, 31),
        OP(OP_MOV, 4),
        OP(OP_RECIPE_END, 0)
};

const U8 servo_2_recipe[] = {
        OP(OP_LOOP, 5),
        OP(OP_MOV, 0),
        OP(OP_MOV, 1),
        OP(OP_MOV, 2),
        OP(OP_MOV, 3),
        OP(OP_MOV, 4),
        OP(OP_MOV, 5),
        OP(OP_END_LOOP, 5),
        OP(OP_RECIPE_END, 0)
};
