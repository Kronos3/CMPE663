//
// Created by tumbar on 10/11/21.
//

#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>
#include <uart.h>
#include "metrics.h"
#include "tim.h"

typedef struct
{
    U32 total;
    U32 n;
    U32 max;
    U32 min;
} Statistic;

static struct
{
    U32 customers[TELLER_N];
    Statistic statistics[METRIC_N];
    U32 max_depth;                      //!< Maximum depth of the customer queue
} metrics = {0};

static SemaphoreHandle_t metrics_lock = NULL;

void metric_init(void)
{
    metrics_lock = xSemaphoreCreateMutex();
    FW_ASSERT(metrics_lock && "Failed to create mutex");

    memset(&metrics, 0, sizeof(metrics));
}

void metric_add(teller_t teller_id, metric_t metric_id, U32 value)
{
    xSemaphoreTake(metrics_lock, portMAX_DELAY);

    switch (metric_id)
    {
        case METRIC_QUEUE_LENGTH:
            if (value > metrics.max_depth)
            {
                metrics.max_depth = value;
            }
            break;
        case METRIC_TELLER_SERVICE:
            // Increment number of customers for this teller
            // when they get services
            metrics.customers[teller_id]++;
            // fallthrough
        case METRIC_TELLER_WAIT:
        case METRIC_CUSTOMER_WAIT:
        case METRIC_TELLER_1_BREAK:
        case METRIC_TELLER_2_BREAK:
        case METRIC_TELLER_3_BREAK:
            // Add to the metrics for this type
            metrics.statistics[metric_id].total += value;
            if (!metrics.statistics[metric_id].n)
            {
                metrics.statistics[metric_id].max = value;
                metrics.statistics[metric_id].min = value;
            }
            else if (value > metrics.statistics[metric_id].max)
            {
                metrics.statistics[metric_id].max = value;
            }
            else if (value < metrics.statistics[metric_id].min)
            {
                metrics.statistics[metric_id].min = value;
            }
            metrics.statistics[metric_id].n++;
            break;
        case METRIC_N:
        default:
            xSemaphoreGive(metrics_lock);
            FW_ASSERT(0 && "Invalid metric request", metric_id);
    }

    if (xSemaphoreGive(metrics_lock) == pdFALSE)
    {
        FW_ASSERT(0 && "Failed to release metrics");
    }
}

#define TIME_FORMAT "%u minutes %u seconds"
#define TIME_ARGS(ticks) tim_tick_to_min(ticks), tim_tick_to_second(ticks)

static inline U32 statistic_average(const Statistic* self)
{
    return self->total / self->n;
}

void metrics_display(void)
{
    uprintf(
            "1. The total number of customers: %u\r\n"
            "2. Number of customers served (each teller): %u, %u, %u\r\n"
            "3. The average time each customer spends waiting in the queue: " TIME_FORMAT "\r\n"
            "4. The average time each customer spends with the teller: " TIME_FORMAT "\r\n"
            "5. The average time tellers wait for customers: " TIME_FORMAT "\r\n"
            "6. The maximum customer wait time in the queue: " TIME_FORMAT "\r\n"
            "7. The maximum wait time for tellers waiting for customers: " TIME_FORMAT "\r\n"
            "8. The maximum transaction time for the tellers: " TIME_FORMAT "\r\n"
            "9. Maximum queue depth: %u\r\n",
            metrics.customers[0] + metrics.customers[1] + metrics.customers[2],
            metrics.customers[0], metrics.customers[1], metrics.customers[2],
            TIME_ARGS(statistic_average(&metrics.statistics[METRIC_CUSTOMER_WAIT])),
            TIME_ARGS(statistic_average(&metrics.statistics[METRIC_TELLER_SERVICE])),
            TIME_ARGS(statistic_average(&metrics.statistics[METRIC_TELLER_WAIT])),
            TIME_ARGS(metrics.statistics[METRIC_CUSTOMER_WAIT].max),
            TIME_ARGS(metrics.statistics[METRIC_TELLER_WAIT].max),
            TIME_ARGS(metrics.statistics[METRIC_TELLER_SERVICE].max),
            metrics.max_depth
    );

    uprintf("\r\n=== Grad student additional metrics ===\r\n"
            "Number of breaks (each teller): %u, %u, %u\r\n"
            "Average break time (each teller): "  TIME_FORMAT ", " TIME_FORMAT ", " TIME_FORMAT "\r\n"
            "Longest break time (each teller): "  TIME_FORMAT ", " TIME_FORMAT ", " TIME_FORMAT "\r\n"
            "Shortest break time (each teller): " TIME_FORMAT ", " TIME_FORMAT ", " TIME_FORMAT "\r\n",

            metrics.statistics[METRIC_TELLER_1_BREAK].n,
            metrics.statistics[METRIC_TELLER_2_BREAK].n,
            metrics.statistics[METRIC_TELLER_3_BREAK].n,

            TIME_ARGS(statistic_average(&metrics.statistics[METRIC_TELLER_1_BREAK])),
            TIME_ARGS(statistic_average(&metrics.statistics[METRIC_TELLER_2_BREAK])),
            TIME_ARGS(statistic_average(&metrics.statistics[METRIC_TELLER_3_BREAK])),

            TIME_ARGS(metrics.statistics[METRIC_TELLER_1_BREAK].max),
            TIME_ARGS(metrics.statistics[METRIC_TELLER_2_BREAK].max),
            TIME_ARGS(metrics.statistics[METRIC_TELLER_3_BREAK].max),

            TIME_ARGS(metrics.statistics[METRIC_TELLER_1_BREAK].min),
            TIME_ARGS(metrics.statistics[METRIC_TELLER_2_BREAK].min),
            TIME_ARGS(metrics.statistics[METRIC_TELLER_3_BREAK].min)
    );
}
