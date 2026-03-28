set(SOURCE_FILES randbytestream.c)

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include
                  ${CMAKE_BINARY_DIR}/generated
)


add_executable(randbytestream ${SOURCE_FILES})

target_include_directories(randbytestream
    PUBLIC ${INCLUDE_FILES}

)
target_link_libraries(randbytestream PRIVATE
	library_options
    libcurl
    qrng
	)
    


install(TARGETS randbytestream DESTINATION /usr/bin)

