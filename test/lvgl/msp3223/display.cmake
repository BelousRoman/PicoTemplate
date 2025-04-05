cmake_minimum_required(VERSION 3.13)

if (NOT EXISTS ${DISPLAY_LIB})
    set(DISPLAY_LIB msp3223)
endif()

add_library(${DISPLAY_LIB} STATIC)

target_sources(${DISPLAY_LIB} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/display.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ili9341.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ft6336u.c
)

target_include_directories(${DISPLAY_LIB} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/hdr
)

if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	target_include_directories(${DISPLAY_LIB} PRIVATE
		${PROJECT_DIR}/${FREERTOS_CFG_PATH}
	)
endif()

if (PICO_CYW43_SUPPORTED)
	target_include_directories(${DISPLAY_LIB} PRIVATE
		${PROJECT_DIR}/${LWIP_CFG_PATH}
	)
endif()

target_link_libraries(${DISPLAY_LIB}
	pico_stdlib
    hardware_spi
    hardware_pwm
    hardware_i2c
)

if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	target_link_libraries(${DISPLAY_LIB}
		${FREERTOS_LIB}
	)
endif()

if (PICO_CYW43_SUPPORTED)
	if (NOT ${INCLUDE_FREERTOS} MATCHES false)
		target_link_libraries(${DISPLAY_LIB} 
			pico_cyw43_arch_lwip_sys_freertos
		)
	else()
		target_link_libraries(${DISPLAY_LIB} 
			pico_cyw43_arch_none
		)
	endif()
endif()