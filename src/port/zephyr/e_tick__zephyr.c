#include "e.h"

#if E__PORT == E__PORT_OPTION__ZEPHYR

#include <zephyr/kernel.h>
#include <zephyr/init.h>

uint32_t e_tick__get_ms()
{
    return k_uptime_get_32();
}
  

void e_tick__delay_ms(uint32_t delay_ms)
{
    k_sleep(K_MSEC(delay_ms));
    
}

SYS_INIT(e__init,APPLICATION,0);


#endif