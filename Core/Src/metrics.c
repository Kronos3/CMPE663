//
// Created by tumbar on 11/11/21.
//

#include <metrics.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <uart.h>

static I32 iter_n = 0;
static volatile I32 metric_ready = 0;
static const U16* volatile metric_dac_buf = NULL;
static const U16* volatile metric_adc_buf = NULL;

static U16 metric_adc_scratch[BUF_SIZE];

void metrics_compute(const U16 dac_buf[BUF_SIZE], const U16 adc_buf[BUF_SIZE])
{
    FW_ASSERT(!metric_ready && "Metric overflow");
    FW_ASSERT(!metric_dac_buf);
    FW_ASSERT(!metric_dac_buf);

    metric_dac_buf = dac_buf;
    metric_adc_buf = adc_buf;
    metric_ready = 1;
}

int cmpfunc(const void* a, const void* b)
{
    return (*(U16*) a - *(U16*) b);
}

U32 isqrt(U32 a)
{
    U32 op = a;
    U32 res = 0;
    // The second-to-top bit is set: use 1u << 14 for U16 type
    // use 1uL << 30 for U32 type
    U32 one = 1uL << 30;

    // "one" starts at the highest power of four <= than the argument.
    while (one > op)
    {
        one >>= 2;
    }

    while (one != 0)
    {
        if (op >= res + one)
        {
            op = op - (res + one);
            res = res + 2 * one;
        }
        res >>= 1;
        one >>= 2;
    }
    return res;
}

void metrics_task(void)
{
    if (!metric_ready) return;

    FW_ASSERT(metric_dac_buf);
    FW_ASSERT(metric_adc_buf);

    // Copy data from ADC buffer into scratch space
    memcpy(metric_adc_scratch, metric_adc_buf, sizeof(metric_adc_scratch));

    // Sort the data to help compute the median
    qsort(metric_adc_scratch, BUF_SIZE, sizeof(U16), cmpfunc);

    // Compute the metrics
    U32 mean_sum = 0;
    U32 absolute_difference_sum = 0;

    for (U32 i = 0; i < BUF_SIZE; i++)
    {
        mean_sum += metric_adc_buf[i];
        absolute_difference_sum += ABS(metric_dac_buf[i] - metric_adc_buf[i]);
    }

    // Compute mean and variance
    U32 mean = mean_sum / BUF_SIZE;
    U32 variance_sum = 0;
    for (U32 i = 0; i < BUF_SIZE; i++)
    {
        U32 diff = (metric_adc_buf[i] - mean);
        variance_sum += diff * diff;
    }

    U32 variance = isqrt(variance_sum / BUF_SIZE);
    U32 median = metric_adc_scratch[BUF_SIZE / 2];

    uprintf("=============\r\n"
            "Iteration #%d\r\n"
            "Absolute difference: %d\r\n"
            "Mean: %d\r\n"
            "Median: %d\r\n"
            "Variance: %d\r\n",
            ++iter_n,
            absolute_difference_sum,
            mean,
            median,
            variance);

    // Reset metrics queue
    metric_dac_buf = NULL;
    metric_adc_buf = NULL;
    metric_ready = 0;

    adc_dac_next_state();
}
