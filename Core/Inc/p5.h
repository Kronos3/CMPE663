//
// Created by tumbar on 11/3/21.
//

#ifndef CMPE663_P5_H
#define CMPE663_P5_H

#include <types.h>

#define DYNAMIC_RANGE (1 << 12)

#define PI (3.141592653589793)
#define PI2 (6.283185307179586)

// Maximum number of samples for an entire cycle
#define MAX_SAMPLES (2000)

extern U32 sample_buf[MAX_SAMPLES];

// Number of samples per second the DAC can produce (1 MSPS)
// Attempt to keep the DAC running at maximum
// If the frequency is too high, lower the number of samples
#define DAC_SPS 1e6

typedef struct {
    U32 arr;            //!< Timer auto-reload register, determines sample frequency
    U32 n;              //!< Number of samples per cycle
} Trigger;

/**
 * Assuming no prescaler (0) on a 80MHz clock,
 * calculate the ARR value to give a certain wave
 * frequency with BUF_SIZE samples
 * @param self Trigger structure to initialize
 * @param frequency in Hz
 */
void p5_compute_trigger(Trigger* self, U32 frequency);

/**
 * Square wave (starts at max_v)
 * @param min_v minimum voltage
 * @param max_v maximum voltage
 * @param n number of samples in the buffer
 */
void p5_square_wave(U32 min_v, U32 max_v, U32 n);

/**
 * Triangle wave
 * @param min_v minimum voltage
 * @param max_v maximum voltage
 * @param n number of samples in the buffer
 */
void p5_triangle_wave(U32 min_v, U32 max_v, U32 n);

/**
 * Sine wave
 * @param min_v minimum voltage
 * @param max_v maximum voltage
 * @param n number of samples in the buffer
 */
void p5_sin_wave(U32 min_v, U32 max_v, U32 n);

#endif //CMPE663_P5_H
