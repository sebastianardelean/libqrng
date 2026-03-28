set(SOURCE_FILES randbytes.c)

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include
                  ${CMAKE_BINARY_DIR}/generated
)


add_executable(randbytes ${SOURCE_FILES})

target_include_directories(randbytes
    PUBLIC ${INCLUDE_FILES}

)
target_link_libraries(randbytes PRIVATE
	library_options
    libcurl
    qrng
	)
    


install(TARGETS randbytes DESTINATION /usr/bin)

