cmake_minimum_required(VERSION 3.12)

project(tick_test)

set(E_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../")
set(E_SRC_DIR "${E_DIR}/src")
set(UNITY_DIR "${CMAKE_CURRENT_LIST_DIR}/../../unity")

# Unity framework (download from https://github.com/ThrowTheSwitch/Unity)
set(UNITY_SOURCES
    "${UNITY_DIR}/unity.c"
)

# E library core sources (only what we need, NO port tick implementations)
set(E_CORE_SOURCES
    "${E_SRC_DIR}/e.c"
    "${E_SRC_DIR}/e_tick.c"
)

# Test sources (includes mock that replaces port tick)
set(TEST_SOURCES
    src/test_tick.c
    src/mock_tick.c
)

add_executable(tick_test
    ${UNITY_SOURCES}
    ${E_CORE_SOURCES}
    ${TEST_SOURCES}
)

# Configure e_config.h (use TEST port for unit testing - no platform headers)
set(E__PORT "E__PORT_OPTION__TEST")
set(CONFIG__E_LOG__ENABLE 0)
set(CONFIG__E_LOG__RTT_ENABLE 0)
set(CONFIG__E_TRIGGER__ENABLE 0)

include(${E_DIR}cmake/e.cmake)
e_config__set_defaults()

configure_file(
    "${E_SRC_DIR}/e_config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/e/e_config.h"
    @ONLY
)

target_include_directories(tick_test PRIVATE
    src/
    ${E_SRC_DIR}
    ${UNITY_DIR}
    "${CMAKE_CURRENT_BINARY_DIR}/e"
)

target_compile_options(tick_test PRIVATE
    -Wall
    -Wextra
    -fdiagnostics-color=always
)

# Fix Clang/Windows SDK conflict with C11 noreturn macro
# Unity includes <stdnoreturn.h> on non-GCC which conflicts with Windows SDK
target_compile_definitions(tick_test PRIVATE
    UNITY_NORETURN=_Noreturn
)

# CTest integration
enable_testing()
add_test(NAME tick_tests COMMAND tick_test)
