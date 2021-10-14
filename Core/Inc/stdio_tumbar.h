//
// Created by tumbar on 10/13/21.
//

#ifndef CMPE663_STDIO_TUMBAR_H
#define CMPE663_STDIO_TUMBAR_H

#include <types.h>
#include <stdarg.h>

#define printf_putchar

/**
 * FreeRTOS safe vprintf
 * (My implementation)
 * @param putchar putchar function to call
 * @param format
 * @param args
 * @return
 */
I32 printf_tumbar(
        void* USARTx,
        const char* format,
        va_list args
);

#endif //CMPE663_STDIO_TUMBAR_H
