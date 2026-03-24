#include "e.h"


void e_state__init(e_state_machine_t *sm,
				   e_state_transition_function_t state_transitition_function,
				   e_state_table_entry_t *state_table
				   )
{

	sm->current_state = 0;
	sm->queued_state = 0;
	sm->master_state = e_state__crunching;
	sm->transition = state_transitition_function;
	sm->state_table = state_table;
	e_state__transition(sm,0);
}

void e_state__crunch(e_state_machine_t *sm)
{

	if(sm->master_state == e_state__waiting)
	{
		if(e_tick__timeout(&(sm->state_tick),sm->time_to_wait))
		{
			sm->master_state = e_state__crunching;

			if(sm->queued_state != sm->current_state)
			{
				e_state__transition(sm,sm->queued_state);
			}

		}
	}

	if(sm->master_state == e_state__transitioning)
	{
		sm->current_state = sm->queued_state;
		sm->master_state = e_state__crunching;
		sm->state_tick = e_tick__get_ms();
	}

	if (sm->master_state == e_state__crunching)
	{
		if(sm->state_table != CONFIG_E_NULL)
		{
			sm->state_table[sm->current_state].state_function(sm);
		}
	}
}

void e_state__transition(e_state_machine_t * sm,
						 int32_t next_state)
{
	if (sm->master_state == e_state__crunching)
	{
		if(sm->transition != CONFIG_E_NULL)
		{
			if(sm->transition(sm,next_state) == true)
			{
					sm->queued_state = next_state;
					sm->master_state = e_state__transitioning;
			}
		}
	}

}

void e_state__wait(e_state_machine_t * sm,uint32_t ms_to_wait)
{
	if (sm->master_state == e_state__crunching)
	{
		sm->master_state = e_state__waiting;
		sm->state_tick = e_tick__get_ms();
		sm->time_to_wait = ms_to_wait;
		sm->queued_state = sm->current_state;
	}
}

void e_state__delayed_transition(e_state_machine_t * sm,
								 int32_t next_state,
								 uint32_t ms_to_wait)
{
	if (sm->master_state == e_state__crunching)
	{
		sm->master_state = e_state__waiting;
		sm->state_tick = e_tick__get_ms();
		sm->time_to_wait = ms_to_wait;
		sm->queued_state = next_state;
	}
}




