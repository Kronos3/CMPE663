//
// Created by tumbar on 10/11/21.
//

#ifndef CMPE663_METRICS_PUB_H
#define CMPE663_METRICS_PUB_H

#include <ipc/ipc_pub.h>

typedef enum {
    METRIC_CUSTOMER_WAIT,       //!< Time customer waits for teller
    METRIC_TELLER_WAIT,         //!< Time teller waits for customer
    METRIC_TELLER_SERVICE,      //!< Time a customer is serviced by teller
    METRIC_N,
} metric_t;

void metric_add(task_t teller_id, metric_t metric_id, U32 value);

#endif //CMPE663_METRICS_PUB_H
