
#ifndef E_PORT_ZEPHYR
#define E_PORT_ZEPHYR

#include <zephyr/kernel.h>

//Some systems have RAM/Flash at this location.  Pick a number that works for you...
#define CONFIG_E_NULL			  		    	   (void *)0

#define E__PORT__HAS_TICK						     (1)

#endif
