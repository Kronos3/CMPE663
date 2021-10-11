//
// Created by tumbar on 10/11/21.
//

#include "tim.h"
#include <FreeRTOS.h>
#include <timers.h>

static U32 tim_start_time = 0;
static U32 tim_end_time = 0;

void tim_sim_start(void)
{
    tim_start_time = xTaskGetTickCount();

    // The bank is open for business between the hours of 9:00am and 4:00pm
    tim_end_time = tim_end_time + tim_time_to_tick(7 * 60, 0);
}

I32 tim_sim_running(void)
{
    return tim_get_time() < tim_end_time;
}

U32 tim_get_time(void)
{
    return xTaskGetTickCount() - tim_start_time;
}

U32 tim_time_to_tick(U32 minute, U32 seconds)
{
    // Tick increment happens at 1000 Hz (configTICK_RATE_HZ)
    // Time should scale such that 100ms is 1 minute (600 times faster than realtime)
    U32 number_seconds = (minute * 60 + seconds);
    return configTICK_RATE_HZ * number_seconds / 600;
}
