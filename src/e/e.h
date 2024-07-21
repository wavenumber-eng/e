#include "stdio.h"
#include "stdlib.h"
#include "stddef.h"
#include "stdbool.h"

#include "e_config.h"

#include "e_queue.h"
#include "e_debug.h"
#include "e_shell.h"
#include "e_tick.h"

#ifndef _E_H_
#define _E_H_

void 	e__init();
void 	e__crunch();

#if CONFIG__ENABLE_E_SHELL != 0
	void e__register_system_shell(e_shell__context_t shell);
#endif

#endif /* SYSTEM_H_ */
