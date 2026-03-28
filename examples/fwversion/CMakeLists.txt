set(SOURCE_FILES fwversion.c)

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include
                  ${CMAKE_BINARY_DIR}/generated
)


add_executable(fwversion ${SOURCE_FILES})

target_include_directories(fwversion
    PUBLIC ${INCLUDE_FILES}

)
target_link_libraries(fwversion PRIVATE
	library_options
    libcurl
    qrng
	)
    


install(TARGETS fwversion DESTINATION /usr/bin)

