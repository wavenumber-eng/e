#include <zephyr/kernel.h>

#ifndef E_PORT_ZEPHYR
#define E_PORT_ZEPHYR

//Some systems have RAM/Flash at this location.  Pick a number that works for you...
#define CONFIG__E_LIST_NULL			  			   (void *)0

#define CONFIG__E_TICK_IRQ_HANDLER_NAME			   	e_tick__fire

#define CONFIG__E_TICK_PERIOD__MS			       (1)

#define CONFIG__E_TICK_IRQ_PRIORITY		  	      

#define CONFIG__E_WFI						       

#define CONFIG__E_TICK_INIT()				       


#endif
