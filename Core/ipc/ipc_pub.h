//
// Created by tumbar on 10/11/21.
//

#ifndef CMPE663_IPC_PUB_H
#define CMPE663_IPC_PUB_H

#include <types.h>

// Opaque forward declaration
typedef struct QueuedTask_prv QueuedTask;

typedef enum {
    TASK_UNKNOWN = -1,          //!< Unset task
    TASK_STATISTICS = 0,        //!< Handles statistics over all tellers
    TASK_TELLER_1,              //!< First teller
    TASK_TELLER_2,              //!< Second teller
    TASK_TELLER_3,              //!< Third teller
    TASK_TIM,                   //!< Time keeping (implements alarm mechanism)
    TASK_N
} task_t;

typedef enum {
    IPC_CALL_ASYNC = 0x1,
    IPC_REPLY_ASYNC = 0x2,
} message_flags_t;

typedef void (*MessageReply)(U32 ret);

typedef U32 (*MessageCall)(
        U32 arg1,
        U32 arg2,
        U32 arg3,
        U32 arg4);

/**
 * Make an external call to another task
 * Encode the call inside a message and queue it
 * to the task's message stream
 * @param call Function in the target task to execute
 * @param reply Reply callback to run when message finishes
 * @param ... Arguments to pass to call
 */
void ipc_call(
        task_t task_id,
        MessageCall call,
        MessageReply reply,
        ...);

/**
 * Main loop for a task
 * @param self task queue
 */
void ipc_task_main(task_t task_id);

/**
 * Get currently running task_id
 * @return Current task id
 */
task_t ipc_task_self(void);

/**
 * Register a new IPC message queue for
 * a single task.
 * @param task_id Task ID to register
 * @return Created queue
 */
QueuedTask* ipc_register(task_t task_id);

#endif //CMPE663_IPC_PUB_H
