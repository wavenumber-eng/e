cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})


project(tick_test VERSION 1.0.0)


target_include_directories(app PRIVATE 
                          ${CMAKE_SOURCE_DIR}/src
                          )

target_sources(app PRIVATE 

   src/main.c 

)

