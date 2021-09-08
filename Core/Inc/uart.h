#ifndef __STM32L476G_DISCOVERY_UART_H
#define __STM32L476G_DISCOVERY_UART_H

#include "stm32l476xx.h"
#include <types.h>

// Read and write to the same UART device
#define u_stdout USART2
#define u_stdin USART2

#define BufferSize 50

// HAL definitions
#define GPIO_SPEED_FREQ_VERY_HIGH    (0x00000003u)    /*!< range 50 MHz to 80 MHz, please refer to the product datasheet */
#define GPIO_AF7_USART2                ((uint8_t)0x07) /*!< USART2 Alternate Function mapping     */
#define GPIO_MODE_AF_PP             (0x00000002u)   /*!< Alternate Function Push Pull Mode     */
#define UART_WORDLENGTH_8B          0x00000000U     /*!< 8-bit long UART frame */
#define UART_PARITY_NONE            0x00000000U     /*!< No parity   */
#define UART_OVERSAMPLING_16        0x00000000U     /*!< Oversampling by 16 */
#define UART_STOPBITS_1             0x00000000U     /*!< UART frame with 1 stop bit    */
#define UART_HWCONTROL_NONE         0x00000000U     /*!< No hardware control       */


void USART1_IRQHandler(void);

void USART2_IRQHandler(void);

/**
 * Printf to uart
 * @param format_str format string
 * @param ... printf va_args
 * @return length of write
 */
I32 uprintf(const char* format_str, ...);

/**
 * Read a line from the uart
 * (Block until newline is read)
 * Always null terminate string
 * @param buf buffer to read into
 * @param len max length to reach
 * @return Pointer to buf
 */
char* ugetline(char buf[], U32 len);

void USART_Write(USART_TypeDef* USARTx, const U8* buffer, U32 nBytes);

U8 USART_Read(USART_TypeDef* USARTx);

void USART_Delay(U32 us);

void USART_IRQHandler(USART_TypeDef* USARTx, U8* buffer, U32* pRx_counter);

void USART2_Init(I32 baudrate);

#endif /* __STM32L476G_DISCOVERY_UART_H */
