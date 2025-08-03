#include "e.h"

#if E__PORT == E__PORT_OPTION__RPI__RP2XXXX

#include "pico/stdlib.h"

int32_t e_port__tick_init()
{
   return 0;
}

uint32_t e_tick__get_ms()
{

}

void e_tick__delay_ms(uint32_t delay_ms)
{

}



#endif
