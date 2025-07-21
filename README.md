
# e for embedded

Wavenumber core embedded utilities for baremetal and rtos.   The initial focus is bare metal to have some simple utilities.

These c utilities are intended to be simple helper functions with minimal surface area.

Some of the uilities are pure software (the byte queue) but others require a "port" to enable timing.   For example, the tick functions need to read a timer (ARM systick)

Most embedded applications need the same "stuff".   e is the skeleton of the "stuff"

## core functionality

### e_tick

Covers the basics of being able to read a timer,  check a timeout.   there is a simple mechanism to add functions to a list that get call periodically in a main loop or in an isr context

currently focus on simple 1msec ticks w/ 32-bit atomic read for basic task scheduling.

### e_queue

A simple byte queue for single producer, single consumer

### e_state

A simple statemachine framework.

# Notes:

## Zephyr

get infrastructure setup with some kconfig options to get things to build.

## Ports

## Bare Metal

MCXA and MCXN is working.

## directory struct

### cmake

This has some of the helper cmake files, etc

### lib/zephyr

Zephyr specific module setups

### src

The core .c/.h files

### test

Test projects.  Current have 1 project for zephyr tick.   Will add unity tests, etc.

### zephyr

This has the module.yml for pointing zephyr




