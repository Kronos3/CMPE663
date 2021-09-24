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
// NOTE: For servo safety reasons
//        The PWM signal written to the motor should not
//        exceed 10% duty cycle. A software assertion will
//        catch these violations
const static U32 mot_servo_cycle_tenth_percent[] = {
        20,  // 2% 0
        35,  // 1
        50,  // 2
        65,  // 3
        80,  // 4
        100  // 10% 5
};

static inline
void mot_set_pwm(volatile U32* ccr, U8 duty_cycle_percent)
{
    FW_ASSERT(ccr);

    // Perform a safety check on the inputted duty cycle
    FW_ASSERT(duty_cycle_percent >= 20 && duty_cycle_percent <= 100);

    U32 ccr_temp = (TIM2_ARR / 1000) * duty_cycle_percent;
    FW_ASSERT_N(ccr_temp <= 2000, ccr_temp);

    *ccr = ccr_temp;
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
                pos < sizeof(mot_servo_cycle_tenth_percent) / sizeof(mot_servo_cycle_tenth_percent[0]),
                pos);

    // Set the PWM duty cycle on the correct PIN
    mot_set_pwm(
            motor_table[mid].ccr,
            mot_servo_cycle_tenth_percent[pos]
    );
    motor_table[mid].current_position = pos;
}
