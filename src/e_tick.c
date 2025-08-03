#include "e.h"


#if E__PORT == E__PORT_OPTION__SKELETON
	#define E__USE_SKELETON_TICK 
#else
	#ifndef E__PORT__HAS_TICK
		#warning "This port has no tick driver. Implement tick functions in e_tick_drv_if.h and define E__PORT__HAS_TICK"
		#define E__USE_SKELETON_TICK
	#endif
#endif

#ifdef E__USE_SKELETON_TICK

//Implement placeholders... Not accurate or useful in any way
	
#warning "using skeleton tick implementation"

static volatile uint32_t e__skeleton_ms_ticker;

uint32_t e_tick__get_ms()
{
    return e__skeleton_ms_ticker++;
}

void e_tick__delay_ms(uint32_t delay_ms)
{
	for(int i=0;i<delay_ms;i++)
	{

	}
 }

 #endif



uint32_t e_tick__delta(uint32_t *time_in)
{
	volatile uint32_t elapsed;

	if (time_in == CONFIG__E_NULL)
		return 0;

	elapsed = e_tick__get_ms();

	if (elapsed >= *time_in)
	{
		elapsed -= *time_in;
	}
	else
	{
		elapsed += (0xFFFFFFFF - *time_in);
	}

	return elapsed;
}

uint32_t e_tick__timeout(uint32_t *time_in, uint32_t delta)
{
	volatile uint32_t elapsed;

	elapsed = e_tick__delta(time_in);

	if (elapsed >= delta)
	{
		*time_in = e_tick__get_ms();
		return 1 + (elapsed - delta);
	}

	return 0;
}



