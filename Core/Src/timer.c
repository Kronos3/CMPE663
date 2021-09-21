//
// Created by tumbar on 9/2/21.
//

#include <timer.h>

POSTStatus p1_post(void)
{
    TIM_TypeDef* self = TIM2;
    self->CNT = 0;  // clear the timer counter so that overflow is not possible
    U32 curr_tim = self->CNT;

    // Wait for approx 100ms or the first pulse
    while (self->CNT <= curr_tim + 100000 && !(self->SR & TIM_SR_CC1IF));

    // Check if a pulse occurred in the last 100 ms
    // If it has, the event flag will be sent
    return (self->SR & TIM_SR_CC1IF) ? POST_SUCCESS : POST_FAILURE;
}

U32 p1_take_measurement(U32* last_cnt)
{
    TIM_TypeDef* self = TIM2;

    // Disable interrupts if its enabled
    if (self->SR & TIM_SR_UIF)
    {
        self->SR &= ~(TIM_SR_UIF);
    }

    // Wait until the timer event is tripped again
    while (!(self->SR & TIM_SR_CC1IF));

    // Reading CCR1 should clear the input flag
    U32 t2 = self->CCR1;

    // Compare against the last measurement
    U32 elapsed = t2 - *last_cnt;
    *last_cnt = t2;
    return elapsed;
}
