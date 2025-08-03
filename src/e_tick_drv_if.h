#include <stdint.h>
#include <stdbool.h>

#ifndef E__TICK_DRV_IF_H_
#define E__TICK_DRV_IF_H_

#ifdef __cplusplus

extern "C"
{
#endif

//These must be supplied by the port driver.

uint32_t e_tick__get_ms();

void e_tick__delay_ms(uint32_t delay_ms);

void e_tick__init();

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM__TICK_H_ */
