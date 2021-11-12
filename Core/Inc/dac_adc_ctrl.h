//
// Created by tumbar on 11/3/21.
//

#ifndef CMPE663_DAC_ADC_CTRL_H
#define CMPE663_DAC_ADC_CTRL_H

#define DYNAMIC_RANGE (1 << 12)

// Drop three bits of accuracy (these are mostly just noise)
#define BUF_SIZE (DYNAMIC_RANGE * 2)

/**
 * Perform necessary initialization to
 * get the
 * @param hadc
 * @param hdac
 */
void dac_adc_init(void* hadc, void* hdac);

void adc_start(void);

/**
 * Rotate the buffers being used for ADC and DAC
 * (Dump the ADC readings into the DAC)
 * @param hdac
 * @param hadc
 */
void adc_dac_rotate_buffers(void);

/**
 * Interrupt handler for tim2
 */
void dac_tim2_int(void);

void adc_dac_next_state(void);
void adc_dac_set_continuous(void);
void adc_dac_set_single(void);
void adc_dac_toggle_state(void);

#endif //CMPE663_DAC_ADC_CTRL_H
