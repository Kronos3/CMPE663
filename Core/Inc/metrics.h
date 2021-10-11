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
    METRIC_TELLER_BREAK_1,      //!< Length of a teller 1's break period
    METRIC_TELLER_BREAK_2,      //!< Length of a teller 2's break period
    METRIC_TELLER_BREAK_3,      //!< Length of a teller 3's break period
    METRIC_N,
} metric_t;

void metric_add(teller_t teller_id, metric_t metric_id, U32 value);

#endif //CMPE663_METRICS_H
