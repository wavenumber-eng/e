/*
 * Unit tests for e_btn_tap (tap sequence detector)
 *
 * Tests single, double, triple tap detection, gap window
 * timing, callback mode, linked list processing, and edge cases.
 */

#include "unity.h"
#include "e_btn.h"

/*============================================================
 * Test Configuration
 *============================================================*/
#define PROCESS_MS      10
#define DEBOUNCE_MS     10
#define GAP_WINDOW_MS   400

/*============================================================
 * Mock Button
 *============================================================*/
static uint32_t mock_btn_val = 0;

static bool mock_btn_read(struct e_btn *btn)
{
    (void)btn;
    return mock_btn_val ? true : false;
}

/* Second mock for linked list testing */
static uint32_t mock_btn2_val = 0;

static bool mock_btn2_read(struct e_btn *btn)
{
    (void)btn;
    return mock_btn2_val ? true : false;
}

/*============================================================
 * Callback Tracking
 *============================================================*/
static uint32_t cb_press_count = 0;
static uint32_t cb_call_count = 0;
static e_btn_tap_t *cb_tap_ptr = NULL;

static void test_tap_callback(struct e_btn_tap *tap, uint32_t press_count)
{
    cb_press_count = press_count;
    cb_call_count++;
    cb_tap_ptr = tap;
}

/*============================================================
 * Test Fixtures
 *============================================================*/
static e_btn_tap_t tap;

void setUp(void)
{
    mock_btn_val = 0;
    mock_btn2_val = 0;
    cb_press_count = 0;
    cb_call_count = 0;
    cb_tap_ptr = NULL;
    e_btn_tap__init(&tap, DEBOUNCE_MS, GAP_WINDOW_MS, mock_btn_read);
}

void tearDown(void)
{
}

/*============================================================
 * Helpers
 *============================================================*/

/* Crunch N times at PROCESS_MS intervals */
static void crunch_n(e_btn_tap_t *t, uint32_t n)
{
    for(uint32_t i = 0; i < n; i++)
    {
        e_btn_tap__crunch(t, PROCESS_MS);
    }
}

/*
 * Simulate a complete press and release cycle.
 * With debounce=10ms and process=10ms, we need:
 *   - 3 crunches pressed  (30ms > 10ms debounce) -> DOWN
 *   - 3 crunches released (30ms > 10ms debounce) -> UP
 */
static void press_and_release(e_btn_tap_t *t, uint32_t *mock_val)
{
    *mock_val = 1;
    crunch_n(t, 3);
    *mock_val = 0;
    crunch_n(t, 3);
}

/* Advance time with no button activity */
static void idle_ms(e_btn_tap_t *t, uint32_t ms)
{
    uint32_t n = ms / PROCESS_MS;
    for(uint32_t i = 0; i < n; i++)
    {
        e_btn_tap__crunch(t, PROCESS_MS);
    }
}

/*============================================================
 * Tests: Basic Behavior
 *============================================================*/

void test_no_press_no_event(void)
{
    crunch_n(&tap, 100);
    TEST_ASSERT_EQUAL_UINT32(0, e_btn_tap__get_event(&tap));
}

void test_single_press(void)
{
    press_and_release(&tap, &mock_btn_val);

    /* Event should NOT be available yet (gap window still open) */
    TEST_ASSERT_EQUAL_UINT32(0, e_btn_tap__get_event(&tap));

    /* Wait for gap window to expire */
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);

    TEST_ASSERT_EQUAL_UINT32(1, e_btn_tap__get_event(&tap));
}

void test_single_press_clears_after_read(void)
{
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);

    TEST_ASSERT_EQUAL_UINT32(1, e_btn_tap__get_event(&tap));
    TEST_ASSERT_EQUAL_UINT32(0, e_btn_tap__get_event(&tap));
}

void test_double_press(void)
{
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, 100);
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);

    TEST_ASSERT_EQUAL_UINT32(2, e_btn_tap__get_event(&tap));
}

void test_triple_press(void)
{
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, 100);
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, 100);
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);

    TEST_ASSERT_EQUAL_UINT32(3, e_btn_tap__get_event(&tap));
}

/*============================================================
 * Tests: Window Boundaries
 *============================================================*/

void test_two_singles_outside_window(void)
{
    /* First press → single */
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);
    TEST_ASSERT_EQUAL_UINT32(1, e_btn_tap__get_event(&tap));

    /* Second press → separate single */
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);
    TEST_ASSERT_EQUAL_UINT32(1, e_btn_tap__get_event(&tap));
}

void test_no_event_during_gap_window(void)
{
    press_and_release(&tap, &mock_btn_val);

    /* Check at various points during the gap window */
    idle_ms(&tap, 100);
    TEST_ASSERT_EQUAL_UINT32(0, e_btn_tap__get_event(&tap));

    idle_ms(&tap, 100);
    TEST_ASSERT_EQUAL_UINT32(0, e_btn_tap__get_event(&tap));

    idle_ms(&tap, 100);
    TEST_ASSERT_EQUAL_UINT32(0, e_btn_tap__get_event(&tap));

    /* Now let window expire */
    idle_ms(&tap, 200);
    TEST_ASSERT_EQUAL_UINT32(1, e_btn_tap__get_event(&tap));
}

void test_second_press_resets_gap_timer(void)
{
    press_and_release(&tap, &mock_btn_val);

    /* Wait almost the full window */
    idle_ms(&tap, 350);

    /* Second press just before window expires — should reset timer */
    press_and_release(&tap, &mock_btn_val);

    /* Original window would have expired by now, but timer was reset */
    idle_ms(&tap, 100);
    TEST_ASSERT_EQUAL_UINT32(0, e_btn_tap__get_event(&tap));

    /* Let the reset window expire */
    idle_ms(&tap, GAP_WINDOW_MS);
    TEST_ASSERT_EQUAL_UINT32(2, e_btn_tap__get_event(&tap));
}

