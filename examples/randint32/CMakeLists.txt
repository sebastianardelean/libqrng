set(SOURCE_FILES randint32.c)

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include
                  ${CMAKE_BINARY_DIR}/generated
)


add_executable(randint32 ${SOURCE_FILES})

target_include_directories(randint32
    PUBLIC ${INCLUDE_FILES}

)
target_link_libraries(randint32 PRIVATE
	library_options
    libcurl
    qrng
	)
    


install(TARGETS randint32 DESTINATION /usr/bin)

