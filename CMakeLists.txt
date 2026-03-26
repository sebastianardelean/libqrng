# Minimum CMake version required
cmake_minimum_required(VERSION 3.20)

# Project name and version
project(qrng VERSION 1.0)
set(PROJECT_NAME qrng)




# Set the C standard to C99 (or C11 if preferred)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)




add_library(project_options INTERFACE)
add_library(library_options INTERFACE)

option(ENABLE_DEBUG "Enable Debug build" ON)

if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
  # using Clang
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  if(ENABLE_DEBUG)
    target_compile_options(project_options INTERFACE -Wall -Wextra -g)
    target_compile_options(library_options INTERFACE -Wall -Wextra -g -fPIC)
  else()
    target_compile_options(project_options INTERFACE -Wall -Wextra -O2)
    target_compile_options(library_options INTERFACE -Wall -Wextra -O2 -fPIC)
  endif()
elseif (CMAKE_C_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
#  target_compile_options(project_options INTERFACE /Zi /W3 /DEXE_TRACE)
#  target_compile_options(library_options INTERFACE /Zi /W3 /DLIB_TRACE)
#  target_link_options(project_options INTERFACE /subsystem:windows /entry:wWinMain)
#  target_link_options(library_options INTERFACE /subsystem:windows /entry:DllMain)
#  set(CMAKE_C_FLAGS_DEBUG "/DDEBUG /Zi /W3")
#  set(CMAKE_C_FLAGS_RELEASE "/Ox")
#  if(ENABLE_DEBUG)
#    target_compile_options(library_options INTERFACE $<$<CONFIG:Debug>:${CMAKE_C_FLAGS_DEBUG}>)
#  else()
#    target_compile_options(library_options INTERFACE $<$<CONFIG:Release>:${CMAKE_C_FLAGS_RELEASE}>)
#  endif()

endif()

include(FetchContent)
# Download spdlog & fmtlib

FetchContent_Declare(
        curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG        8c908d2d0a6d32abdedda2c52e90bd56ec76c24d # release-8.19.0
)

set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(CURL_ZLIB OFF CACHE BOOL "" FORCE)
set(CURL_BROTLI OFF CACHE BOOL "" FORCE)
set(CURL_ZSTD OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(curl
        
        )


add_subdirectory("src")
add_subdirectory("examples")

