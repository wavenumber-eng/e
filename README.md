
**WORK IN PROGRESS**

# e for embedded

Wavenumber core embedded utilities for baremetal and rtos.   The initial focus is bare metal to have some simple utilities.

These c utilities are intended to be simple helper functions with minimal surface area.

Some of the uilities are pure software (the byte queue) but others require a "port" to enable timing.   For example, the tick functions need to read a timer (ARM systick)

Most embedded applications need the same "stuff".   e is the skeleton of the "stuff"

## core functionality

### e_tick

Covers the basics of being able to read a timer,  check a timeout.   there is a simple mechanism to add functions to a list that get call periodically in a main loop or in an isr context

### e_queue

A simple byte queue for single producer, single consumder

### e_state

A simple statemachine framework.

# Notes:

## Zephyr

Broken... Not yet working

need to get infrastructure setup with some kconfig options to get things to build.

## Ports

## Bare Metal

MCXA and MCXN is working.


