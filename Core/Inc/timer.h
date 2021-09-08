//
// Created by tumbar on 9/2/21.
//

#ifndef CMPE663_TIMER_H
#define CMPE663_TIMER_H

#include <stm32l476xx.h>
#include <types.h>

/**
 * Initialize a timer. Counter will count in ms
 * @param self timer to initialize
 */
void p1_init_tim(TIM_TypeDef* self);

#endif //CMPE663_TIMER_H
