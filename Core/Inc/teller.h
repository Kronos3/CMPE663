//
// Created by tumbar on 10/11/21.
//

#ifndef CMPE663_TELLER_H
#define CMPE663_TELLER_H

#include <types.h>

#define MAX_QUEUE_LENGTH 32

typedef enum {
    TELLER_1,
    TELLER_2,
    TELLER_3,
} teller_t;

typedef enum {
    TELLER_WAITING = 0,     //!< Waiting for customer
    TELLER_SERVING,         //!< Serving a customer
    TELLER_ON_BREAK,        //!< Teller is on break
} teller_state_t;

typedef struct {
    teller_t id;
    teller_state_t state;

    U32 start_wait;         //!< When we started to wait for the next customer
    U32 next_break;         //!< When our next break is
} Teller;

typedef struct {
    U32 queue_start;        //!< When the customer started queuing
    U32 transaction_time;   //!< Length of transaction
} Customer;

/**
 * Perform common initialization for tellers
 */
void teller_init(void);

/**
 * Main task for each teller
 * @param argument (void*)(teller_t)
 */
void teller_task(void* argument);

void bank_queue_customer(const Customer* customer);

#endif //CMPE663_TELLER_H
