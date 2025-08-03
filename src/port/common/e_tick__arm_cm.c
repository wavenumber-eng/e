#include "e.h"

#if CONFIG__E_PORT__USE_ARM_SYSTICK == 1

#ifndef __NVIC_PRIO_BITS
	#define __NVIC_PRIO_BITS 	3
#endif

#ifndef CONFIG__E_TICK__ARM_SYSTICK_IRQ_PRIORITY
    #define CONFIG__E_TICK__ARM_SYSTICK_IRQ_PRIORITY ((1<<__NVIC_PRIO_BITS)-1)
#endif

#ifndef CONFIG__E_TICK__ARM_SYSTICK_PERIOD__MS
    #define CONFIG__E_TICK__ARM_SYSTICK_PERIOD__MS  (1)
#endif

static volatile uint32_t e__delay_ticker;

static volatile uint32_t e__ms_ticker;

void SysTick_Handler()
{
	e__delay_ticker++;
    e__ms_ticker++
}

extern void SystemCoreClockUpdate();

extern uint32_t SystemCoreClock;

int32_t e_port__tick_init()
{
   SystemCoreClockUpdate(); 
 
   SysTick_Config((uint64_t)SystemCoreClock * (uint64_t)CONFIG__E_TICK__ARM_SYSTICK_PERIOD__MS) / 1000);
 
   NVIC_SetPriority(-1 , CONFIG__E_TICK__ARM_SYSTICK_IRQ_PRIORITY);
 
   return 0;
}

uint32_t e_tick__get_ms()
{
    return e__ms_ticker;
}

void e_tick__delay_ms(uint32_t delay_ms)
{
    e__delay_ticker = 0;
  
    while (e__delay_ticker < delay_ms)
    {
        __WFI();
    }
 }

 #endif



