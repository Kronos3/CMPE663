//
// Created by tumbar on 11/3/21.
//

#include <dac_adc_ctrl.h>
#include <types.h>

#include <stm32l4xx.h>
#include <stm32l4xx_hal_dac.h>
#include <uart.h>
#include <metrics.h>

#define BUF_N 4

static U16 dma_buf[BUF_N][BUF_SIZE];

static ADC_HandleTypeDef* g_hadc = NULL;
static DAC_HandleTypeDef* g_hdac = NULL;

static U16* dac_ptr = NULL;
static U16* adc_ptr = NULL;

static I32 adc_ptr_i = 0, dac_ptr_i = 0;

static volatile I32 dac_i = 0;
static volatile I32 adc_i = 0;

enum {
    DAC_RUNNING,
    DAC_STOPPED,
} dac_state = DAC_RUNNING;

enum {
    ADC_RECORDING,
    ADC_STOPPED,
} adc_state = ADC_RECORDING;

enum {
    CONTINUOUS,
    SINGLE
} run_type = CONTINUOUS;

void dac_adc_init(void* hadc, void* hdac)
{
    g_hdac = hdac;
    g_hadc = hadc;

    dac_ptr_i = 0;
    adc_ptr_i = 1;
    dac_ptr = dma_buf[dac_ptr_i];
    adc_ptr = dma_buf[adc_ptr_i];

    // Initialize the starting triangular wave
    U32 k = 0;
    for (U32 i = 0; i < BUF_SIZE; i++)
    {
        if (i < DYNAMIC_RANGE)
        {
            dac_ptr[i] = k++;
        }
        else
        {
            dac_ptr[i] = --k;
        }
    }
}

void adc_dac_next_state(void)
{
    switch (run_type)
    {
        case CONTINUOUS:
            // Both should be running
            dac_state = DAC_RUNNING;
            adc_state = ADC_RECORDING;
            break;
        case SINGLE:
            // Should only iterate once
            dac_state = DAC_STOPPED;
            adc_state = ADC_STOPPED;
            break;
        default:
            FW_ASSERT(0 && "Invalid running state", run_type);
    }
}

void adc_dac_set_continuous(void)
{
    uprintf("== Continuous ==\r\n");
    run_type = CONTINUOUS;
    dac_state = DAC_RUNNING;
    adc_state = ADC_RECORDING;
    adc_i = 0;
    dac_i = 0;
}

void adc_dac_set_single(void)
{
    uprintf("== Single ==\r\n");
    run_type = SINGLE;
    dac_state = DAC_RUNNING;
    adc_state = ADC_RECORDING;
    adc_i = 0;
    dac_i = 0;
}

void adc_dac_toggle_state(void)
{
    switch(run_type)
    {
        case CONTINUOUS:
            adc_dac_set_single();
            break;
        case SINGLE:
            adc_dac_set_continuous();
            break;
        default:
            FW_ASSERT(0 && "Invalid run state", run_type);
    }
}

void adc_dac_rotate_buffers(void)
{
    // We are using four buffers so that the ADC will
    // not write to a buffer that is calculating metrics
    dac_ptr = dma_buf[(++dac_ptr_i) % BUF_N];
    FW_ASSERT(dac_ptr == adc_ptr, dac_ptr, adc_ptr);
    adc_ptr = dma_buf[(++adc_ptr_i) % BUF_N];
}

void dac_cycle_complete(void)
{
    // When DAC has finished output the signal over the GPIO
    metrics_compute(dac_ptr, adc_ptr);
    adc_dac_rotate_buffers();
}
void dac_tim2_int(void)
{
    switch(dac_state)
    {
        case DAC_RUNNING:
        {
            FW_ASSERT(dac_i < BUF_SIZE, dac_i);

            // Write value to the DAC
            DAC1->DHR12R1 = dac_ptr[dac_i++];
            HAL_ADC_Start_IT(g_hadc);

            // Reset the counter
            if (dac_i >= BUF_SIZE)
            {
                dac_i = 0;
                adc_i = 0;
                dac_cycle_complete();
            }
        }
            break;
        case DAC_STOPPED:
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
        case ADC_RECORDING:
        {
            if (adc_i < BUF_SIZE)
            {
                // Read and Update The ADC Result
                adc_ptr[adc_i++] = HAL_ADC_GetValue(hadc);
            }
        }
            break;
        case ADC_STOPPED:
            break;
        default:
            FW_ASSERT(0 && "Invalid ADC state", adc_state);
    }
}
