macro(set_default var_name default_value)
    if(NOT DEFINED ${var_name})
        set(${var_name} ${default_value})
    endif()
endmacro()

macro(e_config__set_defaults)
    set_default(CONFIG__E_TRIGGER__ENABLE 1)
    set_default(CONFIG__E_TRIGGER__FOREGROUND_CHECK_INTERVAL_MS 1)
    set_default(CONFIG__E_BQ_MAX_PRINTF_LEN 128)
    set_default(CONFIG__E_VT100__ENABLE 1)
    set_default(CONFIG__E_VT100__DEFAULT_COLOR "E_VT100__WHITE")
    set_default(CONFIG__E_LOG__ENABLE 0)
    set_default(CONFIG__E_LOG__RTT_ENABLE 0)
    set_default(CONFIG__E_LOG__RTT_TX_BUFFER_SIZE 4096)
    set_default(CONFIG__E_SHELL__ENABLE 0)
    set_default(CONFIG__E_SHELL_PRINTF_MAX_LEN 0)
    set_default(CONFIG__E_SHELL_USE_HISTORY 0)
    set_default(CONFIG__E_SHELL_SEARCH_IN_HIST 0)
    set_default(CONFIG__E_SHELL_AUTO_COMPLETE 0)
    set_default(CONFIG__E_SHELL_BUFFER_SIZE 0)
    set_default(CONFIG__E_SHELL_MAX_ARGS 0)
    set_default(CONFIG__E_SHELL_HIST_MAX 0)
    set_default(CONFIG__E_SHELL_MAX_CMD 0)
    set_default(CONFIG__E_ACTIVITY__STACK_DEPTH 16)
    set_default(CONFIG__E_MSG_HEADER_1      0xBE)
    set_default(CONFIG__E_MSG_HEADER_2      0xEF)
    set_default(CONFIG__E_MSG_HEADER_3      0xAA)
    set_default(CONFIG__E_MSG_HEADER_4      0x55)
    
endmacro()

function(e__get_core_src output_var e_base_dir)
    set(E_SRC_DIR "${e_base_dir}/src")
    
    set(CORE_SOURCES
        "${E_SRC_DIR}/e.c"
        "${E_SRC_DIR}/e_tick.c"        
        "${E_SRC_DIR}/e_queue.c"
        "${E_SRC_DIR}/e_state.c"
        "${E_SRC_DIR}/e_btn.c"
        "${E_SRC_DIR}/e_shell.c"
        "${E_SRC_DIR}/e_util.c"
        "${E_SRC_DIR}/e_msg.c"
        "${E_SRC_DIR}/e_trigger.c"
        "${E_SRC_DIR}/e_activity.c"
        
        "${E_SRC_DIR}/port/common/e_tick__arm_cm.c"
        "${E_SRC_DIR}/port/pc/e_tick__pc.c"
        "${E_SRC_DIR}/port/zephyr/e_tick__zephyr.c"
        "${E_SRC_DIR}/port/rpi__rp2xxx/e_tick__rpi_tick.c"
       
    )

    message("E__PORT is set to ${E__PORT}")

    if(CONFIG__E_LOG__RTT_ENABLE)
        list(APPEND CORE_SOURCES
            "${E_SRC_DIR}/external/rtt/SEGGER_RTT.c"
            "${E_SRC_DIR}/external/rtt/SEGGER_RTT_printf.c"
        )
    endif()

    set(${output_var} ${CORE_SOURCES} PARENT_SCOPE)

endfunction()

function(e__add_to_target target_name e_base_dir port_type)
    # Set the port type
    set(E__PORT ${port_type})
    
    # Set defaults
    e_config__set_defaults()
    
    # Set up directories
    set(E_SRC_DIR "${e_base_dir}/src")
    
    # Configure the header file
    configure_file(
        "${E_SRC_DIR}/e_config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/e/e_config.h"
        @ONLY
    )
    
    # Get the core sources
    e__get_core_src(E_SOURCES ${e_base_dir})
    
    # Add sources to target
    target_sources(${target_name} PRIVATE ${E_SOURCES})
    
    # Add include directories
    target_include_directories(${target_name} PUBLIC 
        "${CMAKE_CURRENT_BINARY_DIR}/e"
        "${E_SRC_DIR}"
    )
endfunction()