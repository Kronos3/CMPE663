//
// Created by tumbar on 9/2/21.
//

#ifndef CMPE663_TIMER_H
#define CMPE663_TIMER_H

#include <stm32l476xx.h>
#include <types.h>

typedef enum
{
    POST_SUCCESS = 0,
    POST_FAILURE = -1,
} POSTStatus;

/**
 * Perform the POST routine:
 *   Wait for a pulse within 100 milliseconds
 * @return 0 for success, non-zero for error
 */
POSTStatus p1_post(void);

/**
 * Take a pulse measurement
 * @return Duration of pulse period in milliseconds
 */
I32 p1_take_measurement(void);

#endif //CMPE663_TIMER_H
