# 1: Set the minimum required version of CMake to be 3.10
cmake_minimum_required(VERSION 3.10)

project(mikelibc
    VERSION 1.0.5
    DESCRIPTION "Mike's personal C library"
    HOMEPAGE_URL https://github.com/msoulier/mikelibc
    LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

add_library(mike STATIC
    madt.c mbase64.c mdebug.c mikelib.c mlog.c mnet.c mstr.c mutil.c)
target_link_libraries(mike PUBLIC ssl crypto curl)
target_include_directories(mike
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
           ${CMAKE_CURRENT_SOURCE_DIR})

add_compile_definitions(MIKELIBC_THREADS=1)

target_compile_definitions(mike PUBLIC
    $<$<CONFIG:Debug>:
        MDEBUG=1
    >)

add_executable(mb64
    mb64.c)
target_link_libraries(mb64 mike)

add_executable(sha1
    sha1.c)
target_link_libraries(sha1 mike)

add_executable(muri
    muri.c)
target_link_libraries(muri mike)

add_executable(bins
    bins.c)
target_link_libraries(bins mike)

message("mikelibc CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/t ${CMAKE_CURRENT_BINARY_DIR}/t)
