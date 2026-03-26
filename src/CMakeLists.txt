
set(SOURCE_FILES qrng.c)

set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include
                  ${CMAKE_BINARY_DIR}/generated
)

add_library(qrng SHARED ${SOURCE_FILES})


target_include_directories(qrng
    PUBLIC ${INCLUDE_FILES}

)
target_link_libraries(qrng PRIVATE
	library_options
    libcurl
	)
    
# Define the export macro for Windows
#target_compile_definitions(demodll PRIVATE MY_LIBRARY_EXPORTS)


install(TARGETS qrng DESTINATION /usr/lib)
install(DIRECTORY ${INCLUDE_FILES}/ DESTINATION /usr/include)
