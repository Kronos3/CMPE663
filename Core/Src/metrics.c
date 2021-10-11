//
// Created by tumbar on 10/11/21.
//

#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>
#include "metrics.h"

typedef struct {
    U32 total;
    U32 n;
    U32 max;
    U32 min;
} Statistic;

static struct {
    U32 customers[3];
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
        case METRIC_CUSTOMER_WAIT:
            // Increment number of customers for this teller
            // when they get services
            metrics.customers[teller_id]++;
            // fallthrough
        case METRIC_TELLER_WAIT:
        case METRIC_TELLER_SERVICE:
        case METRIC_TELLER_BREAK_1:
        case METRIC_TELLER_BREAK_2:
        case METRIC_TELLER_BREAK_3:
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
            xSemaphoreGive(metric_id);
            FW_ASSERT(0 && "Invalid metric request", metric_id);
    }

    xSemaphoreGive(metric_id);
}
