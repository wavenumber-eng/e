
#ifndef E_PORT_NXP__MCXA_
#define E_PORT_NXP__MCXA_

#include "fsl_device_registers.h"

#ifndef __NVIC_PRIO_BITS
	#define __NVIC_PRIO_BITS 	3
#endif

//Some systems have RAM/Flash at this location.  Pick a number that works for you...
#define CONFIG_E_NULL                               (void *)0

#define CONFIG_E_PORT_USE_ARM_SYSTICK              (1)

#ifndef CONFIG_E_TICK_ARM_SYSTICK_IRQ_PRIORITY
    #define CONFIG_E_TICK_ARM_SYSTICK_IRQ_PRIORITY ((1<<__NVIC_PRIO_BITS)-1)
#endif

#define E__PORT__HAS_TICK                            (1)

#endif
