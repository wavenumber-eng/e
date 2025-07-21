#include "e.h"

#if E__PORT == E__PORT_OPTION__ZEPHYR

#include <zephyr/kernel.h>

volatile int test;
void thread_e_entry_point(void *dummy1, void *dummy2, void *dummy3)
{
    ARG_UNUSED(dummy1);
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);

    e__init();

    while (1)
    {
        test++;
        e_tick__irq();
        k_sleep(K_MSEC(CONFIG__E_TICK_PERIOD__MS));
    }
}

K_THREAD_DEFINE(thread_e, CONFIG__E_TICK_THREAD_STACK_SIZE,
				thread_e_entry_point, NULL, NULL, NULL,
				CONFIG__E_TICK_IRQ_PRIORITY, 0, 0);

int32_t e_port__tick_hw_init()
{
  return 0;
}

#endif