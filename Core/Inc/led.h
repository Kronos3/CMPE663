//
// Created by tumbar on 9/23/21.
//

#ifndef CMPE663_LED_H
#define CMPE663_LED_H

#include "types.h"

/**
 * Initialize the GPIO pins for LED usage
 */
void gpio_led_init(void);

// LED control interface
void set_led_1(U32 on);
void set_led_2(U32 on);
void set_led_3(U32 on);
void set_led_4(U32 on);

void seven_segment_set(U32 number);
void seven_segment_task(void* argument);

#endif //CMPE663_LED_H
