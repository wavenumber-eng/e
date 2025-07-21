#include "e_config__port_option.h"

#ifndef _E_CONFIG__PORT_H_
#define _E_CONFIG__PORT_H_

#ifndef E__PORT
    #error "Porting layer not defined for e"
#else

#if E__PORT == E__PORT_OPTION__MCXN

#include <port/nxp__mcxn/e_port__nxp__mcxn.h>

#elif E__PORT == E__PORT_OPTION__ZEPHYR

#include <port/zephyr/e_port__zephyr.h>

#elif E__PORT == E__PORT_OPTION__RPI__RP2350

#include <port/rpi__rp2350/e_port__rpi_rp2350.h>


#elif E__PORT == E__PORT_OPTION__MCXA

#include <port/nxp__mcxa/e_port__nxp__mcxa.h>

#elif E__PORT == E__PORT_OPTION__LPC865

#include <port/nxp__lpc865/e_port__nxp__lpc865.h>

#endif

#ifndef CONFIG__E_LIST_NULL
#error "e port layer needs  CONFIG__E_LIST_NULL  defined"
#endif

#ifndef CONFIG__E_TICK_PERIOD__MS
#error "e port layer needs  CONFIG__E_TICK_PERIOD__MS  defined"
#endif

#ifndef CONFIG__E_TICK_IRQ_PRIORITY
#error "e port layer needs  CONFIG__E_TICK_IRQ_PRIORITY  defined"
#endif

#ifndef CONFIG__E_WFI
#error "e port layer needs  CONFIG__E_WFI  defined"
#endif

#endif

#endif