/*============================================================
 * Tests: Callback Mode
 *============================================================*/

void test_callback_fires_with_count(void)
{
    tap.event_cb = test_tap_callback;

    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);

    TEST_ASSERT_EQUAL_UINT32(1, cb_press_count);
    TEST_ASSERT_EQUAL_UINT32(1, cb_call_count);
    TEST_ASSERT_EQUAL_PTR(&tap, cb_tap_ptr);
}

void test_callback_double_press(void)
{
    tap.event_cb = test_tap_callback;

    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, 100);
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);

    TEST_ASSERT_EQUAL_UINT32(2, cb_press_count);
    TEST_ASSERT_EQUAL_UINT32(1, cb_call_count);
}

void test_callback_does_not_set_event_flag(void)
{
    tap.event_cb = test_tap_callback;

    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);

    /* Callback should have fired, but event flag should be clear */
    TEST_ASSERT_EQUAL_UINT32(0, e_btn_tap__get_event(&tap));
    TEST_ASSERT_EQUAL_UINT32(1, cb_call_count);
}

/*============================================================
 * Tests: Linked List
 *============================================================*/

void test_linked_list_processes_both(void)
{
    e_btn_tap_t tap2;
    e_btn_tap__init(&tap2, DEBOUNCE_MS, GAP_WINDOW_MS, mock_btn2_read);

    e_btn_tap__add_to_list(&tap, &tap2);

    /* Press tap1 once, tap2 twice */
    mock_btn_val = 1;
    mock_btn2_val = 1;
    crunch_n(&tap, 3);
    mock_btn_val = 0;
    mock_btn2_val = 0;
    crunch_n(&tap, 3);

    /* Second press on tap2 only */
    idle_ms(&tap, 100);
    mock_btn2_val = 1;
    crunch_n(&tap, 3);
    mock_btn2_val = 0;
    crunch_n(&tap, 3);

    /* Wait for both windows to expire */
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);

    TEST_ASSERT_EQUAL_UINT32(1, e_btn_tap__get_event(&tap));
    TEST_ASSERT_EQUAL_UINT32(2, e_btn_tap__get_event(&tap2));
}

void test_add_to_list_returns_true(void)
{
    e_btn_tap_t tap2;
    e_btn_tap__init(&tap2, DEBOUNCE_MS, GAP_WINDOW_MS, mock_btn2_read);

    TEST_ASSERT_TRUE(e_btn_tap__add_to_list(&tap, &tap2));
}

void test_add_to_list_self_returns_false(void)
{
    TEST_ASSERT_FALSE(e_btn_tap__add_to_list(&tap, &tap));
}

/*============================================================
 * Tests: Init / Edge Cases
 *============================================================*/

void test_init_sets_idle_state(void)
{
    TEST_ASSERT_EQUAL_UINT32(BTN_TAP_STATE__IDLE, tap.state);
    TEST_ASSERT_EQUAL_UINT32(0, tap.event);
    TEST_ASSERT_EQUAL_UINT32(0, tap.count);
    TEST_ASSERT_EQUAL_UINT32(GAP_WINDOW_MS, tap.gap_window_ms);
}

void test_multiple_sequences(void)
{
    /* First: double tap */
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, 100);
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);
    TEST_ASSERT_EQUAL_UINT32(2, e_btn_tap__get_event(&tap));

    /* Second: single tap */
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);
    TEST_ASSERT_EQUAL_UINT32(1, e_btn_tap__get_event(&tap));

    /* Third: triple tap */
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, 100);
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, 100);
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);
    TEST_ASSERT_EQUAL_UINT32(3, e_btn_tap__get_event(&tap));
}

void test_event_not_lost_if_not_read(void)
{
    /* First single - don't read it */
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);

    /* Do another single - event should overwrite since not consumed */
    press_and_release(&tap, &mock_btn_val);
    idle_ms(&tap, GAP_WINDOW_MS + PROCESS_MS);

    /* Should get the latest event */
    uint32_t ev = e_btn_tap__get_event(&tap);
    TEST_ASSERT_GREATER_THAN(0, ev);
}


/*============================================================
 * Main
 *============================================================*/

int main(void)
{
    UNITY_BEGIN();

    /* Basic behavior */
    RUN_TEST(test_no_press_no_event);
    RUN_TEST(test_single_press);
    RUN_TEST(test_single_press_clears_after_read);
    RUN_TEST(test_double_press);
    RUN_TEST(test_triple_press);

    /* Window boundaries */
    RUN_TEST(test_two_singles_outside_window);
    RUN_TEST(test_no_event_during_gap_window);
    RUN_TEST(test_second_press_resets_gap_timer);

    /* Callback mode */
    RUN_TEST(test_callback_fires_with_count);
    RUN_TEST(test_callback_double_press);
    RUN_TEST(test_callback_does_not_set_event_flag);

    /* Linked list */
    RUN_TEST(test_linked_list_processes_both);
    RUN_TEST(test_add_to_list_returns_true);
    RUN_TEST(test_add_to_list_self_returns_false);

    /* Init / edge cases */
    RUN_TEST(test_init_sets_idle_state);
    RUN_TEST(test_multiple_sequences);
    RUN_TEST(test_event_not_lost_if_not_read);

    return UNITY_END();
}
