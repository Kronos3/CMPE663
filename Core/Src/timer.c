//
// Created by tumbar on 9/2/21.
//

#include <timer.h>

// Each count is 0.1 ms
#define PRESCALER 8000

void p1_init_tim(TIM_TypeDef* self)
{
    self->CCR1;

    // Trigger
    self->CNT = 10000;
}
