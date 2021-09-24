//
// Created by tumbar on 9/22/21.
//

#include "mot.h"
#include "stm32l476xx.h"

#define TIM2_ARR (20000)

static struct {
    MotorId mid;
    MotPos current_position;
    volatile U32* ccr;  //!< PWM duty cycle control register

    // TODO(tumbar) Set up target/dispatch mot commanding
} motor_table[MOT_N] = {
        {MOT_SERVO_1, 0, &TIM2->CCR1},
        {MOT_SERVO_2, 0, &TIM2->CCR2}
};

// Position to PWM percentage
// These may be changed as needed
// NOTE 1: For servo safety reasons
//         The PWM signal written to the motor should not
//         exceed 10% duty cycle. A software assertion will
//         catch these violations
// NOTE 2: TIM2 runs at with a 79 pre-scalar connected to an 80Mhz clock
//         ARR set to 20000 (20 ms period)
//         Each count is 1us
const static U32 mot_servo_duty_counts[] = {
    1000,   // 0 (all the way to the right)
    1200,   // 1
    1400,   // 2
    1600,   // 3
    1800,   // 4
    2000,   // 5 (all the way to the left)
};

static inline
void mot_set_pwm(volatile U32* ccr, U32 duty_cycle_count)
{
    FW_ASSERT(ccr);

    // Perform a safety check on the inputted duty cycle
    // Don't go above 10 % duty cycle
    // Don't go below 5 % duty cycle
    FW_ASSERT_N(duty_cycle_count <= 2000 && duty_cycle_count >= 1000, duty_cycle_count);

    *ccr = duty_cycle_count;
}

MotPos mot_get_position(MotorId mid)
{
    FW_ASSERT_N(mid > MOT_INVALID && mid < MOT_N, mid);
    return motor_table[mid].current_position;
}

void mot_set_position(MotorId mid, MotPos pos)
{
    FW_ASSERT(mid > MOT_INVALID && mid < MOT_N);

    // Validate the position range
    FW_ASSERT_N(pos >= 0 && pos <= 5 &&
                pos < sizeof(mot_servo_duty_counts) / sizeof(mot_servo_duty_counts[0]),
                pos);

    // Set the PWM duty cycle on the correct PIN
    mot_set_pwm(
            motor_table[mid].ccr,
            mot_servo_duty_counts[pos]
    );
    motor_table[mid].current_position = pos;
}
