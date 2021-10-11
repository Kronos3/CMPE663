//
// Created by tumbar on 10/11/21.
//

#include <stm32l476xx.h>
#include "rng.h"

static I32 rng_inited = 0;

void rng_init(void)
{
    // Enable the RNG
    RNG->CR |= RNG_CR_RNGEN;
    rng_inited = 1;
}

static U32 rng_priv_rand(void)
{
    // Check that no occurred on the hardware
    FW_ASSERT(rng_inited && "RNG not initialized");
    FW_ASSERT(!(
            RNG->SR & (RNG_SR_SEIS | RNG_SR_CEIS)
    ) && "RNG Error occurred", RNG->SR, RNG->SR & RNG_SR_SEIS, RNG->SR & RNG_SR_CEIS);

    // Wait for a new random number
    while (!(RNG->SR & RNG_SR_DRDY));

    return RNG->DR;
}

U32 rng_new(U32 min, U32 max)
{
    FW_ASSERT(max > min, min, max);

    U32 random_number = rng_priv_rand();

    // Limit this number between the min and max
    return (random_number % (max - min)) + min;
}
