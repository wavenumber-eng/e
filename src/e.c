#include "e.h"


#if CONFIG_E_SHELL_ENABLE != 0

static e_shell__context_t e_shell__system = NULL;

void e__register_system_shell(e_shell__context_t shell)
{
	e_shell__system = shell;
}

e_shell__context_t e__get_system_shell()
{
	return e_shell__system;
}

#endif

int e__init()
{
	//Supplied by the port hw tick driver
	#if E__PORT__HAS_TICK == 1
		e_tick__init();
	#endif

	return 0;
}

void e__crunch()
{
	#if CONFIG_E_TRIGGER_ENABLE != 0
		e_trig__crunch();
	#endif

	#if CONFIG_E_SHELL_ENABLE != 0

		if(e_shell__system!=NULL)
		{
			e_shell__crunch(e_shell__system);
		}

	#endif

}
