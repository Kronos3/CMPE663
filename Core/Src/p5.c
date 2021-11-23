//
// Created by tumbar on 11/22/21.
//

#include <p5.h>
#include <math.h>

U32 sample_buf[MAX_SAMPLES];

void p5_compute_trigger(Trigger* self, U32 freq_i)
{
    F64 n = DAC_SPS / freq_i;
    if (n > MAX_SAMPLES)
    {
        n = MAX_SAMPLES;
    }

    F64 raw_arr = (80e6 / (F64) freq_i) / n;

    // Subtract one and round
    self->n = (U32) n;
    self->arr = (U32) (raw_arr - 0.5);
}

void p5_square_wave(U32 min_v, U32 max_v, U32 n)
{
    for (U32 i = 0; i < n / 2; i++)
    {
        sample_buf[i] = max_v;
    }

    for (U32 i = 0; i < n / 2; i++)
    {
        sample_buf[i + n / 2] = min_v;
    }
}

void p5_triangle_wave(U32 min_v, U32 max_v, U32 n)
{
    F64 x = min_v;
    F64 dx = ((F64) (max_v - min_v)) / (n / 2.0);
    for (U32 i = 0; i < n / 2; i++)
    {
        sample_buf[i] = (U32) x;
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
        sample_buf[i] = min_v + (U32) (a * (sin(i * dt) + 1.0));
    }
}
