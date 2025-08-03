#include "e.h"

#if( (E__PORT == E__PORT_OPTION__RPI__RP2XXX) || \
    (E__PORT == E__PORT_OPTION__RPI__RP2040)  || \
    (E__PORT == E__PORT_OPTION__RPI__RP2350)  || \
    (E__PORT == E__PORT_OPTION__RPI__RP2354))

    #include "pico/stdlib.h"
#include "pico/time.h"

void e_tick__init()
{

}

uint32_t e_tick__get_ms()
{
   return to_ms_since_boot(get_absolute_time());
}

void e_tick__delay_ms(uint32_t delay_ms)
{
   sleep_ms(delay_ms);
}



#endif
