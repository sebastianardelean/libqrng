set(SOURCE_FILES randdouble.c)

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include
                  ${CMAKE_BINARY_DIR}/generated
)


add_executable(randdouble ${SOURCE_FILES})

target_include_directories(randdouble
    PUBLIC ${INCLUDE_FILES}

)
target_link_libraries(randdouble PRIVATE
	library_options
    libcurl
    qrng
	)
    


install(TARGETS randdouble DESTINATION /usr/bin)

