//
// Created by tumbar on 9/2/21.
//

#include <timer.h>
#include <uart.h>

#define NO_TIMEOUT (-1)

// 1 MHz counter frequency
// 1000 counts = 1 ms
#define TIM_1MHZ_TO_MS(cnt) ((cnt) / 1000)

/**
 * Waits for TIM2 to trigger and returns its time
 * @param self Timer to wait on
 * @param timeout in ms (-1 for none)
 * @return Milliseconds taken, negative for error
 */
static I32 wait_for_timer(TIM_TypeDef* self, I32 timeout_ms)
{
    if (timeout_ms >= 0)
    {
        // When a timeout is set, we will poll the timer counter
        // every nth loop. Once the counter exceeds the timeout in ms

        U32 start_cnt = self->CNT;

        // Wait for the timeout on the timer
        while (!(self->SR & TIM_SR_CC2OF_Msk) && TIM_1MHZ_TO_MS(self->CNT - start_cnt) < timeout_ms)
        {
            // Assume 80Mhz per instruction (not quite but close because integer operations)
            // ~2 instructions in this loop, 40 MHz per loop
            // Check the timer counter
            I32 delay = 10000;
            while (--delay);  // delay CPU by ~ .25 ms
        }

        if (!(self->SR & TIM_SR_CC2OF_Msk))
        {
            // Timer event not tripped
            // Timeout was reached
            return -1;
        }
    }
    else
    {
        // Wait until the timer event is tripped
        while (!(self->SR & TIM_SR_CC2OF_Msk));
    }

    return (I32)TIM_1MHZ_TO_MS(self->CNT);
}

I32 p1_post(void)
{
    if (wait_for_timer(TIM2, 100) < 0)
    {
        uprintf("No signal received before timeout\n");
        return 1;
    }

    // Successful POST
    return 0;
}

I32 p1_take_measurement(void)
{
    return wait_for_timer(TIM2, NO_TIMEOUT);
}
