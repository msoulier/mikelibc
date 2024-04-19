# 1: Set the minimum required version of CMake to be 3.10
cmake_minimum_required(VERSION 3.10)

project(mikelibc
    VERSION 1.0.5
    DESCRIPTION "Mike's personal C library"
    HOMEPAGE_URL https://github.com/msoulier/mikelibc
    LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

configure_file(config.h.in config.h)

add_library(mike STATIC
    madt.c mbase64.c mdebug.c mikelib.c mlog.c mnet.c mstr.c mutil.c)
target_include_directories(mike PUBLIC ${PROJECT_BINARY_DIR})

add_executable(mb64
    mb64.c mbase64.c mdebug.c)

set(CMAKE_C_COMPILER gcc)
