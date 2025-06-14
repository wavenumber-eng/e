#include "e.h"
#include <zephyr/kernel.h>

#define PRIORITY 0

extern void e_tick__fire();

void thread_e_entry_point(void *dummy1, void *dummy2, void *dummy3)
{
	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);

    e__init();

    while(1)
    {
        e_tick__fire();
        k_sleep(K_MSEC(1));
    }

}


K_THREAD_DEFINE(thread_e, 2048,
				thread_e_entry_point, NULL, NULL, NULL,
				PRIORITY, 0, 0);


