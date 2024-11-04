# 1: Set the minimum required version of CMake to be 3.10
cmake_minimum_required(VERSION 3.10)

project(mikelibc_tests
    LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_COMPILER gcc)

add_compile_definitions(MDEBUG=1 MIKELIBC_THREADS=1)

function(create_test NAME)
    add_executable(test_${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/test_${NAME}.c)
    target_link_libraries(test_${NAME} mike ssl crypto curl)
    add_test(NAME test_${NAME}
             COMMAND test_${NAME})
endfunction()

create_test(version)
create_test(sha1)
create_test(msplit)
create_test(encrypt_decrypt)
create_test(b64_enc_dec)
create_test(b64_enc_dec_openssl)
create_test(dstnow)
create_test(mlinked_list)

enable_testing()
