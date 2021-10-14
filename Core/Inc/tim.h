//
// Created by tumbar on 10/11/21.
//

#ifndef CMPE663_TIM_H
#define CMPE663_TIM_H

#include <types.h>

/**
 * Start the simulation by initializing the start and end times
 */
void tim_sim_start(void);

/**
 * Check if the simulation is still running
 * (Is the bank still open)
 * @return 1 or 0
 */
I32 tim_sim_running(void);

/**
 * Get the human readable time
 * @param hour pointer to hour destination
 * @param min pointer to minute destination
 * @param sec pointer to second destination
 */
void human_time(U32* hour, U32* min, U32* sec);

/**
 * Get current simulation time
 * @return sim time in ticks
 */
U32 tim_get_time(void);

/**
 * Get tick count given a number of minutes and seconds
 * @param minute
 * @param seconds
 * @return Number of simulation ticks
 */
U32 tim_time_to_tick(U32 minute, U32 seconds);

/**
 * Convert RTOS ticks to minutes
 * @param ticks RTOS ticks
 * @return minutes
 */
U32 tim_tick_to_min(U32 ticks);

/**
 * Convert RTOS ticks to (remainder) seconds
 * @param ticks RTOS ricks
 * @return seconds
 */
U32 tim_tick_to_second(U32 ticks);

#endif //CMPE663_TIM_H
