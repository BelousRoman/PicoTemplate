cmake_minimum_required(VERSION 3.13)

# Set program name and it's location
set(PROJECT_NAME Test)
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/test")

# Enable/disable output over UART/USB (1 - enables, 0 - disables)
set(STDIO_UART 1)
set(STDIO_USB 1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set variables for SDK
set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/pico-sdk")
set(PICO_PLATFORM rp2040 CACHE STRING "On-Board Chip")
set(PICO_BOARD pico_w CACHE STRING "Board Type")

set(FREERTOS_KERNEL_PATH "${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel")
set(PICO_PORT_PATH "${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040")
set(FreeRTOS_FAT_PATH "${CMAKE_SOURCE_DIR}/lib/FreeRTOS-FAT")
set(FreeRTOS_FAT_PORT_PATH "${CMAKE_SOURCE_DIR}/lib/FreeRTOS-FAT-Port/src/FreeRTOS+FAT+CLI")

set(LWIP_CFG_PATH ${PROJECT_DIR}/config/lwip)
set(FREERTOS_CFG_PATH ${PROJECT_DIR}/config/FreeRTOS-Kernel)
set(FREERTOS_FAT_CFG_PATH ${PROJECT_DIR}/config/FreeRTOS-FAT)

set(FREERTOS_LIB FreeRTOS-Kernel-Heap4)
set(FREERTOS_FAT_LIB FreeRTOS+FAT+CLI)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include(${PICO_PORT_PATH}/FreeRTOS_Kernel_import.cmake)

add_subdirectory(${FreeRTOS_FAT_PORT_PATH})

message("Building Pico Project ${PROJECT_NAME} with FreeRTOS using ${FREERTOS_LIB} and ${FREERTOS_FAT_LIB} libs")

project(${PROJECT_NAME} C CXX ASM)

include(${PROJECT_DIR}/project.cmake)
