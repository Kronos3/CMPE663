//
// Created by tumbar on 11/13/21.
//

#ifndef CMPE663_GRAD_PROJECT_H
#define CMPE663_GRAD_PROJECT_H

#include <types.h>

#define SPEED_OF_SOUND_MS 343

typedef enum {
    STATUS_FAILURE = -1,
    STATUS_SUCCESS = 0
} status_t;

/**
 * Wait for the next event on the timer input
 * capture pin (rising or falling)
 * @return
 */
U32 tim_get_event(void);

/**
 * Send a ping to the ultrasonic
 * This will trigger the ultrasonic pulse
 * on the falling edge of the input pulse
 */
void ultrasonic_send_ping(void);

/**
 * Measure the elapsed echo pulse time after
 * sending a ping (standalone call)
 * @return elapsed echo in microseconds
 */
U32 ultrasonic_measure(void);

/**
 * Convert ultrasonic measurement to distance
 * @param useconds elapsed time of echo in microsecond
 * @return distance in mm
 */
static inline U32 ultrasonic_convert(U32 useconds)
{
    // m/s * us -> us / 1000 -> mm
    return SPEED_OF_SOUND_MS * useconds / 1000;
}

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
