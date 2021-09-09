#include <uart.h>
#include <stdio.h>
#include <stdarg.h>
#include <types.h>


// UART Ports:
// ===================================================
// PD.5 = USART2_TX (AF7)  |  PD.6 = USART2_RX (AF7)

void USART2_Init(I32 baudrate)
{
    ////////////  CONFIGURE CLOCKS	/////////////////
    /* Init the low level hardware : GPIO, CLOCK */
    U32 tmpreg;
    (void) tmpreg;    /// avoid compiler warning

    // enable USART2 CLK
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    tmpreg = RCC->APB1ENR1 & RCC_APB1ENR1_USART2EN;    // Delay after an RCC peripheral clock enabling

    // enable GPIOA CLK
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    tmpreg = RCC->AHB2ENR & RCC_AHB2ENR_GPIOAEN;        // Delay after an RCC peripheral clock enabling

    //////////// CONFIGURE GPIO /////////////////
    /* Configure the IO Speed */
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2_Msk | GPIO_OSPEEDR_OSPEED3_Msk);
    GPIOA->OSPEEDR |= GPIO_SPEED_FREQ_VERY_HIGH << GPIO_OSPEEDR_OSPEED2_Pos;
    GPIOA->OSPEEDR |= GPIO_SPEED_FREQ_VERY_HIGH << GPIO_OSPEEDR_OSPEED3_Pos;

    /* Configure the IO Output Type to Push-Pull */
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2_Msk | GPIO_OTYPER_OT3_Msk);

    /* Set the Pull-up/down  to none */
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2_Msk | GPIO_PUPDR_PUPD3_Msk);

    /* Configure Alternate functions */
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk | GPIO_AFRL_AFSEL3_Msk);
    GPIOA->AFR[0] |= GPIO_AF7_USART2 << GPIO_AFRL_AFSEL2_Pos;
    GPIOA->AFR[0] |= GPIO_AF7_USART2 << GPIO_AFRL_AFSEL3_Pos;

    /* Configure IO Direction mode to Alternate */
    GPIOA->MODER &= ~(GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE3_Msk);
    GPIOA->MODER |= GPIO_MODE_AF_PP << GPIO_MODER_MODE2_Pos;
    GPIOA->MODER |= GPIO_MODE_AF_PP << GPIO_MODER_MODE3_Pos;

    //////////// CONFIGURE USART2 /////////////////
    // Disable UART
    USART2->CR1 &= ~USART_CR1_UE;

    /* Set the UART Communication parameters */
    USART2->CR1 &= ~(USART_CR1_M1_Pos | USART_CR1_PCE_Msk | USART_CR1_OVER8_Msk | USART_CR1_TE_Msk | USART_CR1_RE_Msk);
    USART2->CR1 |= UART_WORDLENGTH_8B | UART_PARITY_NONE | UART_OVERSAMPLING_16 | USART_CR1_TE | USART_CR1_RE;

    /*-------------------------- USART CR2 Configuration -----------------------*/
    /* Configure the UART Stop Bits: Set STOP[13:12] bits according to huart->Init.StopBits value */
    USART2->CR2 &= ~USART_CR2_STOP;
    USART2->CR2 |= UART_STOPBITS_1;

    /*-------------------------- USART CR3 Configuration -----------------------*/
    /* Configure
     * - UART HardWare Flow Control: set CTSE and RTSE bits according to HwFlowCtl value
     * - One-bit sampling method versus three samples' majority rule according to huart->Init.OneBitSampling */
    USART2->CR3 &= ~(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT);
    USART2->CR3 |= UART_HWCONTROL_NONE | UART_OVERSAMPLING_16;

    /*-------------------------- USART BRR Configuration -----------------------*/
#define UART_DIV_SAMPLING16(__PCLK__, __BAUD__)  (((__PCLK__) + ((__BAUD__)/2U)) / (__BAUD__))
    U32 pclk = 16000000;        //PCLK1Freq;
    U32 usartdiv = (U16) (UART_DIV_SAMPLING16(pclk, baudrate));
    USART2->BRR = usartdiv;

    /* In asynchronous mode, the following bits must be kept cleared:
     *  - LINEN and CLKEN bits in the USART_CR2 register
     *  - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
    USART2->CR2 &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
    USART2->CR3 &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);

    // Enable UART
    USART2->CR1 |= USART_CR1_UE;

    while ((USART2->ISR & USART_ISR_TEACK) == 0); // Verify that the USART is ready for reception
    while ((USART2->ISR & USART_ISR_REACK) == 0); // Verify that the USART is ready for transmission
}


U8 USART_Read(USART_TypeDef* USARTx)
{
    // SR_RXNE (Read data register not empty) bit is set by hardware
    while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
    // USART resets the RXNE flag automatically after reading DR
    return ((U8) (USARTx->RDR & 0xFF));
    // Reading USART_DR automatically clears the RXNE flag
}

I32 uprintf(const char* format_str, ...)
{
    va_list l;
    va_start(l, format_str);
    char b[1024];
    I32 n = vsnprintf(b, 1024, format_str, l);
    va_end(l);

    USART_Write(u_stdout, (U8*) b, n);
    return n;
}

char* ugetline(char buf[], U32 len)
{
    U32 i;
    for (i = 0; i < len - 1; i++)
    {
        buf[i] = USART_Read(u_stdin);

        // Allow the user to view what they are typing
        USART_Write(u_stdout, (U8*)&buf[i], 1);

        if (buf[i] == '\r')
        {
            // End of line
            char lf = '\n';
            USART_Write(u_stdout, (U8*)&lf, 1);
            break;
        }
    }

    // Add the null terminator
    buf[i] = 0;
    return buf;
}

char ugetc(void)
{
    return (char)USART_Read(u_stdin);
}

void USART_Write(USART_TypeDef* USARTx, const U8* buffer, U32 nBytes)
{
    int i;
    // A byte to be transmitted is written to the TDR (transmit data register), and the TXE (transmit empty) bit is cleared.
    // The TDR is copied to an output shift register for serialization when that register is empty, and the TXE bit is set.
    for (i = 0; i < nBytes; i++)
    {
        while (!(USARTx->ISR & USART_ISR_TXE));                            // wait until TXE (TX empty) bit is set
        USARTx->TDR = buffer[i] & 0xFF;        // writing USART_TDR automatically clears the TXE flag
    }
    while (!(USARTx->ISR & USART_ISR_TC));                                    // wait until TC bit is set
    USARTx->ISR &= ~USART_ISR_TC;
}

void USART_IRQHandler(USART_TypeDef* USARTx, U8* buffer, U32* pRx_counter)
{
    if (USARTx->ISR & USART_ISR_RXNE)
    {                        // Received data
        buffer[*pRx_counter] = USARTx->RDR;         // Reading USART_DR automatically clears the RXNE flag
        (*pRx_counter)++;
        if ((*pRx_counter) >= BufferSize)
        {
            (*pRx_counter) = 0;
        }
    }
    else if (USARTx->ISR & USART_ISR_TXE)
    {
        //USARTx->ISR &= ~USART_ISR_TXE;            // clear interrupt
        //Tx1_Counter++;
    }
    else if (USARTx->ISR & USART_ISR_ORE)
    {            // Overrun Error
        while (1);
    }
    else if (USARTx->ISR & USART_ISR_PE)
    {                // Parity Error
        while (1);
    }
    else if (USARTx->ISR & USART_ISR_PE)
    {                // USART_ISR_FE
        while (1);
    }
    else if (USARTx->ISR & USART_ISR_NE)
    {            // Noise Error Flag
        while (1);
    }
}

