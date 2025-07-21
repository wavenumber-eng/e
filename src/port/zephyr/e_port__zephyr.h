
#ifndef E_PORT_ZEPHYR
#define E_PORT_ZEPHYR

#include <zephyr/kernel.h>

//Some systems have RAM/Flash at this location.  Pick a number that works for you...
#define CONFIG__E_LIST_NULL			  			   (void *)0

#define CONFIG__E_TICK_PERIOD__MS			       (1)

#define CONFIG__E_TICK_IRQ_PRIORITY		  	       (1)      

#define CONFIG__E_WFI			                    			       


#endif
