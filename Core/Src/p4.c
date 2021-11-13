//
// Created by tumbar on 11/11/21.
//

#include <p4.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <uart.h>

static I32 iter_n = 0;
static volatile I32 metric_busy = 0;
static const U16* volatile metric_dac_buf = NULL;
static const U16* volatile metric_adc_buf = NULL;

// Keeps track of the number of iterations ran in TEN mode
static I32 ten_counter = 0;

static U16 metric_ten_save[2][BUF_SIZE];

void metrics_compute(const U16 dac_buf[BUF_SIZE], const U16 adc_buf[BUF_SIZE])
{
    FW_ASSERT(!metric_busy && "Metric overflow");
    FW_ASSERT(!metric_dac_buf);
    FW_ASSERT(!metric_dac_buf);

    metric_dac_buf = dac_buf;
    metric_adc_buf = adc_buf;
    metric_busy = 1;
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
    if (!metric_busy) return;

    FW_ASSERT(metric_dac_buf);
    FW_ASSERT(metric_adc_buf);

    // Copy data from ADC buffer into scratch space
    memcpy(metric_ten_save[1], metric_adc_buf, sizeof(metric_ten_save[1]));

    // Sort the data to help compute the median
    qsort(metric_ten_save[1], BUF_SIZE, sizeof(U16), cmpfunc);

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
    U32 median = metric_ten_save[1][BUF_SIZE / 2];

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

    p4_state_event();

    // Reset metrics queue
    metric_dac_buf = NULL;
    metric_adc_buf = NULL;
    metric_busy = 0;
}


void p4_state_event(void)
{
    switch (run_type)
    {
        case CONTINUOUS:
            // Both should be running
            dac_state = DEVICE_RUNNING;
            adc_state = DEVICE_RUNNING;
            break;
        case SINGLE:
            // Should only iterate once
            dac_state = DEVICE_STOPPED;
            adc_state = DEVICE_STOPPED;
            break;
        case TEN:
            ten_counter++;
            if (ten_counter >= 10)
            {
                // Save the last iteration
                FW_ASSERT(metric_adc_buf);
                memcpy(metric_ten_save[1], metric_adc_buf, sizeof(metric_ten_save[1]));
                dac_state = DEVICE_STOPPED;
                adc_state = DEVICE_STOPPED;

                // Print the raw data to the uart
                p4_ten_raw_data();
            }
            else
            {
                // Save the first iteration
                if (ten_counter == 1)
                {
                    FW_ASSERT(metric_adc_buf);
                    memcpy(metric_ten_save[0], metric_adc_buf, sizeof(metric_ten_save[0]));
                }

                dac_state = DEVICE_RUNNING;
                adc_state = DEVICE_RUNNING;
            }
            break;
        default:
            FW_ASSERT(0 && "Invalid running state", run_type);
    }
}

void p4_ten_raw_data(void)
{
    for (U32 i = 0; i < 2; i++)
    {
        uprintf("Iteration #%d:\r\n", i);
        for (U32 j = 0; j < BUF_SIZE; j++)
        {
            uprintf("%d%s",
                    metric_ten_save[i][j],
                    (j + 1 < BUF_SIZE) ? "," : "\r\n");

            // Don't dump too much data at once
            if (j && j % (BUF_SIZE / 4) == 0)
            {
                uprintf("\r\nPress any key to print next [CHUNK]\r\n");
                while(!(USART2->ISR & USART_ISR_RXNE));
                (void)(USART2->RDR & 0xFF);
            }
        }

        uprintf("Press any key to print next [ITERATION]\r\n");
        while(!(USART2->ISR & USART_ISR_RXNE));
        (void)(USART2->RDR & 0xFF);
    }
}

void p4_switch_state(void)
{
    // Go to next state
    run_type = (run_type + 1) % STATE_N;

    const char* name;
    dac_state = DEVICE_RUNNING;
    adc_state = DEVICE_RUNNING;
    adc_dac_restart();

    switch (run_type)
    {
        case CONTINUOUS:
            name = "Continuous";
            break;
        case SINGLE:
            name = "Single";
            break;
        case TEN:
            name = "Ten";
            dac_adc_init(); // reset the initial wave to triangle
            ten_counter = 0;
            iter_n = 0;
            break;
        default:
            FW_ASSERT(0 && "Invalid state", run_type);
    }

    uprintf("== %s ==\r\n", name);
}
