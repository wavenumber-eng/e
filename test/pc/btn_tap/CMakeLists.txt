cmake_minimum_required(VERSION 3.12)

project(btn_tap_test)

set(E_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../")
set(E_SRC_DIR "${E_DIR}/src")
set(UNITY_DIR "${CMAKE_CURRENT_LIST_DIR}/../../unity")

# Unity framework
set(UNITY_SOURCES
    "${UNITY_DIR}/unity.c"
)

# E library - only e_btn.c (self-contained, no e_tick dependency)
set(E_CORE_SOURCES
    "${E_SRC_DIR}/e_btn.c"
)

# Test sources
set(TEST_SOURCES
    src/test_btn_tap.c
)

add_executable(btn_tap_test
    ${UNITY_SOURCES}
    ${E_CORE_SOURCES}
    ${TEST_SOURCES}
)

# Configure e_config.h (use TEST port for unit testing)
set(E__PORT "E__PORT_OPTION__TEST")
set(CONFIG_E_LOG_ENABLE 0)
set(CONFIG_E_LOG_RTT_ENABLE 0)
set(CONFIG_E_TRIGGER_ENABLE 0)

include(${E_DIR}cmake/e.cmake)
e_config__set_defaults()

configure_file(
    "${E_SRC_DIR}/e_config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/e/e_config.h"
    @ONLY
)

target_include_directories(btn_tap_test PRIVATE
    src/
    ${E_SRC_DIR}
    ${UNITY_DIR}
    "${CMAKE_CURRENT_BINARY_DIR}/e"
)

target_compile_options(btn_tap_test PRIVATE
    -Wall
    -Wextra
    -fdiagnostics-color=always
)

# Fix Clang/Windows SDK conflict with C11 noreturn macro
target_compile_definitions(btn_tap_test PRIVATE
    UNITY_NORETURN=_Noreturn
)

# CTest integration
enable_testing()
add_test(NAME btn_tap_tests COMMAND btn_tap_test)
