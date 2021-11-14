//
// Created by tumbar on 11/3/21.
//

#ifndef CMPE663_DAC_ADC_CTRL_H
#define CMPE663_DAC_ADC_CTRL_H

#include <types.h>

typedef enum {
    DEVICE_RUNNING,     //!< DAC or ADC running
    DEVICE_STOPPED
} device_state_t;

typedef enum {
    CONTINUOUS,         //!< Keep feeding ADC into DAC
    SINGLE,             //!< Feed ADC into DAC a single time
    TEN,                //!< Feed ADC into DAC 10 times
    STATE_N,
} run_state_t;

extern volatile device_state_t dac_state;
extern volatile device_state_t adc_state;
extern volatile run_state_t run_type;

#define DYNAMIC_RANGE (1 << 12)

// Drop three bits of accuracy (these are mostly just noise)
#define BUF_SIZE (DYNAMIC_RANGE * 2)

// Use three buffers to give time to metrics to compute
// without the ADC overwriting the data
#define BUF_N 3
COMPILE_ASSERT(BUF_N >= 2, too_few_buffers);

/**
 * Initialize the first buffer to a new triangle wave
 */
void dac_adc_init(void);

/**
 * Rotate the buffers being used for ADC and DAC
 * (Dump the ADC readings into the DAC)
 * @param hdac
 * @param hadc
 */
void adc_dac_rotate_buffers(void);

/**
 * Interrupt handler for tim2
 *   1. Write the new sample to the DAC
 *   2. Trigger conversion on the ADC
 *     if cycle is complete:
 *          1. Queue up metrics calculation
 *          2. Rotate Buffers:
 *              DAC = ADC
 *              ADC = clean_buffer
 *          3. Restart cycle
 */
void dac_tim2_int(void);

/**
 * Restart the sample indices of the ADC and DAC
 * (restarts the current iterations)
 */
void adc_dac_restart(void);

#endif //CMPE663_DAC_ADC_CTRL_H
