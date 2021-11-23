#include <uart.h>
#include <stdio.h>
#include <stdarg.h>
#include <types.h>

#include <stdio_tumbar.h>


U8 USART_Read(USART_TypeDef* USARTx)
{
    // SR_RXNE (Read data register not empty) bit is set by hardware
    while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
    // USART resets the RXNE flag automatically after reading DR
    return ((U8) (USARTx->RDR & 0xFF));
    // Reading USART_DR automatically clears the RXNE flag
}

void uclear(void)
{
    static const U8 clear_screen_bytes[] = {
            0x1B, 0x5B, 0x32, 0x4A,     // Clear the screen
            0x1B, 0x5B, 0x48,           // Move cursor to home
    };

//    if (usart_lock) xSemaphoreTake(usart_lock, portMAX_DELAY);
    USART_Write(u_stdout, clear_screen_bytes, 7);
//    if (usart_lock) xSemaphoreGive(usart_lock);
}


I32 uprintf(const char* format_str, ...)
{
    va_list l;
//    if (usart_lock) xSemaphoreTake(usart_lock, portMAX_DELAY);
    va_start(l, format_str);
    I32 n = printf_tumbar(u_stdout, format_str, l);
    va_end(l);
//    if (usart_lock) xSemaphoreGive(usart_lock);
    return n;
}

I32 uprintf_no_lock(const char* format_str, ...)
{
    va_list l;
    va_start(l, format_str);
    I32 n = printf_tumbar(u_stdout, format_str, l);
    va_end(l);

    return n;
}

char* ugetline(char buf[], U32 len)
{
    I32 i;
    for (i = 0; i < len - 1; i++)
    {
        buf[i] = USART_Read(u_stdin);

        // Allow the user to view what they are typing
        USART_Write(u_stdout, (U8*) &buf[i], 1);

        if (buf[i] == '\r')
        {
            // End of line
            char lf = '\n';
            USART_Write(u_stdout, (U8*) &lf, 1);
            break;
        }
        else if (buf[i] == 0x7F) // handle the DEL key
        {
            if (i == 0)
            {
                // No more characters to delete
                i--;
                continue;
            }

            const char* b = "\b \b"; // backspace
            USART_Write(u_stdout, (U8*) b, 3);
            i--; // remove the DEL character
            i--; // Delete the last inputted character
        }
    }

    // Add the null terminator
    buf[i] = 0;
    return buf;
}

char ugetc(void)
{
    return (char) USART_Read(u_stdin);
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

