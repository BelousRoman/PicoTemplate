cmake_minimum_required(VERSION 3.13)

if (NOT EXISTS ${FT6336U_LIB})
    set(FT6336U_LIB ft6336u)
endif()

add_library(${FT6336U_LIB} STATIC)

target_sources(${FT6336U_LIB} PUBLIC
    ${PROJECT_DIR}/msp3223/src/ft6336u.c
)
target_include_directories(${FT6336U_LIB} PUBLIC
    ${PROJECT_DIR}/msp3223/hdr
)

if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	target_include_directories(${FT6336U_LIB} PRIVATE
		${PROJECT_DIR}/${FREERTOS_CFG_PATH}
	)
endif()

if (PICO_CYW43_SUPPORTED)
	target_include_directories(${FT6336U_LIB} PRIVATE
		${PROJECT_DIR}/${LWIP_CFG_PATH}
	)
endif()

target_link_libraries(${FT6336U_LIB}
	pico_stdlib
    hardware_i2c
)

if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	target_link_libraries(${FT6336U_LIB}
		${FREERTOS_LIB}
	)
endif()

if (PICO_CYW43_SUPPORTED)
	if (NOT ${INCLUDE_FREERTOS} MATCHES false)
		target_link_libraries(${FT6336U_LIB} 
			pico_cyw43_arch_lwip_sys_freertos
		)
	else()
		target_link_libraries(${FT6336U_LIB} 
			pico_cyw43_arch_none
		)
	endif()
endif()