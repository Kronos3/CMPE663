//
// Created by tumbar on 11/11/21.
//

#ifndef CMPE663_P4_H
#define CMPE663_P4_H

#include <types.h>
#include <dac_adc_ctrl.h>

/**
 * Called from inside the timer interrupt
 * when a single iteration has finished.
 * This will queue up the metrics_task to begin computing
 * the metrics and print to the uart
 * @param dac_buf
 * @param adc_buf
 */
void metrics_queue(const U16 dac_buf[BUF_SIZE], const U16 adc_buf[BUF_SIZE]);

/**
 * Waits for signals from the timer interrupt via
 * metrics compute
 * (Non blocking - for super-loop)
 */
void metrics_task(void);

/**
 * Notify the state machine that a cycle has
 * completed and the metrics have finished computing
 */
void p4_state_event(void);

/**
 * Go to the next available run type
 * CONTINUOUS -> SINGLE -> TEN
 *     ^                    |
 *     +--------------------+
 */
void p4_switch_state(void);

/**
 * Print the raw data from iteration #1 and #10
 * after running completing TEN run mode
 */
void p4_ten_raw_data(void);

#endif //CMPE663_P4_H
