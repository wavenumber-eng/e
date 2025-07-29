macro(set_default var_name default_value)
    if(NOT DEFINED ${var_name})
        set(${var_name} ${default_value})
    endif()
endmacro()

macro(e_config__set_defaults)
    set_default(CONFIG__E_FOREGROUND_TRIGGER_CHECK_INTERVAL_MS 1)
    set_default(CONFIG__E_BQ_MAX_PRINTF_LEN 128)
    set_default(CONFIG__E_VT100__ENABLE 1)
    set_default(CONFIG__E_VT100_DEFAULT_COLOR "E_VT100__WHITE")
    set_default(CONFIG__E_LOG__ENABLE 1)
    set_default(CONFIG__E_LOG__RTT_ENABLE 1)
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
endmacro()

function(e__get_core_src output_var e_base_dir)
    set(E_SRC_DIR "${e_base_dir}/src")
    
    set(CORE_SOURCES
        "${E_SRC_DIR}/e.c"
        "${E_SRC_DIR}/e_tick.c"        
        "${E_SRC_DIR}/e_queue.c"
        "${E_SRC_DIR}/e_state.c"
        "${E_SRC_DIR}/e_button.c"
        "${E_SRC_DIR}/e_shell.c"
        "${E_SRC_DIR}/e_util.c"
        "${E_SRC_DIR}/e_msg.c"
    )

    # Add RTT sources conditionally
    if(CONFIG__E_LOG__RTT_ENABLE)
        list(APPEND CORE_SOURCES
            "${E_SRC_DIR}/external/rtt/SEGGER_RTT.c"
            "${E_SRC_DIR}/external/rtt/SEGGER_RTT_printf.c"
        )
    endif()

    # Set the output variable
    set(${output_var} ${CORE_SOURCES} PARENT_SCOPE)
endfunction()

