cmake_minimum_required(VERSION 3.13)

# Set program name and it's location
set (PROJECT_NAME Test)
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/test")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set variables for SDK
set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/pico-sdk")
set(PICO_PLATFORM rp2040 CACHE STRING "On-Board Chip")
set(PICO_BOARD pico_w CACHE STRING "Board Type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (NOT DEFINED PROJECT_NAME)
	set(PROJECT_NAME Test)
endif()

if (NOT DEFINED PROJECT_DIR)
	set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/test")
endif()

project(${PROJECT_NAME} C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include(${PROJECT_DIR}/build.cmake)

