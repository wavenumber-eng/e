#ifndef E_PORT_NXP__LPC865_H_
#define E_PORT_NXP__LPC865_H_

#ifndef __NVIC_PRIO_BITS
	#define __NVIC_PRIO_BITS 	2
#endif

//Some systems have RAM/Flash at this location.  Pick a number that works for you...
#define CONFIG__E_LIST_NULL			  			  (void *)0

#define CONFIG__E_TICK_IRQ_HANDLER_NAME			   SysTick_Handler

#define CONFIG__E_TICK_PERIOD__MS			      (1)

#define CONFIG__E_TICK_IRQ_PRIORITY		  	      ((1<<__NVIC_PRIO_BITS)-1)

#define CONFIG__E_WFI						       __WFI()

#define CONFIG__E_USE_ARM_SYSTICK				  (1)	

#endif
