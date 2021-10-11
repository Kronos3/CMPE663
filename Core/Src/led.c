//
// Created by tumbar on 9/23/21.
//

#include <stm32l476xx.h>
#include "stm32l4xx_hal.h"
#include "led.h"

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define SHLD_A5_Pin GPIO_PIN_0
#define SHLD_A5_GPIO_Port GPIOC
#define SHLD_A4_Pin GPIO_PIN_1
#define SHLD_A4_GPIO_Port GPIOC
#define SHLD_A0_Pin GPIO_PIN_0
#define SHLD_A0_GPIO_Port GPIOA
#define SHLD_A1_Pin GPIO_PIN_1
#define SHLD_A1_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define SHLD_A2_Pin GPIO_PIN_4
#define SHLD_A2_GPIO_Port GPIOA
#define SHLD_D13_Pin GPIO_PIN_5
#define SHLD_D13_GPIO_Port GPIOA
#define SHLD_D12_Pin GPIO_PIN_6
#define SHLD_D12_GPIO_Port GPIOA
#define SHLD_D11_Pin GPIO_PIN_7
#define SHLD_D11_GPIO_Port GPIOA
#define SHLD_A3_Pin GPIO_PIN_0
#define SHLD_A3_GPIO_Port GPIOB
#define SHLD_D6_Pin GPIO_PIN_10
#define SHLD_D6_GPIO_Port GPIOB
#define SHLD_D9_Pin GPIO_PIN_7
#define SHLD_D9_GPIO_Port GPIOC
#define SHLD_D7_SEG7_Clock_Pin GPIO_PIN_8
#define SHLD_D7_SEG7_Clock_GPIO_Port GPIOA
#define SHLD_D8_SEG7_Data_Pin GPIO_PIN_9
#define SHLD_D8_SEG7_Data_GPIO_Port GPIOA
#define SHLD_D2_Pin GPIO_PIN_10
#define SHLD_D2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SHLD_D3_Pin GPIO_PIN_3
#define SHLD_D3_GPIO_Port GPIOB
#define SHLD_D5_Pin GPIO_PIN_4
#define SHLD_D5_GPIO_Port GPIOB
#define SHLD_D4_SEG7_Latch_Pin GPIO_PIN_5
#define SHLD_D4_SEG7_Latch_GPIO_Port GPIOB
#define SHLD_D10_Pin GPIO_PIN_6
#define SHLD_D10_GPIO_Port GPIOB
#define SHLD_D15_Pin GPIO_PIN_8
#define SHLD_D15_GPIO_Port GPIOB
#define SHLD_D14_Pin GPIO_PIN_9
#define SHLD_D14_GPIO_Port GPIOB


void set_led_1(U32 on)
{
    if (on) // a5
        HAL_GPIO_WritePin(SHLD_D13_GPIO_Port, SHLD_D13_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(SHLD_D13_GPIO_Port, SHLD_D13_Pin, GPIO_PIN_SET);
}

void set_led_2(U32 on)
{
    if (on) //a6
        HAL_GPIO_WritePin(SHLD_D12_GPIO_Port, SHLD_D12_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(SHLD_D12_GPIO_Port, SHLD_D12_Pin, GPIO_PIN_SET);
}

void set_led_3(U32 on)
{
    if (on) //a7
        HAL_GPIO_WritePin(SHLD_D11_GPIO_Port, SHLD_D11_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(SHLD_D11_GPIO_Port, SHLD_D11_Pin, GPIO_PIN_SET);
}

void set_led_4(U32 on)
{
    if (on)
        HAL_GPIO_WritePin(SHLD_D10_GPIO_Port, SHLD_D10_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(SHLD_D10_GPIO_Port, SHLD_D10_Pin, GPIO_PIN_SET);
}

void gpio_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*Configure GPIO pins : SHLD_A5_Pin SHLD_A4_Pin */
    GPIO_InitStruct.Pin = SHLD_A5_Pin|SHLD_A4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
    GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : SHLD_D13_Pin SHLD_D12_Pin SHLD_D11_Pin SHLD_D7_SEG7_Clock_Pin */
    GPIO_InitStruct.Pin = SHLD_D13_Pin|SHLD_D12_Pin|SHLD_D11_Pin|SHLD_D7_SEG7_Clock_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : SHLD_A3_Pin */
    GPIO_InitStruct.Pin = SHLD_A3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SHLD_A3_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : SHLD_D6_Pin SHLD_D5_Pin */
    GPIO_InitStruct.Pin = SHLD_D6_Pin|SHLD_D5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : SHLD_D9_Pin */
    GPIO_InitStruct.Pin = SHLD_D9_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(SHLD_D9_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : SHLD_D8_SEG7_Data_Pin */
    GPIO_InitStruct.Pin = SHLD_D8_SEG7_Data_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(SHLD_D8_SEG7_Data_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : SHLD_D3_Pin */
    GPIO_InitStruct.Pin = SHLD_D3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SHLD_D3_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : SHLD_D4_SEG7_Latch_Pin */
    GPIO_InitStruct.Pin = SHLD_D4_SEG7_Latch_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SHLD_D4_SEG7_Latch_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : SHLD_D10_Pin */
    GPIO_InitStruct.Pin = SHLD_D10_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SHLD_D10_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : SHLD_D15_Pin SHLD_D14_Pin */
    GPIO_InitStruct.Pin = SHLD_D15_Pin|SHLD_D14_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
