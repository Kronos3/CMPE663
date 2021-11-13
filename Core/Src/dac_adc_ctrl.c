//
// Created by tumbar on 11/3/21.
//

#include <dac_adc_ctrl.h>
#include <types.h>

#include <stm32l4xx.h>
#include <uart.h>
#include <p4.h>

// Use three buffers to give time to metrics to compute
// without the ADC overwriting the data
#define BUF_N 3
COMPILE_ASSERT(BUF_N >= 2, too_few_buffers);

// Sample buffers
static U16 dma_buf[BUF_N][BUF_SIZE];

// HAL ADC for use in tim2_interrupt
extern ADC_HandleTypeDef hadc1;

// Index of the buffer in use by ADC and DAC
static I32 adc_ptr_i = 0, dac_ptr_i = 0;

// Index of the current sample in the current buffer
static volatile I32 dac_i = 0;
static volatile I32 adc_i = 0;

// Device and program states
volatile device_state_t dac_state = DEVICE_RUNNING;
volatile device_state_t adc_state = DEVICE_RUNNING;
volatile run_state_t run_type = CONTINUOUS;

void dac_adc_init(void)
{
    dac_ptr_i = 0;
    adc_ptr_i = 1;

    // Initialize the starting triangular wave
    U32 k = 0;
    for (U32 i = 0; i < BUF_SIZE; i++)
    {
        if (i < DYNAMIC_RANGE)
        {
            dma_buf[dac_ptr_i][i] = k++;
        }
        else
        {
            dma_buf[dac_ptr_i][i] = --k;
        }
    }
}

void adc_dac_rotate_buffers(void)
{
    // We are using four buffers so that the ADC will
    // not write to a buffer that is calculating metrics
    dac_ptr_i = (dac_ptr_i + 1) % BUF_N;
    FW_ASSERT(dac_ptr_i == adc_ptr_i, dac_ptr_i, adc_ptr_i);
    adc_ptr_i = (adc_ptr_i + 1) % BUF_N;
}

void dac_cycle_complete(void)
{
    // When DAC has finished output the signal over the GPIO
    metrics_compute(dma_buf[dac_ptr_i], dma_buf[adc_ptr_i]);
    adc_dac_rotate_buffers();
}

void adc_dac_restart(void)
{
    dac_i = 0;
    adc_i = 0;
}

void dac_tim2_int(void)
{
    switch(dac_state)
    {
        case DEVICE_RUNNING:
        {
            FW_ASSERT(dac_i < BUF_SIZE, dac_i);

            // Write value to the DAC
            DAC1->DHR12R1 = dma_buf[dac_ptr_i][dac_i++];
            HAL_ADC_Start_IT(&hadc1);

            // Reset the counter
            if (dac_i >= BUF_SIZE)
            {
                adc_dac_restart();
                dac_cycle_complete();
            }
        }
            break;
        case DEVICE_STOPPED:
        {
            // NULL out the DAC
            DAC1->DHR12R1 = 0;
        }
            break;
        default:
            FW_ASSERT(0 && "Invalid DAC state", dac_state);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    switch(adc_state)
    {
        case DEVICE_RUNNING:
        {
            if (adc_i < BUF_SIZE)
            {
                // Read and Update The ADC Result
                dma_buf[adc_ptr_i][adc_i++] = HAL_ADC_GetValue(hadc);
            }
        }
            break;
        case DEVICE_STOPPED:
            break;
        default:
            FW_ASSERT(0 && "Invalid ADC state", adc_state);
    }
}
