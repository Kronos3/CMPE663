//
// Created by tumbar on 9/2/21.
//

#ifndef CMPE663_TYPES_H
#define CMPE663_TYPES_H

#include <stdint.h>

typedef int8_t I8;
typedef uint8_t U8;
typedef int16_t I16;
typedef uint16_t U16;
typedef int32_t I32;
typedef uint32_t U32;

typedef float F32;
typedef double F64;

#define COMPILE_ASSERT(expr, name) typedef char __compile_assert_##name[(expr) ? 0 : -1]

#endif //CMPE663_TYPES_H
