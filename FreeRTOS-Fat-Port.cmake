
if (NOT DEFINED FreeRTOS_FAT_PORT_PATH)
    set(FreeRTOS_FAT_PORT_PATH "${CMAKE_SOURCE_DIR}/lib/FreeRTOS-FAT-Port//src/FreeRTOS+FAT+CLI")
endif()

if (NOT DEFINED FreeRTOS_FAT_PATH)
    set(FreeRTOS_FAT_PATH "${CMAKE_SOURCE_DIR}/lib/FreeRTOS-FAT")
endif()


if (NOT DEFINED FreeRTOS_FAT_CFG_PATH)
    set(FreeRTOS_FAT_CFG_PATH "${PROJECT_DIR}/config/FreeRTOS-FAT")
endif()

add_library(${FREERTOS_FAT_LIB} INTERFACE)

target_sources(${FREERTOS_FAT_LIB} INTERFACE
        ${FreeRTOS_FAT_PATH}/ff_crc.c
        ${FreeRTOS_FAT_PATH}/ff_dir.c
        ${FreeRTOS_FAT_PATH}/ff_error.c
        ${FreeRTOS_FAT_PATH}/ff_fat.c
        ${FreeRTOS_FAT_PATH}/ff_file.c
        ${FreeRTOS_FAT_PATH}/ff_format.c
        ${FreeRTOS_FAT_PATH}/ff_ioman.c
        ${FreeRTOS_FAT_PATH}/ff_locking.c
        ${FreeRTOS_FAT_PATH}/ff_memory.c
        ${FreeRTOS_FAT_PATH}/ff_stdio.c
        ${FreeRTOS_FAT_PATH}/ff_string.c
        ${FreeRTOS_FAT_PATH}/ff_sys.c
        ${FreeRTOS_FAT_PATH}/ff_time.c 
        ${FreeRTOS_FAT_PORT_PATH}/portable/RP2040/dma_interrupts.c
        ${FreeRTOS_FAT_PORT_PATH}/portable/RP2040/ff_sddisk.c
        ${FreeRTOS_FAT_PORT_PATH}/portable/RP2040/sd_card.c
        ${FreeRTOS_FAT_PORT_PATH}/portable/RP2040/SPI/sd_card_spi.c
        ${FreeRTOS_FAT_PORT_PATH}/portable/RP2040/SPI/sd_spi.c
        ${FreeRTOS_FAT_PORT_PATH}/portable/RP2040/SPI/my_spi.c
        ${FreeRTOS_FAT_PORT_PATH}/portable/RP2040/SDIO/sd_card_sdio.c
        ${FreeRTOS_FAT_PORT_PATH}/portable/RP2040/SDIO/rp2040_sdio.c
        ${FreeRTOS_FAT_PORT_PATH}/src/crc.c
        ${FreeRTOS_FAT_PORT_PATH}/src/sd_timeouts.c
        ${FreeRTOS_FAT_PORT_PATH}/src/ff_utils.c
       ${FreeRTOS_FAT_PORT_PATH}/src/freertos_callbacks.c
        ${FreeRTOS_FAT_PORT_PATH}/src/FreeRTOS_strerror.c
        ${FreeRTOS_FAT_PORT_PATH}/src/FreeRTOS_time.c
        ${FreeRTOS_FAT_PORT_PATH}/src/my_debug.c
        ${FreeRTOS_FAT_PORT_PATH}/src/util.c
)

target_link_libraries(${FREERTOS_FAT_LIB} INTERFACE
        cmsis_core
        ${FREERTOS_LIB}
        hardware_adc
        hardware_clocks
        hardware_dma
        hardware_pio
        hardware_rtc
        hardware_spi
        hardware_timer
        pico_multicore
        pico_stdlib
        pico_sync
)

target_include_directories(${FREERTOS_FAT_LIB} INTERFACE 
        ${FreeRTOS_FAT_PATH}/include/
        ${FreeRTOS_FAT_PORT_PATH}/include/
        ${FreeRTOS_FAT_PORT_PATH}/portable/RP2040/
        ${FreeRTOS_FAT_CFG_PATH}
)