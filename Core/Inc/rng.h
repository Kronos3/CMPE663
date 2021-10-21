//
// Created by tumbar on 10/11/21.
//

#ifndef CMPE663_RNG_H
#define CMPE663_RNG_H

#include <types.h>

/**
 * Initialize the random number generator hardware peripheral
 */
void rng_init(void);

/**
 * Generate a new random number inside two bounds
 * Utilized the RNG
 * @param min Minimum bound
 * @param max Maximum bound
 * @return random number in bounds
 */
U32 rng_new(U32 min, U32 max);

#endif //CMPE663_RNG_H
