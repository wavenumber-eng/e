
#ifndef E_PORT_RPI__RP2350_
#define E_PORT_RPI__RP2350_


#include <stdio.h>
#include "stdbool.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"

//Some systems have RAM/Flash at this location.  Pick a number that works for you...

#define CONFIG__E_LIST_NULL			  			  (void *)0

#define CONFIG__E_TICK_PERIOD__MS			      (1)

#define CONFIG__E_TICK_IRQ_PRIORITY 			  (0)

#define CONFIG__E_WFI						      __wfi()


#endif
