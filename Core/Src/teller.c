//
// Created by tumbar on 10/11/21.
//

#include <teller.h>
#include <tim.h>
#include <metrics.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <rng.h>

static QueueHandle_t customer_queue;
static Customer garbage_check_buffer;       // Scratch space for peek()

void teller_init(void)
{
    customer_queue = xQueueCreate(MAX_QUEUE_LENGTH, sizeof(Customer));
    FW_ASSERT(customer_queue && "Failed to initialize queue");
}

void bank_queue_customer(const Customer* customer)
{
    // Wait until the queue has an available spot if it's full
    xQueueSend(customer_queue, customer, portMAX_DELAY);
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
    metric_add(self->id, METRIC_TELLER_BREAK_1 + self->id, break_length);

    self->state = TELLER_ON_BREAK;
    vTaskDelay(break_length);
}

void teller_task(void* argument)
{
    Teller self = {
            .id = (U32)argument,
            .state = TELLER_WAITING,
            .start_wait = tim_get_time(),
            .next_break = tim_get_time()
    };

    // Schedule the first break
    teller_schedule_break(&self);

    while (tim_sim_running() || xQueuePeek(customer_queue, &garbage_check_buffer, 0))
    {
        // Check if we should to go on break
        if (tim_get_time() > self.next_break)
        {
            teller_take_break(&self);
        }

        // Wait for the next customer
        teller_next_customer(&self);
    }

    // Teller has finished for the day
    // Tell the parent task (TODO)
}
