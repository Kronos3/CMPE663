//
// Created by tumbar on 11/13/21.
//

#ifndef CMPE663_ULTRASONIC_H
#define CMPE663_ULTRASONIC_H

#include <types.h>

#define SPEED_OF_SOUND_MS 343

/**
 * Wait for the next ping event on the
 * echo pin (rising or falling)
 * @return
 */
U32 ultrasonic_get_ping(void);

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

#endif //CMPE663_ULTRASONIC_H
