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
