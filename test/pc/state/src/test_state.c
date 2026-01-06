/*
 * Unit tests for e_state module
 */

#include "unity.h"
#include "e.h"
#include "mock_tick.h"

/*---------------------------------------------------------------------------
 * Test fixtures and helpers
 *---------------------------------------------------------------------------*/

/* State IDs for test state machine */
enum {
    ST_A = 0,
    ST_B,
    ST_C,
    ST_COUNT
};

/* Tracking for state function calls */
static int state_a_calls = 0;
static int state_b_calls = 0;
static int state_c_calls = 0;
static int transition_calls = 0;
static int last_transition_from = -1;
static int last_transition_to = -1;
static bool allow_transitions = true;

/* State functions */
static int32_t state_a(e_sm_t* sm)
{
    (void)sm;
    state_a_calls++;
    return 0;
}

static int32_t state_b(e_sm_t* sm)
{
    (void)sm;
    state_b_calls++;
    return 0;
}

static int32_t state_c(e_sm_t* sm)
{
    (void)sm;
    state_c_calls++;
    return 0;
}

/* State table */
static e_state_table_entry_t test_states[] = {
    [ST_A] = { state_a, "STATE_A" },
    [ST_B] = { state_b, "STATE_B" },
    [ST_C] = { state_c, "STATE_C" },
};

/* Transition callback */
static bool on_transition(e_sm_t* sm, int32_t next)
{
    transition_calls++;
    last_transition_from = sm->current_state;
    last_transition_to = next;
    return allow_transitions;
}

/* Reset all test state */
static void reset_test_state(void)
{
    state_a_calls = 0;
    state_b_calls = 0;
    state_c_calls = 0;
    transition_calls = 0;
    last_transition_from = -1;
    last_transition_to = -1;
    allow_transitions = true;
}

void setUp(void)
{
    mock_tick_reset();
    reset_test_state();
}

void tearDown(void)
{
}

/*---------------------------------------------------------------------------
 * e_state__init() tests
 *---------------------------------------------------------------------------*/

void test_init_starts_at_state_zero(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);

    TEST_ASSERT_EQUAL_INT32(ST_A, sm.current_state);
}

void test_init_sets_state_table(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);

    TEST_ASSERT_EQUAL_PTR(test_states, sm.state_table);
}

void test_init_sets_transition_callback(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);

    TEST_ASSERT_EQUAL_PTR(on_transition, sm.transition);
}

void test_init_triggers_transition_to_zero(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);

    /* Init calls transition(0) */
    TEST_ASSERT_EQUAL_INT(1, transition_calls);
    TEST_ASSERT_EQUAL_INT(0, last_transition_to);
}

/*---------------------------------------------------------------------------
 * e_state__crunch() tests
 *---------------------------------------------------------------------------*/

void test_crunch_runs_current_state_function(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);

    /* First crunch completes transition, second runs state */
    e_state__crunch(&sm);
    e_state__crunch(&sm);

    TEST_ASSERT_GREATER_THAN(0, state_a_calls);
}

void test_crunch_only_runs_current_state(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);

    e_state__crunch(&sm);
    e_state__crunch(&sm);
    e_state__crunch(&sm);

    TEST_ASSERT_GREATER_THAN(0, state_a_calls);
    TEST_ASSERT_EQUAL_INT(0, state_b_calls);
    TEST_ASSERT_EQUAL_INT(0, state_c_calls);
}

/*---------------------------------------------------------------------------
 * e_state__transition() tests
 *---------------------------------------------------------------------------*/

void test_transition_changes_state(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);  /* Complete init transition */

    e_state__transition(&sm, ST_B);
    e_state__crunch(&sm);  /* Complete transition */

    TEST_ASSERT_EQUAL_INT32(ST_B, sm.current_state);
}

void test_transition_calls_callback(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);

    reset_test_state();  /* Clear init transition */

    e_state__transition(&sm, ST_B);

    TEST_ASSERT_EQUAL_INT(1, transition_calls);
    TEST_ASSERT_EQUAL_INT(ST_A, last_transition_from);
    TEST_ASSERT_EQUAL_INT(ST_B, last_transition_to);
}

void test_transition_rejected_by_callback(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);

    allow_transitions = false;
    e_state__transition(&sm, ST_B);
    e_state__crunch(&sm);

    /* Should stay in ST_A */
    TEST_ASSERT_EQUAL_INT32(ST_A, sm.current_state);
}

void test_transition_resets_state_tick(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);

    mock_tick_set(1000);
    e_state__transition(&sm, ST_B);
    e_state__crunch(&sm);

    TEST_ASSERT_EQUAL_UINT32(1000, sm.state_tick);
}

/*---------------------------------------------------------------------------
 * e_state__wait() tests
 *---------------------------------------------------------------------------*/

void test_wait_parks_state_machine(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);

    reset_test_state();

    /* State A calls wait */
    e_state__wait(&sm, 100);

    /* Crunch should not run state function while waiting */
    e_state__crunch(&sm);
    e_state__crunch(&sm);

    TEST_ASSERT_EQUAL_INT(0, state_a_calls);
}

