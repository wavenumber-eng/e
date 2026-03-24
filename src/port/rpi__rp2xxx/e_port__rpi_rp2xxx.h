
#ifndef E_PORT_RPI__RP2XXX_
#define E_PORT_RPI__RP2XXX_


#include <stdio.h>
#include "stdbool.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"

//Some systems have RAM/Flash at this location.  Pick a number that works for you...
#define CONFIG_E_NULL			  			         (void *)0

#define E__PORT__HAS_TICK						     (1)

#endif
