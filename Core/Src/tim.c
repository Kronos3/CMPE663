//
// Created by tumbar on 10/11/21.
//

#include "tim.h"
#include <FreeRTOS.h>
#include <timers.h>
#include <uart.h>

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

void human_time(U32* hour, U32* min, U32* sec)
{
    U32 current_tim = tim_get_time();

    // Each (100 ms realtime) is (1 min sim-time)
    // Each tick is (1 ms realtime)
    // Each tick is (0.6 seconds sim-time)
    // Time 0 is 9 AM
    // One hour is 6000 ticks

    // Convert ticks to a user readable about
    *hour = 9 + (current_tim / 6000);
    *min = (current_tim % 6000) / 100;
    *sec = ((current_tim * 600) / configTICK_RATE_HZ) % 60;
}

U32 tim_get_time(void)
{
    return xTaskGetTickCount() - tim_start_time;
}

U32 tim_time_to_tick(U32 minute, U32 seconds)
{
    // Tick increment happens at 1000 Hz (configTICK_RATE_HZ)
    // Time should scale such that 100ms is 1 minute (600 times faster than realtime)
    return (minute * 60 + seconds) * 10 / 6;
}

U32 tim_tick_to_min(U32 ticks)
{
    return ((ticks * 600) / configTICK_RATE_HZ) / 60;
}

U32 tim_tick_to_second(U32 ticks)
{
    return ((ticks * 600) / configTICK_RATE_HZ) % 60;
}
