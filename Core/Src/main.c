/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <p5.h>
#include <uart.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac_ch1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_DMA_Init(void);

static void MX_DAC1_Init(void);

static void MX_TIM2_Init(void);

static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_DAC1_Init();
    MX_TIM2_Init();
    MX_USART2_UART_Init();
    /* USER CODE BEGIN 2 */

    Trigger trigger;
    p5_compute_trigger(&trigger, 1000);
    TIM2->ARR = trigger.arr;

    // Where the actual samples are stored
    static Sample sample_buf[MAX_SAMPLES];

    p5_sin_wave(sample_buf, 1000, 4000, trigger.n);

    HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, sample_buf, trigger.n, DAC_ALIGN_12B_R);
    HAL_TIM_Base_Start(&htim2);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    uprintf("==== Wave types ====\r\n"
            "R,r: Rectangular (square wave)\r\n"
            "T,t: Triangular\r\n"
            "S,s: Sinusoid\r\n"
            "Frequency: 10 Hz - 100000 (10kHz)\r\n"
            "Minimum Voltage: 0-3.3V\r\n"
            "Maximum Voltage: 0-3.3V\r\n\r\n"
            "Input: type freq minv maxv\r\n");

    while (1)
    {
        static char input_buf[64];
        uprintf("> ");
        const char* input = ugetline(input_buf, sizeof(input_buf));

        char wave_type;
        I32 frequency;
        F32 min_v, max_v;
        sscanf(input, "%c %d %f %f",
               &wave_type, &frequency,
               &min_v, &max_v);

        if (frequency < 10 || frequency > 100000)
        {
            uprintf("Frequency %d is out of range [10Hz - 100 kHz\r\n");
            continue;
        }

        if (min_v >= max_v)
        {
            uprintf("Minimum voltage must be lower than maximum voltage\r\n");
            continue;
        }
        else if (min_v < 0.0 || max_v < 0.0 ||
                 min_v > 3.3 || max_v > 3.3)
        {
            uprintf("Voltage %fV-%fV out of range [0V-3.3V]\r\n", min_v, max_v);
            continue;
        }

        void (* wave_f)(Sample*, U32, U32, U32);
        switch (wave_type)
        {
            case 'R':
            case 'r':
                wave_f = p5_square_wave;
                break;
            case 'S':
            case 's':
                wave_f = p5_sin_wave;
                break;
            case 'T':
            case 't':
                wave_f = p5_triangle_wave;
                break;
            default:
                wave_f = NULL;
                uprintf("Invalid wave type: '%c': (R,S,T)\r\n", wave_type);
                break;
        }

        // Make sure the wave type is valid
        if (!wave_f) continue;

        // Compute the voltage in DAC ticks
        U32 min_v_dac = (U32) (min_v * (DYNAMIC_RANGE / 3.3));
        U32 max_v_dac = (U32) (max_v * (DYNAMIC_RANGE / 3.3));

        // Cap the min and max just in case
        if (min_v_dac < 0) min_v_dac = 0;
        if (max_v_dac >= DYNAMIC_RANGE) max_v_dac = DYNAMIC_RANGE - 1;

        // Compute the ARR and sample count based on the requested frequency
        p5_compute_trigger(&trigger, frequency);
        uprintf("type=%c, ARR=%d (%d Hz), N=%d, min_dac=%d, max_dac=%d\r\n",
                wave_type, trigger.arr, frequency,
                trigger.n, min_v_dac, max_v_dac);

        // Disable the timer and the DMA
        TIM2->CR1 &= ~(TIM_CR1_CEN);
        TIM2->ARR = trigger.arr;
        HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);

        // Set the update flag to signal an ARR update
        TIM2->EGR |= TIM_EGR_UG;

        // Compute the DMA samples
        wave_f(sample_buf, min_v_dac, max_v_dac, trigger.n);

        // Resend the DMA request (just in case the sample count changes)
        HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, sample_buf, trigger.n, DAC_ALIGN_12B_R);

        // Re-enable the timer which also starts triggering the DMA
        TIM2->CR1 |= TIM_CR1_CEN;

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

    /* USER CODE BEGIN DAC1_Init 0 */

    /* USER CODE END DAC1_Init 0 */

    DAC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN DAC1_Init 1 */

    /* USER CODE END DAC1_Init 1 */
    /** DAC Initialization
    */
    hdac1.Instance = DAC1;
    if (HAL_DAC_Init(&hdac1) != HAL_OK)
    {
        Error_Handler();
    }
    /** DAC channel OUT1 config
    */
    sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
    sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN DAC1_Init 2 */

    /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
