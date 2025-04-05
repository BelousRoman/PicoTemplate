cmake_minimum_required(VERSION 3.13)

# Add headers and sources
if (NOT ${INCLUDE_FREERTOS} MATCHES false AND NOT ${INCLUDE_FAT} MATCHES false)
	add_executable(${PROJECT_NAME}
	${PROJECT_DIR}/main.c
	${PROJECT_DIR}/hw_config.c
	)
else()
	add_executable(${PROJECT_NAME}
	${PROJECT_DIR}/main.c
	)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	${PROJECT_DIR}/${LVGL_CFG_PATH}
)

if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	target_include_directories(${PROJECT_NAME} PRIVATE
		${PROJECT_DIR}/${FREERTOS_CFG_PATH}
	)

	if (NOT ${INCLUDE_FAT} MATCHES false)
		target_include_directories(${PROJECT_NAME} PRIVATE
			${PROJECT_DIR}/${FREERTOS_FAT_CFG_PATH}
		)
	endif()
endif()

if (PICO_CYW43_SUPPORTED)
	target_include_directories(${PROJECT_NAME} PRIVATE
		${PROJECT_DIR}/${LWIP_CFG_PATH}
	)
endif()

# Add libraries to the build
target_link_libraries(${PROJECT_NAME}
	pico_stdlib
	hardware_adc
	pico_time
	hardware_spi
	${LVGL_LIB}
)

if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	target_link_libraries(${PROJECT_NAME}
		${FREERTOS_LIB}
	)

	if (NOT ${INCLUDE_FAT} MATCHES false)
		target_link_libraries(${PROJECT_NAME}
			${FREERTOS_FAT_LIB}
		)
	endif()
endif()

# Link library to project for board with 'W' postfix
if (PICO_CYW43_SUPPORTED)
	if (NOT ${INCLUDE_FREERTOS} MATCHES false)
		target_link_libraries(${PROJECT_NAME} 
			pico_cyw43_arch_lwip_sys_freertos
		)
	else()
		target_link_libraries(${PROJECT_NAME} 
			pico_cyw43_arch_none
		)
	endif()
endif()

if (NOT ${INCLUDE_FREERTOS} MATCHES false)
	add_compile_definitions(PICO_FREERTOS_BUILD)

	if (NOT ${INCLUDE_FAT} MATCHES false)
		add_compile_definitions(PICO_FREERTOS_FAT_BUILD)
	endif()
endif()
