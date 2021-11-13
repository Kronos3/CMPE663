//
// Created by tumbar on 11/13/21.
//

#ifndef CMPE663_GRAD_PROJECT_H
#define CMPE663_GRAD_PROJECT_H

typedef enum {
    STATUS_FAILURE = -1,
    STATUS_SUCCESS = 0
} status_t;

/**
 * Perform post routine
 * @return SUCCESS or FAILURE
 */
status_t grad_post(void);

/**
 * Display a realtime view of the ultrasonic
 * to calibrate to desired distance
 * Press any key to exit
 */
void grad_pre_measurement(void);

#endif //CMPE663_GRAD_PROJECT_H
