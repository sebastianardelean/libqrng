set(SOURCE_FILES qrandom.c)

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include
                  ${CMAKE_BINARY_DIR}/generated
)


add_executable(qrandom ${SOURCE_FILES})

target_include_directories(qrandom
    PUBLIC ${INCLUDE_FILES}

)
target_link_libraries(qrandom PRIVATE
	library_options
    libcurl
    qrng
	)
    


install(TARGETS qrandom DESTINATION /usr/bin)

