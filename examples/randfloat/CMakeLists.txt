set(SOURCE_FILES randfloat.c)

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include
                  ${CMAKE_BINARY_DIR}/generated
)


add_executable(randfloat ${SOURCE_FILES})

target_include_directories(randfloat
    PUBLIC ${INCLUDE_FILES}

)
target_link_libraries(randfloat PRIVATE
	library_options
    libcurl
    qrng
	)
    


install(TARGETS randfloat DESTINATION /usr/bin)

