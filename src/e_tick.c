#include "e.h"

#ifdef __ZEPHYR__
    #include <zephyr/kernel.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
    #include <time.h>
    #include <sys/time.h>
#endif

static volatile uint32_t e__delay_ticker;

static volatile uint32_t e__ms_ticker;

static uint32_t e_tick__trigger_ticker;

static e_tick__trigger_handler_t *e_tick__trigger_list__irq = CONFIG__E_LIST_NULL;

static e_tick__trigger_handler_t *e_tick__trigger_list__foreground = CONFIG__E_LIST_NULL;

uint32_t e_tick__get_ms()
{
    #ifdef __ZEPHYR__
        return k_uptime_get_32();
    
    #elif defined(_WIN32) || defined(_WIN64)
        // Windows: Use GetTickCount64() or timeGetTime()
        return (uint32_t)GetTickCount();  // Returns milliseconds since boot
    
    #elif defined(__linux__) || defined(__APPLE__)
        // Linux/macOS: Use clock_gettime() with CLOCK_MONOTONIC
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
            return (uint32_t)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
        }
        return 0; // Error fallback
    
    #else
        // Fallback for unknown platforms
        return e__ms_ticker;
    #endif
}
void e_tick__delay_ms(uint32_t delay_ms)
{
    #ifdef __ZEPHYR__
        k_sleep(K_MSEC(delay_ms));
    
    #elif defined(_WIN32) || defined(_WIN64)
        // Windows: Use Sleep() - millisecond precision
        Sleep(delay_ms);
    
    #elif defined(__linux__) || defined(__APPLE__)
        // Linux/macOS: Use nanosleep() - more precise and interruptible
        struct timespec req, rem;
        req.tv_sec = delay_ms / 1000;
        req.tv_nsec = (delay_ms % 1000) * 1000000;
        
        // Handle interruptions (like signals)
        while (nanosleep(&req, &rem) == -1) {
            if (errno == EINTR) {
                req = rem;  // Continue with remaining time
            } else {
                break;  // Other error, give up
            }
        }
    
    #else
        // Fallback for unknown platforms - your original implementation
        e__delay_ticker = 0;
        while (e__delay_ticker < delay_ms) {
            CONFIG__E_WFI;
        }
    #endif
}

uint32_t e_tick__delta(uint32_t *time_in)
{
	volatile uint32_t elapsed;

	if (time_in == NULL)
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

void e_tick__init()
{
	e_tick__trigger_list__irq = CONFIG__E_LIST_NULL;
	e_tick__trigger_list__foreground = CONFIG__E_LIST_NULL;

    e_port__tick_hw_init();
}

void e_tick__trigger_scan(e_tick__trigger_handler_t *trigger_list, uint32_t interval)
{
	e_tick__trigger_handler_t *nt = trigger_list;

	while (nt != CONFIG__E_LIST_NULL)
	{
		if (nt->count != 0)
		{
			nt->duration__mS -= interval;

			if (nt->duration__mS <= 0)
			{
				nt->duration__mS += nt->interval__mS;

				nt->marked_for_exe = true;
			}
		}

		nt = nt->next_item;
	}
}

void e_tick__trigger_exe(e_tick__trigger_handler_t *trigger_list)
{
	e_tick__trigger_handler_t *nt = trigger_list;

	while (nt != CONFIG__E_LIST_NULL)
	{
		if (nt->hook != NULL)
		{
			if (nt->marked_for_exe)
			{
				nt->hook(nt->arg);
				nt->marked_for_exe = false;

				if (nt->count != E_TICK__TRIGGER_FOREVER)
				{
					nt->count = nt->count - 1;
				}
			}
		}
		nt = nt->next_item;
	}
}

void e_tick__register_trigger(e_tick__trigger_handler_t *trigger, bool irq_exe)
{
	e_tick__trigger_handler_t **tl;

	if (irq_exe)
	{
		tl = &e_tick__trigger_list__irq;
	}
	else
	{
		tl = &e_tick__trigger_list__foreground;
	}

	e_tick__trigger_handler_t *next_trigger = *tl;

	if (next_trigger != CONFIG__E_LIST_NULL)
	{
		while (next_trigger->next_item != CONFIG__E_LIST_NULL)
		{
			next_trigger = next_trigger->next_item;
		}

		next_trigger->next_item = trigger;
	}
	else // Deal with case where the list is empty
	{
		*tl = trigger;
		next_trigger = trigger;
		next_trigger->next_item = NULL;
	}
}

void e_tick__crunch()
{
	uint32_t interval;

	if ((interval = e_tick__timeout(&e_tick__trigger_ticker, CONFIG__E_FOREGROUND_TRIGGER_CHECK_INTERVAL_MS)))
	{
		e_tick__trigger_scan(e_tick__trigger_list__foreground, interval);
		e_tick__trigger_exe(e_tick__trigger_list__foreground);
	}
}

void e_tick__irq()
{
	e__delay_ticker += CONFIG__E_TICK_PERIOD__MS;

	e__ms_ticker += CONFIG__E_TICK_PERIOD__MS;

	e_tick__trigger_scan(e_tick__trigger_list__irq, CONFIG__E_TICK_PERIOD__MS);

	e_tick__trigger_exe(e_tick__trigger_list__irq);
}

#if CONFIG__E_USE_ARM_SYSTICK == 1

void SysTick_Handler()
{
	e_tick__irq();
}

extern void SystemCoreClockUpdate()

extern uint32_t SystemCoreClock;

int32_t e_port__tick_hw_init()
{
   SystemCoreClockUpdate(); 
   SysTick_Config((uint32_t)SystemCoreClock * (uint32_t)CONFIG__E_TICK_PERIOD__MS / 1000);
   NVIC_SetPriority(SysTick_IRQn , CONFIG__E_TICK_IRQ_PRIORITY)\

   return 0
}

#endif


