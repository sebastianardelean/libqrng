set(SOURCE_FILES sysinfo.c)

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include
                  ${CMAKE_BINARY_DIR}/generated
)


add_executable(sysinfo ${SOURCE_FILES})

target_include_directories(sysinfo
    PUBLIC ${INCLUDE_FILES}

)
target_link_libraries(sysinfo PRIVATE
	library_options
    libcurl
    qrng
	)
    


install(TARGETS sysinfo DESTINATION /usr/bin)

