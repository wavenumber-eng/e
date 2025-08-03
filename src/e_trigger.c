
#include <stddef.h> 
#include <stdbool.h> 

#include "e_config.h"
#include "e_tick.h"
#include "e_trigger.h"

#ifndef CONFIG__E_TRIGGER__FOREGROUND_CHECK_INTERVAL_MS
    #warning "CONFIG__E_FOREGROUND_TRIGGER_CHECK_INTERVAL_MS not defined, using 1mS"
    #define CONFIG__E_TRIGGER__FOREGROUND_CHECK_INTERVAL_MS (1)
#endif

static uint32_t trigger_tick;

static e_trigger_t *trigger_list__irq = CONFIG__E_NULL;

static e_trigger_t *trigger_list__foreground = CONFIG__E_NULL;

void e_trig__register(e_trigger_t *trigger, bool irq_exe);

void e_trig__scan(e_trigger_t *trigger_list, uint32_t interval)
{
	e_trigger_t *nt = trigger_list;

	while (nt != CONFIG__E_NULL)
	{
		if (nt->count != 0)
		{
			nt->duration__ms -= interval;

			if (nt->duration__ms <= 0)
			{
				nt->duration__ms += nt->interval__ms;

				nt->marked_for_exe = true;
			}
		}

		nt = nt->next;
	}
}

void e_trig__exe(e_trigger_t *trigger_list)
{
	e_trigger_t *nt = trigger_list;

	while (nt != CONFIG__E_NULL)
	{
		if (nt->hook != NULL)
		{
			if (nt->marked_for_exe)
			{
				nt->hook(nt);
				nt->marked_for_exe = false;

				if (nt->count != E_TRIG__FOREVER)
				{
					nt->count = nt->count - 1;
				}
			}
		}
		nt = nt->next;
	}
}

void e_trig__crunch()
{
	uint32_t interval;

	if ((interval = e_tick__timeout(&trigger_tick, CONFIG__E_TRIGGER__FOREGROUND_CHECK_INTERVAL_MS)))
	{
		e_trig__scan(trigger_list__foreground, interval);
		e_trig__exe(trigger_list__foreground);
	}
}

//This needs called from an IRQ handler in the port to function
void e_trig__irq(uint32_t irq_interval_ms)
{
	e_trig__scan(trigger_list__irq, irq_interval_ms);

	e_trig__exe(trigger_list__irq);
}

void e_trig__init()
{
	trigger_list__irq = CONFIG__E_NULL;
	trigger_list__foreground = CONFIG__E_NULL;
}

void e_trig__register(e_trigger_t *trigger, bool irq_exe)
{
	e_trigger_t **tl;

	if (irq_exe)
	{
		tl = &trigger_list__irq;
	}
	else
	{
		tl = &trigger_list__foreground;
	}

	e_trigger_t *next_trigger = *tl;

	if (next_trigger != CONFIG__E_NULL)
	{
		while (next_trigger->next != CONFIG__E_NULL)
		{
			next_trigger = next_trigger->next;
		}

		next_trigger->next = trigger;
	}
	else // Deal with case where the list is empty
	{
		*tl = trigger;
		next_trigger = trigger;
		next_trigger->next = CONFIG__E_NULL;
	}
}


