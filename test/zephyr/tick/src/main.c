#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>
#include "e.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

#define TEST_LOG    printk

volatile uint32_t test_tick;

volatile int32_t count=0;


int main(void)
{
    
    //init is called int he zephyr e thread.

    while (1)
    {
        if(e_tick__timeout(&test_tick, 1000))
        {
                printk("hello %d\r\n",count++);
        }
        
        k_sleep(K_MSEC(10));
    }
}
