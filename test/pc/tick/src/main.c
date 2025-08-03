#include <stdlib.h>
#include <string.h>
#include "e.h"

uint32_t test_tick;

int32_t count=0;

int main(void)
{
   
    //init is called with zephyr sys_init()

    while (1)
    {
        if(count<3)
        {
            if(e_tick__timeout(&test_tick, 1000))
            {
                printf("hello timeout %d\r\n",count++);
            }
        } 
        else
        {
            e_tick__delay_ms(1000);
            
            printk("hello delay %d\r\n",count++);
           
            if(count >= 6)
            {
                test_tick = e_tick__get_ms();
                count =0;
            }
        }  
        k_sleep(K_MSEC(10));
    }
}
