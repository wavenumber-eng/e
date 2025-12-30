#ifndef MOCK_TICK_H
#define MOCK_TICK_H

#include <stdint.h>

// Set the mock time value (what e_tick__get_ms() will return)
void mock_tick_set(uint32_t ms);

// Advance mock time by delta
void mock_tick_advance(uint32_t delta_ms);

// Get current mock time
uint32_t mock_tick_get(void);

// Reset mock time to 0
void mock_tick_reset(void);

#endif // MOCK_TICK_H
