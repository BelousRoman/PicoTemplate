cmake_minimum_required(VERSION 3.13)

if (NOT ${INCLUDE_LVGL} MATCHES false)
    set(PROJECT_DIR "${PROJECT_DIR}/lvgl")

    add_compile_definitions(TEMPLATE_STR="Raspberry Pi Pico Template Project with LVGL")
elseif (NOT ${INCLUDE_FREERTOS} MATCHES false)
    if (NOT ${INCLUDE_FAT} MATCHES false)
        set(PROJECT_DIR "${PROJECT_DIR}/freertos-fat")

        add_compile_definitions(TEMPLATE_STR="Raspberry Pi Pico Template Project with FreeRTOS FAT")
    else()
        set(PROJECT_DIR "${PROJECT_DIR}/freertos")

        add_compile_definitions(TEMPLATE_STR="Raspberry Pi Pico Template Project with FreeRTOS")
    endif()
else()
        set(PROJECT_DIR "${PROJECT_DIR}/standart")

        add_compile_definitions(TEMPLATE_STR="Raspberry Pi Pico Template Project")
endif()

include(${PROJECT_DIR}/project.cmake)

# Set program name and version for picotool
pico_set_program_name(${PROJECT_NAME} "${PROJECT_NAME}")
pico_set_program_version(${PROJECT_NAME} "${PROJECT_VER}")

# Enable/disable output over UART/USB (1 - enables, 0 - disables)
pico_enable_stdio_uart(${PROJECT_NAME} ${STDIO_UART})
pico_enable_stdio_usb(${PROJECT_NAME} ${STDIO_USB})

pico_add_extra_outputs(${PROJECT_NAME})
