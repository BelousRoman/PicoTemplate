cmake_minimum_required(VERSION 3.13)

# Add headers and sources
add_executable(${PROJECT_NAME}
	${PROJECT_DIR}/main.c
	${PROJECT_DIR}/hw_config.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
	${FREERTOS_CFG_PATH}
	${FREERTOS_FAT_CFG_PATH}
)

if (PICO_CYW43_SUPPORTED)
	target_include_directories(${PROJECT_NAME} PRIVATE
		${LWIP_CFG_PATH}
	)
endif()

# Set program name and version for picotool
pico_set_program_name(${PROJECT_NAME} "${PROJECT_NAME}")
pico_set_program_version(${PROJECT_NAME} "0.1")

# Enable/disable output over UART/USB (1 - enables, 0 - disables)
pico_enable_stdio_uart(${PROJECT_NAME} ${STDIO_UART})
pico_enable_stdio_usb(${PROJECT_NAME} ${STDIO_USB})

# Add libraries to the build
target_link_libraries(${PROJECT_NAME}
	pico_stdlib
	hardware_adc
	pico_time
	${FREERTOS_LIB}
	${FREERTOS_FAT_LIB}
)

# Link library to project for board with 'W' postfix
if (PICO_CYW43_SUPPORTED)
	target_link_libraries(${PROJECT_NAME} 
		pico_cyw43_arch_lwip_sys_freertos
	)
endif()

add_compile_definitions(TEMPLATE_STR="Raspberry Pi Pico Template Project with FreeRTOS FAT")

pico_add_extra_outputs(${PROJECT_NAME})
