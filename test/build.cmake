cmake_minimum_required(VERSION 3.13)

# Add headers and sources
add_executable(${PROJECT_NAME}
	${PROJECT_DIR}/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${PROJECT_DIR}
)

# Add define-macro
add_compile_definitions(TEMPLATE_STR="Raspberry Pi Pico Template Project")

# Set program name and version for picotool
pico_set_program_name(${PROJECT_NAME} "${PROJECT_NAME}")
pico_set_program_version(${PROJECT_NAME} "0.1")

# Enable/disable output over UART/USB (1 - enables, 0 - disables)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Add libraries to the build
target_link_libraries(${PROJECT_NAME}
	pico_stdlib
	hardware_adc
)

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(${PROJECT_NAME} 
    pico_cyw43_arch_none
    #pico_cyw43_arch_lwip_threadsafe_background
    #pico_stdio_usb
    #pico_lwip_mbedtls
    #pico_mbedtls
)
endif()

pico_add_extra_outputs(${PROJECT_NAME})
