cmake_minimum_required(VERSION 3.13)

# Set program name and it's location
set(PROJECT_NAME Test)
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/test")

# Enable/disable output over UART/USB (1 - enables, 0 - disables)
set(STDIO_UART 1)
set(STDIO_USB 1)

# Build with FreeRTOS if not 'false'
set(INCLUDE_FREERTOS true)

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

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Get FreeRTOS-Kernel sources if variable INCLUDE_FREERTOS is not 'false'
if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	FetchContent_Declare(FreeRTOS_Kernel
	  	GIT_REPOSITORY https://github.com/raspberrypi/FreeRTOS-Kernel.git
	  	GIT_TAG        main # Note: Best practice to use specific git-hash or tagged version
	)

	message("Fetching FreeRTOS-Kernel")
	FetchContent_Populate(FreeRTOS_Kernel)

	FetchContent_GetProperties(FreeRTOS_Kernel SOURCE_DIR FREERTOS_KERNEL_PATH)
	set(FREERTOS_CFG_PATH ${PROJECT_DIR}/config/FreeRTOS-Kernel)
	set(PICO_PORT_PATH ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040 CACHE STRING "FreeRTOS RP2040 Port Path" FORCE)
	set(FREERTOS_LIB FreeRTOS-Kernel-Heap4)
	
	include(${PICO_PORT_PATH}/FreeRTOS_Kernel_import.cmake)
	
	message("Building Pico Project ${PROJECT_NAME} with FreeRTOS using ${FREERTOS_LIB} lib")
else()
	message("Building Pico Project ${PROJECT_NAME}") 
endif()

project(${PROJECT_NAME} C CXX ASM)

include(${PROJECT_DIR}/build.cmake)

