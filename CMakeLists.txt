cmake_minimum_required(VERSION 3.13)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

# Set program name and it's location
set(PROJECT_NAME Test)
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/test")
set(PROJECT_VER "0.1")

# Build with libraries if not 'false'
set(INCLUDE_FREERTOS true)
set(INCLUDE_FAT true)
set(INCLUDE_LVGL true)

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

set(FREERTOS_KERNEL_PATH "${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel")
set(FREERTOS_FAT_PATH "${CMAKE_SOURCE_DIR}/lib/FreeRTOS-FAT")
set(FREERTOS_FAT_PORT_PATH "${CMAKE_SOURCE_DIR}/lib/FreeRTOS-FAT-Port")
set(LVGL_PATH "${CMAKE_SOURCE_DIR}/lib/LVGL")
# set(LVGL_PATH "${CMAKE_SOURCE_DIR}/test/lvgl/config/LVGL/lvgl-9.2.2")

set(FREERTOS_PORT_PATH "portable/ThirdParty/GCC/RP2040")

set(FREERTOS_FAT_CFG_PATH "config/FreeRTOS-FAT")
set(FREERTOS_CFG_PATH "config/FreeRTOS-Kernel")
set(LVGL_CFG_PATH "config/LVGL")
set(LWIP_CFG_PATH "config/lwip")

set(FREERTOS_LIB FreeRTOS-Kernel-Heap4)
set(FREERTOS_FAT_LIB FreeRTOS-FAT)
set(LVGL_LIB lvgl)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(${PROJECT_NAME} C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include(FetchContent)

# Get libs sources if include variable(s) is not 'false'
if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	FetchContent_Declare(FreeRTOS_Kernel
	  	GIT_REPOSITORY 		https://github.com/raspberrypi/FreeRTOS-Kernel.git
	  	GIT_TAG        		main
		SOURCE_DIR 			${FREERTOS_KERNEL_PATH}
	)

	message("Fetching FreeRTOS-Kernel")
	FetchContent_Populate(FreeRTOS_Kernel)

	# FetchContent_GetProperties(FreeRTOS_Kernel SOURCE_DIR FREERTOS_KERNEL_PATH)
	
	include(${FREERTOS_KERNEL_PATH}/${FREERTOS_PORT_PATH}/FreeRTOS_Kernel_import.cmake)

	if (NOT ${INCLUDE_FAT} MATCHES false)
		FetchContent_Declare(FreeRTOS_FAT
			GIT_REPOSITORY 	https://github.com/FreeRTOS/Lab-Project-FreeRTOS-FAT.git
			GIT_TAG        	main
			SOURCE_DIR 		${FREERTOS_FAT_PATH}
		)

		message("Fetching FreeRTOS-FAT")
		FetchContent_Populate(FreeRTOS_FAT)

		# FetchContent_GetProperties(FreeRTOS_FAT SOURCE_DIR FREERTOS_FAT_PATH)

		FetchContent_Declare(FreeRTOS_FAT_Port
			GIT_REPOSITORY 	git@github.com:BelousRoman/FreeRTOS-FAT-CLI-for-RPi-Pico.git
			GIT_SUBMODULES 	""
			SOURCE_DIR 		${FREERTOS_FAT_PORT_PATH}
		)

		message("Fetching FreeRTOS-FAT-Port")
		FetchContent_Populate(FreeRTOS_FAT_Port)

		# FetchContent_GetProperties(FreeRTOS_FAT_Port SOURCE_DIR FREERTOS_FAT_PORT_PATH)

		add_subdirectory(${FREERTOS_FAT_PORT_PATH}/src/FreeRTOS+FAT+CLI)
	endif()
endif()

if (NOT ${INCLUDE_LVGL} MATCHES false)
	FetchContent_Declare(LVGL
		# GIT_REPOSITORY 		https://github.com/lvgl/lvgl.git
		GIT_REPOSITORY 		git@github.com:lvgl/lvgl.git
		GIT_TAG 			"v9.2.2"
		SOURCE_DIR 			${LVGL_PATH}
	)

	message("Fetching LVGL")

	set(LV_CONF_PATH "${CMAKE_SOURCE_DIR}/test/lvgl/config/LVGL/lv_conf.h")

	# FetchContent_Populate(LVGL)
	FetchContent_MakeAvailable(LVGL)

	# add_subdirectory(${LVGL_PATH})
endif()

message("Building Pico Project \"${PROJECT_NAME}\"") 

add_subdirectory(${PROJECT_DIR})
