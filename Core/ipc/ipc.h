//
// Created by tumbar on 10/10/21.
//

#ifndef CMPE663_IPC_H
#define CMPE663_IPC_H

#include <types.h>
#include <FreeRTOS.h>
#include <queue.h>

#include <ipc/ipc_pub.h>

#define IPC_MAX_MSG (8)

typedef struct Message_prv Message;

struct Message_prv {
    U32 arg1;
    U32 arg2;
    U32 arg3;
    U32 arg4;
    message_flags_t flags;
    task_t reply_task_id;
    MessageCall call;
    MessageReply reply;
};

struct QueuedTask_prv {
    task_t id;                          //!< IPC task id
    TaskHandle_t rtos_task;             //!< FreeRTOS task handle
    QueueHandle_t message_queue;        //!< FreeRTOS queue handle
};

/**
 * Clear an IPC message
 * @param self message to clear
 */
void ipc_message_clear(Message* self);

/**
 * Wait for a message to come in and dispatch it
 * @param self Task queue to wait on
 */
void ipc_dispatch_next(QueuedTask* self);

#endif //CMPE663_IPC_H
