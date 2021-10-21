//
// Created by tumbar on 9/2/21.
//

#include <timer.h>

POSTStatus p1_post(void)
{
    TIM2->CNT = 0;  // clear the timer counter so that overflow is not possible
    U32 curr_tim = TIM2->CNT;

    // Wait for approx 100ms or the first pulse
    while (TIM2->CNT <= curr_tim + 100000 && !(TIM2->SR & TIM_SR_CC1IF));

    // Check if a pulse occurred in the last 100 ms
    // If it has, the event flag will be sent
    return (TIM2->SR & TIM_SR_CC1IF) ? POST_SUCCESS : POST_FAILURE;
}

U32 p1_take_measurement(U32* last_cnt)
{
    // Wait until the timer event is tripped again
    while (!(TIM2->SR & TIM_SR_CC1IF));

    // Reading CCR1 should clear the input flag
    U32 t2 = TIM2->CCR1;

    // Compare against the last measurement
    U32 elapsed = t2 - *last_cnt;
    *last_cnt = t2;
    return elapsed;
}
