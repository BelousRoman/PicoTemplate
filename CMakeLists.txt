cmake_minimum_required(VERSION 3.13)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

# Set program name and it's location
set(PROJECT_NAME TestTemplate)
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/test")
set(PROJECT_VER "0.1")

# Build with libraries if not 'false'
set(INCLUDE_FREERTOS false)
set(INCLUDE_FAT false)

# Enable/disable output over UART/USB (1 - enables, 0 - disables)
set(STDIO_UART 1)
set(STDIO_USB 1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set SDK variables, if using VS Code Extension - ignore sdk path and platform
if (NOT EXISTS ${picoVscode})
	set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/pico-sdk")
	set(PICO_PLATFORM rp2040 CACHE STRING "On-Board Chip")
endif()
# Basic options: pico, pico_w, pico_2, pico_2w
set(PICO_BOARD pico_w CACHE STRING "Board type")

set(PICO_PORT_PATH "portable/ThirdParty/GCC/RP2040")

set(FREERTOS_LIB FreeRTOS-Kernel-Heap4)
set(FREERTOS_FAT_LIB FreeRTOS-FAT)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Get libs sources if include variable(s) is not 'false'
if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	FetchContent_Declare(FreeRTOS_Kernel
	  	GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
	  	GIT_TAG        V11.1.0 # Note: Best practice to use specific git-hash or tagged version
	)

	message("Fetching FreeRTOS-Kernel")
	FetchContent_Populate(FreeRTOS_Kernel)

	FetchContent_GetProperties(FreeRTOS_Kernel SOURCE_DIR FREERTOS_KERNEL_PATH)
	
	include(${FREERTOS_KERNEL_PATH}/${PICO_PORT_PATH}/FreeRTOS_Kernel_import.cmake)

	if (NOT ${INCLUDE_FAT} MATCHES false)
		FetchContent_Declare(FreeRTOS_FAT
			GIT_REPOSITORY https://github.com/FreeRTOS/Lab-Project-FreeRTOS-FAT.git
			GIT_TAG        main # Note: Best practice to use specific git-hash or tagged version
		)

		message("Fetching FreeRTOS-FAT")
		FetchContent_Populate(FreeRTOS_FAT)

		FetchContent_GetProperties(FreeRTOS_FAT SOURCE_DIR FREERTOS_FAT_PATH)

		FetchContent_Declare(FreeRTOS_FAT_Port
			GIT_REPOSITORY git@github.com:BelousRoman/FreeRTOS-FAT-CLI-for-RPi-Pico.git
			GIT_SUBMODULES ""
		)

		message("Fetching FreeRTOS-FAT-Port")
		FetchContent_Populate(FreeRTOS_FAT_Port)

		FetchContent_GetProperties(FreeRTOS_FAT_Port SOURCE_DIR FREERTOS_FAT_PORT_PATH)

		add_subdirectory(${FREERTOS_FAT_PORT_PATH}/src/FreeRTOS+FAT+CLI)

		message("Building Pico Project \"${PROJECT_NAME}\" with FreeRTOS using ${FREERTOS_LIB} and ${FREERTOS_FAT_LIB} libs")
	else()
		message("Building Pico Project \"${PROJECT_NAME}\" with FreeRTOS using ${FREERTOS_LIB} lib")
	endif()
else()
	message("Building Pico Project \"${PROJECT_NAME}\"") 
endif()

project(${PROJECT_NAME} C CXX ASM)

add_subdirectory(${PROJECT_DIR})
