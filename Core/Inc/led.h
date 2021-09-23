//
// Created by tumbar on 9/23/21.
//

#ifndef CMPE663_LED_H
#define CMPE663_LED_H

#include "types.h"
#include "seq.h"

/**
 * Set the led to show the states of the
 * running sequence engines
 * @param engines Pointer to both sequence engines
 */
void led_task(const Sequence* engines[2]);

/**
 * Initialize the GPIO pins for LED usage
 */
void gpio_led_init(void);

// LED control interface
void set_led_1(U32 on);
void set_led_2(U32 on);
void set_led_3(U32 on);
void set_led_4(U32 on);

#endif //CMPE663_LED_H
