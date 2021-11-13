//
// Created by tumbar on 11/13/21.
//

#include <ultrasonic.h>
#include <stm32l4xx.h>

// GPIO Trigger pin on PA1
#define PING_PIN (GPIO_PIN_1)
#define PING_PORT (GPIOA)

// 20 us, (10 us pulse minimum)
#define PULSE_CNT (20)
COMPILE_ASSERT(PULSE_CNT >= 10, pulse_too_short);

static U32 last_ping = 0;
U32 ultrasonic_get_ping(void)
{
    // Wait until the timer event is tripped again
    while (!(TIM2->SR & TIM_SR_CC1IF));

    // Reading CCR1 should clear the input flag
    U32 t2 = TIM2->CCR1;

    // Compare against the last measurement
    U32 elapsed = t2 - last_ping;
    last_ping = t2;
    return elapsed;
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
    (void) ultrasonic_get_ping();

    // Measure the rising -> falling edge time on the echo pin
    return ultrasonic_get_ping();
}
