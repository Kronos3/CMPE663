//
// Created by tumbar on 11/3/21.
//

#ifndef CMPE663_P5_H
#define CMPE663_P5_H

#include <types.h>

#define DYNAMIC_RANGE (1 << 12)

#define BUF_SIZE (DYNAMIC_RANGE * 2)

#define PI (3.14159265359)

extern U32 sample_buf[BUF_SIZE];

/**
 * Assuming no prescaler (0) on a 80MHz clock,
 * calculate the ARR value to give a certain wave
 * frequency with BUF_SIZE samples
 * @param freq
 */
void p5_compute_arr_psc(U32 freq);

/**
 * Square wave (starts at max_v)
 * @param min_v minimum voltage
 * @param max_v maximum voltage
 */
void p5_square_wave(U32 min_v, U32 max_v);

/**
 * Triangle wave
 * @param min_v minimum voltage
 * @param max_v maximum voltage
 */
void p5_triangle_wave(U32 min_v, U32 max_v);

/**
 * Sine wave
 * @param min_v minimum voltage
 * @param max_v maximum voltage
 */
void p5_sin_wave(U32 min_v, U32 max_v);

#endif //CMPE663_P5_H
