cmake_minimum_required(VERSION 3.13)

if (NOT EXISTS ${ILI9341_LIB})
    set(ILI9341_LIB ili9341)
endif()

add_library(${ILI9341_LIB} STATIC)

target_sources(${ILI9341_LIB} PUBLIC
    ${PROJECT_DIR}/msp3223/src/ili9341.c
)
target_include_directories(${ILI9341_LIB} PUBLIC
    ${PROJECT_DIR}/msp3223/hdr
)

if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	target_include_directories(${ILI9341_LIB} PRIVATE
		${PROJECT_DIR}/${FREERTOS_CFG_PATH}
	)
endif()

if (PICO_CYW43_SUPPORTED)
	target_include_directories(${ILI9341_LIB} PRIVATE
		${PROJECT_DIR}/${LWIP_CFG_PATH}
	)
endif()

target_link_libraries(${ILI9341_LIB}
	pico_stdlib
	hardware_pwm
    hardware_spi
)

if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	target_link_libraries(${ILI9341_LIB}
		${FREERTOS_LIB}
	)
endif()

if (PICO_CYW43_SUPPORTED)
	if (NOT ${INCLUDE_FREERTOS} MATCHES false)
		target_link_libraries(${ILI9341_LIB} 
			pico_cyw43_arch_lwip_sys_freertos
		)
	else()
		target_link_libraries(${ILI9341_LIB} 
			pico_cyw43_arch_none
		)
	endif()
endif()