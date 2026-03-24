# SPDX-License-Identifier: Apache-2.0

if(CONFIG_E)
  
    zephyr_library()

    set(E_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../")
    set(E_SRC_DIR "${E_BASE_DIR}/src/")
       
    include(${E_BASE_DIR}/cmake/e.cmake)
    
    set(E__PORT "E__PORT_OPTION__ZEPHYR")

    e__get_core_src(E_SRC_FILES ${E_BASE_DIR})

    zephyr_library_sources(${E_SRC_FILES})

    #This will set all the undefined options to defauts
    e_config__set_defaults()

    configure_file(
        "${E_SRC_DIR}/e_config.h.in"
        "${APPLICATION_BINARY_DIR}/e/e_config.h"
        @ONLY
    )

    zephyr_include_directories(${APPLICATION_BINARY_DIR}/e)
    zephyr_include_directories(${E_SRC_DIR})

endif()

