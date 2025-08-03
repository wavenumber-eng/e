#include <stdint.h>
#include <stdbool.h>

#ifndef E__TICK_H_
#define E__TICK_H_

#ifdef __cplusplus

extern "C"
{
#endif

#include "e_tick_drv_if.h"

uint32_t e_tick__delta(uint32_t *time_in);

uint32_t e_tick__timeout(uint32_t *time_in, uint32_t delta);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM__TICK_H_ */
