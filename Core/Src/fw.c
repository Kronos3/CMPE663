//
// Created by tumbar on 9/24/21.
//

#include <uart.h>
#include <stdio.h>
#include <stdarg.h>
#include <led.h>
#include <fw.h>

void fw_assertion_failure(const char* file, U32 line,
                          const char* expr_str,
                          U32 nargs, ...)
{
    // Mask all interrupts
    // Assertion reached, nothing else should run
    __asm__ ("CPSID I");

    uprintf_no_lock(
            "Assertion failed %s:%d : (%s)",
            file, line, expr_str);

    va_list args;
    va_start(args, nargs);
    for (U32 i = 0; i < nargs; i++)
    {
        uprintf_no_lock(", %d", va_arg(args, int));
    }
    va_end(args);
    uprintf_no_lock("\r\n");

    // Hang Mr. CPU please
    // Flash LEDs using TIM5
    __asm__("BKPT");
    while (1)
    {
        // Create a nice little flashing pattern
        // This cannot be mistaken for some status code
        set_led_1(TIM5->CNT & (1 << 11));
        set_led_2(!(TIM5->CNT & (1 << 11)));
        set_led_3(!(TIM5->CNT & (1 << 11)));
        set_led_4(TIM5->CNT & (1 << 11));
    }
}

void print_context_and_fault(
        const ContextFrame* ctx,
        fault_t context_type)
{
    const char* context_type_name;
    switch (context_type)
    {
        case HARD_FAULT:
            context_type_name = "Hardfault";
            break;
        case MEM_MANAGE:
            context_type_name = "MemManage";
            break;
        case BUS_FAULT:
            context_type_name = "BusFault";
            break;
        case USAGE_FAULT:
            context_type_name = "UsageFault";
            break;
        default:
            context_type_name = "Unhandled";
            break;
    }

    uprintf_no_lock(
            "\r\n%s:\r\n"
            "  r0: 0x%08x\r\n"
            "  r1: 0x%08x\r\n"
            "  r2: 0x%08x\r\n"
            "  r3: 0x%08x\r\n"
            " r12: 0x%08x\r\n"
            "  lr: 0x%x\r\n"
            "  pc: 0x%x\r\n"
            "xpsr: 0x%x\r\n",
            context_type_name,
            ctx->r0,
            ctx->r1,
            ctx->r2,
            ctx->r3,
            ctx->r12,
            ctx->lr,
            ctx->pc,
            ctx->xpsr
    );

    FW_ASSERT(0 && "Hard-fault error pc:", ctx->pc);
}
