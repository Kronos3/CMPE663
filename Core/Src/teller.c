//
// Created by tumbar on 10/11/21.
//

#include <teller.h>
#include <tim.h>
#include <metrics.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <rng.h>
#include <semphr.h>
#include <uart.h>
#include <led.h>

static QueueHandle_t customer_queue;
static SemaphoreHandle_t status_mutex;

static Teller tellers[TELLER_N] = {
        {.id = TELLER_1, .state = TELLER_WAITING},
        {.id = TELLER_2, .state = TELLER_WAITING},
        {.id = TELLER_3, .state = TELLER_WAITING},
};

static Customer garbage_check_buffer;       // Scratch space for peek()
static volatile I32 finished[TELLER_N] = {0};

void teller_init(void)
{
    customer_queue = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(Customer));
    status_mutex = xSemaphoreCreateMutex();
    FW_ASSERT(customer_queue && "Failed to initialize queue");
}

void teller_await_finish(void)
{
    // Join on all the threads
    for (U32 i = 0; i < TELLER_N; i++)
    {
        if (finished[i])
        {
            continue;
        }
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    }
}

void bank_queue_customer(const Customer* customer)
{
    // Wait until the queue has an available spot if it's full
    xQueueSend(customer_queue, customer, portMAX_DELAY);

    // Send the updated queue length to the metrics
    metric_add(0, METRIC_QUEUE_LENGTH, uxQueueMessagesWaiting(customer_queue));
}

static void teller_service(Teller* self, const Customer* customer)
{
    self->state = TELLER_SERVING;
    metric_add(self->id, METRIC_CUSTOMER_WAIT, tim_get_time() - customer->queue_start);
    metric_add(self->id, METRIC_TELLER_WAIT, tim_get_time() - self->start_wait);
    metric_add(self->id, METRIC_TELLER_SERVICE, customer->transaction_time);

    // Wait for the transaction to finish
    vTaskDelay(customer->transaction_time);
}

static void teller_next_customer(Teller* self)
{
    // Initialize the beginning of the wait period if needed
    if (self->state != TELLER_WAITING)
    {
        self->state = TELLER_WAITING;
        self->start_wait = tim_get_time();
    }

    // Only wait until our next break
    Customer customer;
    if (xQueueReceive(customer_queue, &customer, self->next_break - tim_get_time()) == pdTRUE)
    {
        // Handle a customer transaction
        teller_service(self, &customer);
    }
}

static void teller_schedule_break(Teller* self)
{
    // The next break for a teller occurs from 30 to 60 minutes
    // from when they started their previous break
    self->next_break += rng_new(
            tim_time_to_tick(30, 0),
            tim_time_to_tick(60, 0));
}

static void teller_take_break(Teller* self)
{
    // Get a break length
    // Each teller will take a break every 30 to 60 minutes for 1 to 4 minutes.
    U32 break_length = rng_new(
            tim_time_to_tick(1, 0),
            tim_time_to_tick(4, 0)
            );

    // Add the metric before we take the break
    metric_add(self->id, METRIC_TELLER_1_BREAK + self->id, break_length);

    self->state = TELLER_ON_BREAK;
    vTaskDelay(break_length);
}

static const char* teller_status(teller_t id)
{
    FW_ASSERT(id >= 0 && id < TELLER_N, id);

    switch (tellers[id].state)
    {
        case TELLER_WAITING:
            return " WAITING";
        case TELLER_SERVING:
            return " SERVING";
        case TELLER_ON_BREAK:
            return "ON_BREAK";
    }
}

U32 bank_queue_length(void)
{
    return uxQueueMessagesWaiting(customer_queue);
}

void status_task(void* argument)
{
    (void) argument;
    uclear();
    uprintf("=============================\r\n");

    // Each tick is 1ms
    // Lets print every 100ms
    // I know this is relative delay but meh
    xSemaphoreTake(status_mutex, portMAX_DELAY);
    while (tim_sim_running() || xQueuePeek(customer_queue, &garbage_check_buffer, 0))
    {
        U32 hour, min, sec, queue_length;
        human_time(&hour, &min, &sec);
        queue_length = uxQueueMessagesWaiting(customer_queue);

        uprintf("Current time: %02d:%02d:%02d %s\r\n"
                "Customers waiting: %d   \r\n"
                "Teller status: %s, %s, %s\r\n",
                (hour > 12) ? (hour - 12) : hour, min, sec, (hour >= 12) ? "PM" : "AM",
                queue_length,
                teller_status(TELLER_1), teller_status(TELLER_2), teller_status(TELLER_3));

        vTaskDelay(100);
        static const char move_up_str[] = {
                0x1B, 0x5B, '3', 'A', 0x0
        };
        uprintf(move_up_str);
    }
    xSemaphoreGive(status_mutex);

    // Wait for all the tellers to finish up
    teller_await_finish();

    uclear();
    metrics_display();

    vTaskSuspend(xTaskGetCurrentTaskHandle());
    FW_ASSERT(0 && "Status task has finished and was broken out of suspension");
}

void teller_task(void* argument)
{
    // Schedule the first break
    Teller* self = &tellers[(teller_t) argument];
    teller_schedule_break(self);

    while (tim_sim_running() || xQueuePeek(customer_queue, &garbage_check_buffer, 0))
    {
        // Check if we should to go on break
        if (tim_get_time() > self->next_break)
        {
            teller_take_break(self);

            // Schedule the next break
            teller_schedule_break(self);
        }

        // Wait for the next customer
        teller_next_customer(self);
    }

    // Wait until the status process is finished
    xSemaphoreTake(status_mutex, portMAX_DELAY);
    xSemaphoreGive(status_mutex);

    // Teller has finished for the day
    // Tell the parent task that it's done
    finished[self->id] = 1;
    xTaskNotify(xTaskGetHandle("status"), 0, eNoAction);

    // Hang the task (put the task in suspend)
    vTaskSuspend(xTaskGetCurrentTaskHandle());
    FW_ASSERT(0 && "Teller task has finished and was broken out of suspension", self->id);
}
