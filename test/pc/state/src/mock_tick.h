/*
 * Mock tick for state machine testing
 */

#ifndef MOCK_TICK_H
#define MOCK_TICK_H

#include <stdint.h>

void mock_tick_set(uint32_t ms);
void mock_tick_advance(uint32_t delta_ms);
uint32_t mock_tick_get(void);
void mock_tick_reset(void);

#endif /* MOCK_TICK_H */
