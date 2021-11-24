//
// Created by tumbar on 11/22/21.
//

#include <p5.h>

// STM32 has a math library automatically linked in :)
#include <math.h>

// Buffer to store the actual samples getting sent to the DMA
U32 sample_buf[MAX_SAMPLES];

void p5_compute_trigger(Trigger* self, U32 freq_i)
{
    // Calculate the number of samples needed to max out
    // the DAC at the requested frequency
    F64 n = DAC_SPS / freq_i;

    // We only have a certain buffer size in memory
    // Cap to this sample count
    if (n > MAX_SAMPLES)
    {
        n = MAX_SAMPLES;
    }

    // Round N
    self->n = (U32) (n + 0.5);

    // Calculate a trigger frequency based on the number of samples
    // This should not drop below 79 (1 MHz) because the DAC can't
    // take more than that. It should already be handled by the
    // sample count calculation above
    F64 raw_arr = (CPU_FREQ / (F64) freq_i) / self->n;

    // Subtract one and round (raw_arr is really ARR + 1)
    self->arr = (U32) (raw_arr - 0.5);

    // Assert what was previously stated above
    FW_ASSERT(self->arr >= 79 && "Invalid ARR trigger frequency", self->arr);
}

void p5_square_wave(U32 min_v, U32 max_v, U32 n)
{
    for (U32 i = 0; i < n / 2; i++)
    {
        // Initialize the high half
        sample_buf[i] = max_v;

        // Initialize the low half
        sample_buf[i + n / 2] = min_v;
    }
}

void p5_triangle_wave(U32 min_v, U32 max_v, U32 n)
{
    F64 x = min_v;
    F64 dx = ((F64) (max_v - min_v)) / (n / 2.0);
    for (U32 i = 0; i < n / 2; i++)
    {
        // Initialize the first half of the wave (rising)
        sample_buf[i] = (U32) x;

        // Initialize the second half of the wave (falling)
        sample_buf[n - i] = (U32) x;
        x += dx;
    }
}

void p5_sin_wave(U32 min_v, U32 max_v, U32 n)
{
    F64 dt = PI2 / n;
    F64 a = (max_v - min_v) / 2.0;
    for (U32 i = 0; i < n; i++)
    {
        // 1. The angle in sin is the index * step_distance
        // 2. Make the mid-point of sin be 1
        // 3. Scale sin to the amplitude
        // 4. Move the sinusoid to the correct y-offset
        sample_buf[i] = min_v + (U32) (a * (sin(i * dt) + 1.0));
    }
}
