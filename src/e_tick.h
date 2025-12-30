#include <stdint.h>
#include <stdbool.h>

#ifndef E__TICK_H_
#define E__TICK_H_

#ifdef __cplusplus

extern "C"
{
#endif

#include "e_tick_drv_if.h"

/*
 * δt - Get elapsed time since ticker was last set
 * Returns: milliseconds elapsed (handles 32-bit wrap)
 */
uint32_t δt(uint32_t *time_in);

/*
 * τ - Check if timeout period has elapsed
 * Returns: 0 if not expired, elapsed time if expired (resets ticker)
 */
uint32_t τ(uint32_t *time_in, uint32_t period_ms);

/* Legacy aliases */
#define e_tick__delta   δt
#define e_tick__timeout τ

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM__TICK_H_ */
