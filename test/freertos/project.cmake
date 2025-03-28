cmake_minimum_required(VERSION 3.13)

# Add headers and sources
add_executable(${PROJECT_NAME}
	${PROJECT_DIR}/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

# Include config files for FreeRTOS
target_include_directories(${PROJECT_NAME} PRIVATE
    ${FREERTOS_CFG_PATH}
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
)

# Link library to project with 'W' postfix
if (PICO_CYW43_SUPPORTED)
	target_link_libraries(${PROJECT_NAME} 
		pico_cyw43_arch_lwip_sys_freertos
	)
endif()

target_link_libraries(${PROJECT_NAME}
	${FREERTOS_LIB}
)
