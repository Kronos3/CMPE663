//
// Created by tumbar on 11/13/21.
//

#include <grad_project.h>
#include <uart.h>
#include <ultrasonic.h>

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
    uprintf("Press any key to start calibration\r\n");
    while (!(USART2->ISR & USART_ISR_RXNE));
    (void) (USART2->RDR & 0xFF);

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
