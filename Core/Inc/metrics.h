//
// Created by tumbar on 10/11/21.
//

#ifndef CMPE663_METRICS_H
#define CMPE663_METRICS_H


#include <types.h>
#include <teller.h>

typedef enum {
    METRIC_CUSTOMER_WAIT,       //!< Time customer waits for teller
    METRIC_TELLER_WAIT,         //!< Time teller waits for customer
    METRIC_TELLER_SERVICE,      //!< Time a customer is serviced by teller
    METRIC_TELLER_1_BREAK,      //!< Length of a teller 1's break period
    METRIC_TELLER_2_BREAK,      //!< Length of a teller 2's break period
    METRIC_TELLER_3_BREAK,      //!< Length of a teller 3's break period
    METRIC_N,                   //!< Statistic metric count
    METRIC_QUEUE_LENGTH,        //!< Current length of hte customer queue
} metric_t;

/**
 * Handle a new metric and add it to the metric system
 * @param teller_id Teller where event happened
 * @param metric_id Event that happened
 * @param value Metric's value
 */
void metric_add(teller_t teller_id, metric_t metric_id, U32 value);

void metric_init(void);

/**
 * Display all available metrics
 */
void metrics_display(void);

#endif //CMPE663_METRICS_H
