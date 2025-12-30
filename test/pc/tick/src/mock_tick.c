/*
 * Mock tick implementation for unit testing
 * Replaces e_tick__get_ms() with controllable time
 */

#include "mock_tick.h"

static uint32_t mock_time_ms = 0;

void mock_tick_set(uint32_t ms)
{
    mock_time_ms = ms;
}

void mock_tick_advance(uint32_t delta_ms)
{
    mock_time_ms += delta_ms;
}

uint32_t mock_tick_get(void)
{
    return mock_time_ms;
}

void mock_tick_reset(void)
{
    mock_time_ms = 0;
}

/*
 * Override e_tick driver interface functions for testing
 * These replace the real PC port implementations
 */

void e_tick__init(void)
{
    mock_tick_reset();
}

uint32_t e_tick__get_ms(void)
{
    return mock_time_ms;
}

void e_tick__delay_ms(uint32_t delay_ms)
{
    mock_time_ms += delay_ms;
}
