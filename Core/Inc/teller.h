//
// Created by tumbar on 10/11/21.
//

#ifndef CMPE663_TELLER_H
#define CMPE663_TELLER_H

#include <types.h>

#define MAX_QUEUE_LENGTH 256

typedef enum {
    TELLER_1,
    TELLER_2,
    TELLER_3,
    TELLER_N,
} teller_t;

typedef enum {
    TELLER_WAITING = 0,     //!< Waiting for customer
    TELLER_SERVING,         //!< Serving a customer
    TELLER_ON_BREAK,        //!< Teller is on break
} teller_state_t;

typedef struct {
    teller_t id;
    volatile teller_state_t state;

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
extern void teller_task(void* argument);

/**
 * Print the status to the UART
 * @param argument
 */
void status_task(void* argument);
U32 bank_queue_length(void);

/**
 * Wait for all tellers to finish up for the day
 */
void teller_await_finish(void);

/**
 * Add a customer to the waiting queue
 * @param customer customer to add
 */
void bank_queue_customer(const Customer* customer);

#endif //CMPE663_TELLER_H