void test_wait_resumes_after_timeout(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);

    reset_test_state();
    mock_tick_set(0);

    e_state__wait(&sm, 100);

    /* Still waiting */
    mock_tick_set(50);
    e_state__crunch(&sm);
    TEST_ASSERT_EQUAL_INT(0, state_a_calls);

    /* Timeout expired */
    mock_tick_set(100);
    e_state__crunch(&sm);
    TEST_ASSERT_EQUAL_INT(1, state_a_calls);
}

void test_wait_stays_in_same_state(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);

    e_state__wait(&sm, 100);
    mock_tick_advance(200);
    e_state__crunch(&sm);

    /* Should still be in ST_A */
    TEST_ASSERT_EQUAL_INT32(ST_A, sm.current_state);
}

/*---------------------------------------------------------------------------
 * e_state__delayed_transition() tests
 *---------------------------------------------------------------------------*/

void test_delayed_transition_waits(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);

    mock_tick_set(0);
    e_state__delayed_transition(&sm, ST_B, 100);

    /* Before timeout - still in ST_A */
    mock_tick_set(50);
    e_state__crunch(&sm);
    TEST_ASSERT_EQUAL_INT32(ST_A, sm.current_state);
}

void test_delayed_transition_transitions_after_timeout(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);

    mock_tick_set(0);
    e_state__delayed_transition(&sm, ST_B, 100);

    /* After timeout - should transition */
    mock_tick_set(100);
    e_state__crunch(&sm);  /* Timeout triggers transition */
    e_state__crunch(&sm);  /* Complete transition */

    TEST_ASSERT_EQUAL_INT32(ST_B, sm.current_state);
}

void test_delayed_transition_calls_callback(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);

    reset_test_state();
    mock_tick_set(0);
    e_state__delayed_transition(&sm, ST_C, 50);

    mock_tick_set(50);
    e_state__crunch(&sm);

    TEST_ASSERT_EQUAL_INT(1, transition_calls);
    TEST_ASSERT_EQUAL_INT(ST_C, last_transition_to);
}

/*---------------------------------------------------------------------------
 * state_tick behavior tests
 *---------------------------------------------------------------------------*/

static uint32_t captured_tick = 0;

static int32_t capture_tick_state(e_sm_t* sm)
{
    captured_tick = sm->state_tick;
    return 0;
}

static e_state_table_entry_t capture_states[] = {
    { capture_tick_state, "CAPTURE" },
};

void test_state_tick_available_in_state_function(void)
{
    captured_tick = 0;

    e_sm_t sm;
    mock_tick_set(500);
    e_state__init(&sm, on_transition, capture_states);
    e_state__crunch(&sm);
    e_state__crunch(&sm);

    /* state_tick should be set to entry time */
    TEST_ASSERT_EQUAL_UINT32(500, captured_tick);
}

/*---------------------------------------------------------------------------
 * Edge cases
 *---------------------------------------------------------------------------*/

void test_null_transition_callback(void)
{
    e_sm_t sm;
    e_state__init(&sm, NULL, test_states);
    e_state__crunch(&sm);

    /* Transition should be ignored with NULL callback */
    e_state__transition(&sm, ST_B);
    e_state__crunch(&sm);

    TEST_ASSERT_EQUAL_INT32(ST_A, sm.current_state);
}

void test_multiple_transitions_same_crunch(void)
{
    e_sm_t sm;
    e_state__init(&sm, on_transition, test_states);
    e_state__crunch(&sm);

    /* Only first transition should take effect */
    e_state__transition(&sm, ST_B);
    e_state__transition(&sm, ST_C);  /* Should be ignored - already transitioning */
    e_state__crunch(&sm);

    TEST_ASSERT_EQUAL_INT32(ST_B, sm.current_state);
}

/*---------------------------------------------------------------------------
 * Test runner
 *---------------------------------------------------------------------------*/

void run_state_tests(void)
{
    /* Init tests */
    RUN_TEST(test_init_starts_at_state_zero);
    RUN_TEST(test_init_sets_state_table);
    RUN_TEST(test_init_sets_transition_callback);
    RUN_TEST(test_init_triggers_transition_to_zero);

    /* Crunch tests */
    RUN_TEST(test_crunch_runs_current_state_function);
    RUN_TEST(test_crunch_only_runs_current_state);

    /* Transition tests */
    RUN_TEST(test_transition_changes_state);
    RUN_TEST(test_transition_calls_callback);
    RUN_TEST(test_transition_rejected_by_callback);
    RUN_TEST(test_transition_resets_state_tick);

    /* Wait tests */
    RUN_TEST(test_wait_parks_state_machine);
    RUN_TEST(test_wait_resumes_after_timeout);
    RUN_TEST(test_wait_stays_in_same_state);

    /* Delayed transition tests */
    RUN_TEST(test_delayed_transition_waits);
    RUN_TEST(test_delayed_transition_transitions_after_timeout);
    RUN_TEST(test_delayed_transition_calls_callback);

    /* state_tick tests */
    RUN_TEST(test_state_tick_available_in_state_function);

    /* Edge cases */
    RUN_TEST(test_null_transition_callback);
    RUN_TEST(test_multiple_transitions_same_crunch);
}

int main(void)
{
    UNITY_BEGIN();
    run_state_tests();
    return UNITY_END();
}
