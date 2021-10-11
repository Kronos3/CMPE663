//
// Created by tumbar on 10/11/21.
//

#include <ipc/ipc_pub.h>
#include <teller/teller.h>
#include <tim/tim_pub.h>
#include <metrics/metrics_pub.h>

#define TELLER_IDX(t_id) (t_id - 1)

Teller tellers[3];


void teller_init(void)
{

}

void teller_task(task_t id)
{
    while (1)
    {

    }
}

void teller_handle_customer(U32 queue_entrance_time)
{
    U32 waiting_time = queue_entrance_time - tim_get_time();
    metric_add(ipc_task_self(), METRIC_CUSTOMER_WAIT, waiting_time);


}
