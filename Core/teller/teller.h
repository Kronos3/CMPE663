//
// Created by tumbar on 10/11/21.
//

#ifndef CMPE663_TELLER_H
#define CMPE663_TELLER_H

typedef enum {
    TELLER_WAITING = 0,     //!< Waiting for customer
    TELLER_SERVING,         //!< Serving a customer
    TELLER_ON_BREAK,        //!< Teller is on break
} teller_t;

typedef struct {
    teller_t state;
} Teller;

#endif //CMPE663_TELLER_H
