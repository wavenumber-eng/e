# SPDX-License-Identifier: Apache-2.0

zephyr_library()

if(CONFIG_E)
    # Define multiple symbols at once

    # Add include directories
    zephyr_include_directories(../src/)
    zephyr_include_directories(../src/port/zephyr)

    zephyr_library_sources_ifdef(CONFIG_E    ../src/port/zephyr/e_port__zephyr.c)
    zephyr_library_sources_ifdef(CONFIG_E    ../src/e.c)
    zephyr_library_sources_ifdef(CONFIG_E    ../src/e_tick.c)
    zephyr_library_sources_ifdef(CONFIG_E    ../src/e_queue.c)
    zephyr_library_sources_ifdef(CONFIG_E    ../src/e_state.c)
    zephyr_library_sources_ifdef(CONFIG_E    ../src/e_button.c)
endif()

