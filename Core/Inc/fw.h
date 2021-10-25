//
// Created by tumbar on 10/25/21.
//

#ifndef CMPE663_FW_H
#define CMPE663_FW_H

#include <types.h>

typedef enum {
    HARD_FAULT,
    MEM_MANAGE,
    BUS_FAULT,
    USAGE_FAULT,
} fault_t;

typedef struct {
    U32 r0;
    U32 r1;
    U32 r2;
    U32 r3;
    U32 r12;
    U32 lr;
    U32 pc;
    U32 xpsr;
} ContextFrame;

void print_context_and_fault(
        const ContextFrame* ctx,
        fault_t context_type);

#endif //CMPE663_FW_H
