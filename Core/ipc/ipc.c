//
// Created by tumbar on 10/10/21.
//

#include "ipc.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <stdio.h>
#include <stdarg.h>

static QueuedTask ipc_tasks[TASK_N];

static void ipc_call_generic(task_t task_id,
                             MessageCall call,
                             MessageReply reply,
                             va_list args,
                             message_flags_t flags)
{
    // Validate the task id
    FW_ASSERT(task_id > TASK_UNKNOWN && task_id < TASK_N, task_id);
    FW_ASSERT(call);

    Message msg;
    ipc_message_clear(&msg);

    msg.arg1 = va_arg(args, U32);
    msg.arg2 = va_arg(args, U32);
    msg.arg3 = va_arg(args, U32);
    msg.arg4 = va_arg(args, U32);

    msg.reply_task_id = ipc_task_self();
    msg.flags = flags;
    msg.call = call;
    msg.reply = reply;

    // Actually queue up the task

    // Make sure the queue and tasks are valid
    QueuedTask* target = &ipc_tasks[task_id];
    FW_ASSERT(target->id != TASK_UNKNOWN);
    FW_ASSERT(target->message_queue && "Queue not initialized");

    // Send the message to the task
    // Make sure it succeeded
    switch (xQueueSend(target->message_queue, &msg, 0))
    {
        case pdPASS:
            // Successfully queued message
            break;
        default:
            FW_ASSERT(0 && "Message queue full for task", task_id);
    }
}

void ipc_call(
        task_t task_id,
        MessageCall call,
        MessageReply reply,
        ...)
{
    va_list args;
    va_start(args, reply);
    ipc_call_generic(task_id, call, reply, args, IPC_CALL_ASYNC | IPC_REPLY_ASYNC);
    va_end(args);
}

void ipc_call_sync(
        task_t task_id,
        MessageCall call,
        MessageReply reply,
        ...)
{
    va_list args;
    va_start(args, reply);
    ipc_call_generic(task_id, call, reply, args, IPC_CALL_ASYNC);
    va_end(args);
}

void ipc_init(void)
{
    for (U32 ti = 0; ti < TASK_N; ti++)
    {
        QueuedTask* self = &ipc_tasks[ti];

        self->id = TASK_UNKNOWN;
        self->message_queue = NULL;
        self->rtos_task = NULL;
    }
}

QueuedTask* ipc_register(task_t task_id)
{
    // Validate the task id
    FW_ASSERT(task_id > TASK_UNKNOWN && task_id < TASK_N, task_id);

    // Make sure this task has not already been registered
    // Only one threads should have access to this queue
    FW_ASSERT(ipc_tasks[task_id].id == TASK_UNKNOWN, ipc_tasks[task_id].id);

    // Make sure this RTOS thread is not registered as another IPC task
    TaskHandle_t self_task = xTaskGetCurrentTaskHandle();
    for (U32 i = 0; i < TASK_N; i++)
    {
        if (ipc_tasks[i].id != TASK_UNKNOWN)
        {
            if (ipc_tasks[i].rtos_task == self_task)
            {
                FW_ASSERT(0 && "ipc_register() called multiple times in "
                               "same thread for task", task_id);
            }
        }
    }

    QueuedTask* self = &ipc_tasks[task_id];
    self->id = task_id;
    self->rtos_task = self_task;

    // Create a FreeRTOS queue
    self->message_queue = xQueueCreate(IPC_MAX_MSG, sizeof(Message));
    FW_ASSERT(self->message_queue && "Queue creation failed for task", task_id);

    // Place self into the RTOS thread local storage
    *(QueuedTask**)pvTaskGetThreadLocalStoragePointer(NULL, 0x0) = self;

    return self;
}

void ipc_message_clear(Message* self)
{
    self->arg1 = 0;
    self->arg2 = 0;
    self->arg3 = 0;
    self->arg4 = 0;

    self->reply = NULL;
    self->call = NULL;
}

void ipc_dispatch_next(QueuedTask* self)
{
    FW_ASSERT(self);

    while (1)
    {
        Message message;
        if (xQueueReceive(self->message_queue, &message, portMAX_DELAY) == pdFALSE)
        {
            // No message received (failed)
            // Try again
            continue;
        }

        FW_ASSERT(message.call && "No call embedded in message");

        // Dispatch the message in this thread
        U32 ret = message.call(message.arg1, message.arg2, message.arg3, message.arg4);

        // Reply to the message
        if (message.reply)
        {
            if (message.flags & IPC_REPLY_ASYNC)
            {
                // Queue up the reply in the other task
                ipc_call(message.reply_task_id, (MessageCall) message.reply, NULL, ret);
            }
            else
            {
                // Reply in the same task
                message.reply(ret);
            }
        }

        break;
    }
}

void ipc_task_main(task_t task_id)
{
    // Validate the task id
    FW_ASSERT(task_id > TASK_UNKNOWN && task_id < TASK_N, task_id);

    QueuedTask* self = ipc_register(task_id);
    FW_ASSERT(self && "Failed to register task", task_id);

    while (1)
    {
        ipc_dispatch_next(self);
    }

    // We should never get out of the above loop
    FW_ASSERT(0);
}

task_t ipc_task_self(void)
{
    return (*(QueuedTask**)pvTaskGetThreadLocalStoragePointer(NULL, 0x0))->id;
}
