set(SOURCE_FILES randint16.c)

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include
                  ${CMAKE_BINARY_DIR}/generated
)


add_executable(randint16 ${SOURCE_FILES})

target_include_directories(randint16
    PUBLIC ${INCLUDE_FILES}

)
target_link_libraries(randint16 PRIVATE
	library_options
    libcurl
    qrng
	)
    


install(TARGETS randint16 DESTINATION /usr/bin)

