
#include "e.h"

#if E__PORT == E__PORT_OPTION__RPI__RP2350

#include <stdio.h>
#include "stdbool.h"
#include "pico/stdlib.h"

bool repeating_timer_callback( struct repeating_timer *t)
{
    e_tick__irq();
    return true;
}

struct repeating_timer timer;

int32_t e_port__tick_hw_init()
{
    add_repeating_timer_ms(1, repeating_timer_callback, NULL, &timer);
    return 0;
}

#endif
