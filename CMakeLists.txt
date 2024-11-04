# 1: Set the minimum required version of CMake to be 3.10
cmake_minimum_required(VERSION 3.10)

project(mikelibc
    VERSION 1.0.5
    DESCRIPTION "Mike's personal C library"
    HOMEPAGE_URL https://github.com/msoulier/mikelibc
    LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_compile_definitions(MDEBUG=1 MIKELIBC_THREADS=1)

configure_file(config.h.in config.h @ONLY)

add_library(mike STATIC
    madt.c mbase64.c mdebug.c mikelib.c mlog.c mnet.c mstr.c mutil.c)
target_include_directories(mike PUBLIC ${PROJECT_BINARY_DIR})
target_link_libraries(mike)

add_executable(mb64
    mb64.c)
target_link_libraries(mb64 mike curl)

add_executable(sha1
    sha1.c)
target_link_libraries(sha1
    mike ssl crypto curl)

add_executable(muri
    muri.c)
target_link_libraries(muri
    mike ssl crypto curl)

set(CMAKE_C_COMPILER gcc)

enable_testing()
add_test(NAME test_version
         COMMAND test_version
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/t)
add_test(NAME test_sha1
         COMMAND test_sha1
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/t)
add_test(NAME test_msplit
         COMMAND test_msplit
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/t)
add_test(NAME test_encrypt_decrypt
         COMMAND test_encrypt_decrypt
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/t)
add_test(NAME test_b64_enc_dec
         COMMAND test_b64_enc_dec
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/t)
add_test(NAME test_b64_enc_dec_openssl
         COMMAND test_b64_enc_dec_openssl
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/t)
add_test(NAME test_dstnow
         COMMAND test_dstnow
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/t)
