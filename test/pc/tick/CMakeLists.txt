cmake_minimum_required(VERSION 3.12)

project(tick_test)

add_executable(
        tick_test
        src/main.c
        )

set(E_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../")
message("EDIR IS ${E_DIR}")
message("${E_DIR}cmake")

include(${E_DIR}cmake/e.cmake)

set(E__PORT "E__PORT_OPTION__PC")
set(CONFIG__E_LOG__ENABLE 0)
set(CONFIG__E_LOG__RTT_ENABLE 0)


e__add_to_target(tick_test ${E_DIR} "E__PORT_OPTION__PC")


target_include_directories(tick_test PRIVATE src/)


target_compile_options(tick_test PRIVATE -Wall -fdiagnostics-color=always)

