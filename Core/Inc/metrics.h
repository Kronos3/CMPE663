//
// Created by tumbar on 11/11/21.
//

#ifndef CMPE663_METRICS_H
#define CMPE663_METRICS_H

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
void metrics_compute(const U16 dac_buf[BUF_SIZE], const U16 adc_buf[BUF_SIZE]);

/**
 * Waits for signals from the timer interrupt via
 * metrics compute
 * (Non blocking - for super-loop)
 */
void metrics_task(void);

#endif //CMPE663_METRICS_H
