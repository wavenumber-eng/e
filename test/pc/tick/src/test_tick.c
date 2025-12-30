/*
 * Unit tests for e_tick module
 */

#include "unity.h"
#include "e.h"
#include "mock_tick.h"

void setUp(void)
{
    mock_tick_reset();
}

void tearDown(void)
{
}

/*---------------------------------------------------------------------------
 * e_tick__delta() tests
 *---------------------------------------------------------------------------*/

void test_delta_basic(void)
{
    uint32_t ticker = 0;

    mock_tick_set(0);
    ticker = e_tick__get_ms();

    mock_tick_set(100);
    TEST_ASSERT_EQUAL_UINT32(100, e_tick__delta(&ticker));
}

void test_delta_zero_elapsed(void)
{
    uint32_t ticker;

    mock_tick_set(500);
    ticker = e_tick__get_ms();

    // No time passed
    TEST_ASSERT_EQUAL_UINT32(0, e_tick__delta(&ticker));
}

void test_delta_null_pointer(void)
{
    TEST_ASSERT_EQUAL_UINT32(0, e_tick__delta(NULL));
}

void test_delta_wrap_around_simple(void)
{
    uint32_t ticker;

    // Start near max value
    mock_tick_set(0xFFFFFFF0);
    ticker = e_tick__get_ms();

    // Wrap to small value (32 ticks elapsed)
    mock_tick_set(0x00000010);
    TEST_ASSERT_EQUAL_UINT32(32, e_tick__delta(&ticker));
}

void test_delta_wrap_around_one_tick(void)
{
    uint32_t ticker;

    mock_tick_set(0xFFFFFFFF);
    ticker = e_tick__get_ms();

    mock_tick_set(0x00000000);
    TEST_ASSERT_EQUAL_UINT32(1, e_tick__delta(&ticker));
}

void test_delta_wrap_around_max_range(void)
{
    uint32_t ticker;

    // Start at 1
    mock_tick_set(1);
    ticker = e_tick__get_ms();

    // End at 0 (wrapped around entire range - 1)
    mock_tick_set(0);
    TEST_ASSERT_EQUAL_UINT32(0xFFFFFFFF, e_tick__delta(&ticker));
}

void test_delta_large_value(void)
{
    uint32_t ticker;

    mock_tick_set(0);
    ticker = e_tick__get_ms();

    // ~49.7 days in ms (just under max)
    mock_tick_set(0x80000000);
    TEST_ASSERT_EQUAL_UINT32(0x80000000, e_tick__delta(&ticker));
}

/*---------------------------------------------------------------------------
 * e_tick__timeout() tests
 *---------------------------------------------------------------------------*/

void test_timeout_not_expired(void)
{
    uint32_t ticker;

    mock_tick_set(0);
    ticker = e_tick__get_ms();

    mock_tick_set(50);
    // 50ms elapsed, 100ms timeout - not expired
    TEST_ASSERT_EQUAL_UINT32(0, e_tick__timeout(&ticker, 100));
}

void test_timeout_exactly_expired(void)
{
    uint32_t ticker;

    mock_tick_set(0);
    ticker = e_tick__get_ms();

    mock_tick_set(100);
    // 100ms elapsed, 100ms timeout - just expired
    uint32_t result = e_tick__timeout(&ticker, 100);
    TEST_ASSERT_NOT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_UINT32(100, result);  // Returns elapsed time
}

void test_timeout_overshot(void)
{
    uint32_t ticker;

    mock_tick_set(0);
    ticker = e_tick__get_ms();

    mock_tick_set(150);
    // 150ms elapsed, 100ms timeout - overshot by 50ms
    uint32_t result = e_tick__timeout(&ticker, 100);
    TEST_ASSERT_EQUAL_UINT32(150, result);  // Returns elapsed time
}

void test_timeout_resets_ticker(void)
{
    uint32_t ticker;

    mock_tick_set(0);
    ticker = e_tick__get_ms();

    mock_tick_set(100);
    e_tick__timeout(&ticker, 100);

    // Ticker should now be 100
    TEST_ASSERT_EQUAL_UINT32(100, ticker);
}

void test_timeout_periodic_usage(void)
{
    uint32_t ticker;
    int count = 0;

    mock_tick_set(0);
    ticker = e_tick__get_ms();

    // Simulate periodic checks every 100ms
    for (uint32_t t = 0; t <= 500; t += 25)
    {
        mock_tick_set(t);
        if (e_tick__timeout(&ticker, 100))
        {
            count++;
        }
    }

    // Should fire at 100, 200, 300, 400, 500 = 5 times
    TEST_ASSERT_EQUAL_INT(5, count);
}

void test_timeout_wrap_around(void)
{
    uint32_t ticker;

    // Start near max
    mock_tick_set(0xFFFFFF00);
    ticker = e_tick__get_ms();

    // Wrap around, 512 ticks elapsed
    mock_tick_set(0x00000100);

    uint32_t result = e_tick__timeout(&ticker, 500);
    TEST_ASSERT_NOT_EQUAL(0, result);  // Should be expired (512 >= 500)
}

void test_timeout_zero_delta(void)
{
    uint32_t ticker;

    mock_tick_set(100);
    ticker = e_tick__get_ms();

    // Advance time so elapsed > 0
    mock_tick_set(101);

    // Zero timeout should immediately expire (returns elapsed time)
    uint32_t result = e_tick__timeout(&ticker, 0);
    TEST_ASSERT_NOT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_UINT32(1, result);
}

/*---------------------------------------------------------------------------
 * Test runner
 *---------------------------------------------------------------------------*/

void run_tick_tests(void)
{
    /* Delta tests */
    RUN_TEST(test_delta_basic);
    RUN_TEST(test_delta_zero_elapsed);
    RUN_TEST(test_delta_null_pointer);
    RUN_TEST(test_delta_wrap_around_simple);
    RUN_TEST(test_delta_wrap_around_one_tick);
    RUN_TEST(test_delta_wrap_around_max_range);
    RUN_TEST(test_delta_large_value);

    /* Timeout tests */
    RUN_TEST(test_timeout_not_expired);
    RUN_TEST(test_timeout_exactly_expired);
    RUN_TEST(test_timeout_overshot);
    RUN_TEST(test_timeout_resets_ticker);
    RUN_TEST(test_timeout_periodic_usage);
    RUN_TEST(test_timeout_wrap_around);
    RUN_TEST(test_timeout_zero_delta);
}

int main(void)
{
    UNITY_BEGIN();
    run_tick_tests();
    return UNITY_END();
}
