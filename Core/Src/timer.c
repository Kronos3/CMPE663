//
// Created by tumbar on 9/2/21.
//

#include <timer.h>

// 1 MHz counter frequency
// 1000 counts = 1 ms
#define TIM_1MHZ_TO_MS(cnt) ((cnt) / 1000)

POSTStatus p1_post(void)
{
    // Check if there was a signal in the last 100 ms
    // TODO(tumbar) This is definitely wrong
    if (TIM2->CCR2 && TIM_1MHZ_TO_MS(TIM2->CCR2) <= 100)
    {
        return POST_SUCCESS;
    }
    else
    {
        return POST_FAILURE;
    }
}

I32 p1_take_measurement(void)
{
    TIM_TypeDef* self = TIM2;

    // Wait until the timer event is tripped
    while (!(self->SR & TIM_SR_CC2IF));

    I32 out = (I32)TIM_1MHZ_TO_MS(self->CCR2);

    // Clear the interrupt flag
    self->SR &= ~(TIM_SR_CC2IF);

    return out;
}
