//
// Created by tumbar on 11/13/21.
//

#include <grad_project.h>
#include <uart.h>
#include <stm32l4xx_hal.h>

// GPIO Trigger pin on PA1
#define PING_PIN (GPIO_PIN_1)
#define PING_PORT (GPIOA)

// 20 us, (10 us pulse minimum)
#define PULSE_CNT (20)
COMPILE_ASSERT(PULSE_CNT >= 10, pulse_too_short);

status_t grad_post(void)
{
    static char scratch_buf[32];
    uprintf("Place an object ~100mm away from the ultrasonic sensor\r\n"
            "Press [ENTER] to continue: ");
    (void) ugetline(scratch_buf, sizeof(scratch_buf));

    U32 post_measurement = ultrasonic_measure();
    if (post_measurement < 300)
    {
        uprintf("Object is too close\r\n");
        return STATUS_FAILURE;
    }
    else if (post_measurement > 1000)
    {
        uprintf("Object is too far\r\n");
        return STATUS_FAILURE;
    }

    uprintf("POST Successful\r\n");
    return STATUS_SUCCESS;
}

void grad_pre_measurement(void)
{
    // Display a prompt with a live feed of the current measurement
    uprintf("Press any key when distance is satisfactory\r\n");

    // Wait for any user input
    while (!(USART2->ISR & USART_ISR_RXNE))
    {
        U32 ref_tim = TIM2->CNT;
        uprintf("\r%d mm   ", ultrasonic_convert(ultrasonic_measure()));

        // Display at 10Hz
        while (TIM2->CNT < ref_tim + 100000);
    }

    // USART resets the RXNE flag automatically after reading DR
    (void) (USART2->RDR & 0xFF);
}

U32 tim_get_event(void)
{
    // Wait until the timer event is tripped again
    while (!(TIM2->SR & TIM_SR_CC1IF));

    // Reading CCR1 should clear the input flag
    return TIM2->CCR1;
}

void ultrasonic_send_ping(void)
{
    // Send the ping pulse
    HAL_GPIO_WritePin(PING_PORT, PING_PIN, GPIO_PIN_SET);

    // Get the reference timer count in microseconds
    U32 curr = TIM2->CNT;

    // Wait for ping tim
    while (TIM2->CNT < curr + PULSE_CNT);

    // Stop the pulse
    HAL_GPIO_WritePin(PING_PORT, PING_PIN, GPIO_PIN_RESET);
}

U32 ultrasonic_measure(void)
{
    // Pulse the input pin
    ultrasonic_send_ping();

    // Wait for the rising edge on the echo pin
    // Gets a reference time to base response time off of
    U32 reference_cnt = tim_get_event();

    // Measure the rising -> falling edge time on the echo pin
    return tim_get_event() - reference_cnt;
}

