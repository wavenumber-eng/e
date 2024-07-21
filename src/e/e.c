#include "e_config.h"
#include "e.h"

#if CONFIG__ENABLE_E_SHELL != 0

static e_shell__context_t e_shell__system = NULL;


void e__register_system_shell(e_shell__context_t shell)
{
	e_shell__system = shell;
}

#endif

void e__init()
{
	e_tick__init();
}

void e__crunch()
{
	e_tick__crunch();

	#if CONFIG__ENABLE_E_SHELL != 0

		if(e_shell__system!=NULL)
		{
			e_shell__crunch(e_shell__system);
		}

	#endif

}
