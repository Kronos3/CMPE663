//
// Created by tumbar on 10/13/21.
//

#include <stdio_tumbar.h>
#include <stm32l476xx.h>

static inline void usart_putchar(USART_TypeDef* USARTx, U8 c)
{
    while (!(USARTx->ISR & USART_ISR_TXE));                            // wait until TXE (TX empty) bit is set
    USARTx->TDR = c;        // writing USART_TDR automatically clears the TXE flag
}

static inline void usart_flush(USART_TypeDef* USARTx)
{
    while (!(USARTx->ISR & USART_ISR_TC));                                    // wait until TC bit is set
    USARTx->ISR &= ~USART_ISR_TC;
}

static inline void printf_string(
        void* USARTx,
        const char* string)
{
    for (; *string; string++)
    {
        usart_putchar(USARTx, *string);
    }
}

static inline void usart_put_udec(
        USART_TypeDef* USARTx, unsigned i,
        char pad_char, U8 pad_amt)
{
    char buf_[10];
    char* stack_position = buf_;
    if (!i)
    {
        *(stack_position++) = '0';
    }

    while (i)
    {
        *(stack_position++) = '0' + (i % 10);
        i /= 10;
    }

    if ((stack_position - buf_) < pad_amt)
    {
        for (U32 j = 0; j < (pad_amt - (stack_position - buf_)); j++)
        {
            usart_putchar(USARTx, pad_char);
        }
    }

    while (stack_position != buf_)
    {
        usart_putchar(USARTx, *(--stack_position));
    }
}

static inline void usart_put_dec(
        USART_TypeDef* USARTx, int i,
        char pad_char, U8 pad_amt)
{
    if (i < 0)
    {
        usart_putchar(USARTx, '-');
        i *= -1;
    }

    usart_put_udec(USARTx, i, pad_char, pad_amt);
}

static inline void usart_put_hex(
        USART_TypeDef* USARTx, int i, char base_hex_a)
{
    usart_putchar(USARTx, base_hex_a + ((i & 0xF0000000) >> 28));
    usart_putchar(USARTx, base_hex_a + ((i & 0x0F000000) >> 24));
    usart_putchar(USARTx, base_hex_a + ((i & 0x00F00000) >> 20));
    usart_putchar(USARTx, base_hex_a + ((i & 0xF00F0000) >> 16));
    usart_putchar(USARTx, base_hex_a + ((i & 0x0000F000) >> 12));
    usart_putchar(USARTx, base_hex_a + ((i & 0x00000F00) >> 8));
    usart_putchar(USARTx, base_hex_a + ((i & 0x000000F0) >> 4));
    usart_putchar(USARTx, base_hex_a + ((i & 0x0000000F) >> 0));
}

I32 printf_tumbar(
        void* USARTx, const char * const format,
        va_list args
        )
{
    const char* iter = format;
    while (*iter)
    {
        if (*iter == '%')
        {
            char pad_char = 0;
            U8 pad_amt = 0;

            iter++;
            switch(*iter)
            {
                case 's':
                    printf_string(USARTx, va_arg(args, const char*));
                    break;
                case 'c':
                    usart_putchar(USARTx, va_arg(args, int));
                    break;
                case 'd':
                case 'i':
                    usart_put_dec(USARTx, va_arg(args, int), pad_char, pad_amt);
                    break;
                case 'u':
                    usart_put_udec(USARTx, va_arg(args, unsigned), pad_char, pad_amt);
                    break;
                case 'x':
                    usart_put_hex(USARTx, va_arg(args, int), 'a');
                    break;
                case 'X':
                    usart_put_hex(USARTx, va_arg(args, int), 'A');
                    break;
                default:
                    pad_char = *(iter++);
                    pad_amt = (*(iter++) - '0');
                    {
                        if (*iter == 'd' || *iter == 'i')
                        {
                            usart_put_dec(USARTx, va_arg(args, int), pad_char, pad_amt);
                        }
                        else if (*iter == 'u')
                        {
                            usart_put_udec(USARTx, va_arg(args, int), pad_char, pad_amt);
                        }
                    }
                    FW_ASSERT(pad_amt <= 9, pad_amt);
                    break;
            }
        }
        else
        {
            usart_putchar(USARTx, *iter);
        }

        iter++;
    }

    return iter - format;
}