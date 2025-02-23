cmake_minimum_required(VERSION 3.13)

# Add headers and sources
add_executable(${PROJECT_NAME}
	${PROJECT_DIR}/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
	
)

# Include config files for FreeRTOS
if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	target_include_directories(${PROJECT_NAME} PRIVATE
	${PROJECT_DIR}/config/FreeRTOS-Kernel
	)
	
	if (PICO_CYW43_SUPPORTED)
		target_include_directories(${PROJECT_NAME} PRIVATE
			${PROJECT_DIR}/config/lwip
		)
	endif()
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
)

# Link library to project with 'W' postfix
if (PICO_CYW43_SUPPORTED)
	if (NOT ${INCLUDE_FREERTOS} MATCHES false)
		target_link_libraries(${PROJECT_NAME} 
			pico_cyw43_arch_lwip_sys_freertos
		)
	else ()
		target_link_libraries(${PROJECT_NAME} 
			pico_cyw43_arch_none
		)
	endif()
    
endif()

if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	# Link FreeRTOS library
	target_link_libraries(${PROJECT_NAME}
	${FREERTOS_LIB}
	)
	
	# Add define-macros
	add_compile_definitions(TEMPLATE_STR="Raspberry Pi Pico Template Project with FreeRTOS")
	add_compile_definitions(PICO_FREERTOS_BUILD)
else()
	add_compile_definitions(TEMPLATE_STR="Raspberry Pi Pico Template Project")
endif()

pico_add_extra_outputs(${PROJECT_NAME})
