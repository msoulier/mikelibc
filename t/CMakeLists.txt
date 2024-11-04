# 1: Set the minimum required version of CMake to be 3.10
cmake_minimum_required(VERSION 3.10)

project(mikelibc_tests
    LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_COMPILER gcc)

add_compile_definitions(MDEBUG=1 MIKELIBC_THREADS=1)

add_executable(test_version test_version.c)
target_link_libraries(test_version mike ssl crypto curl)
target_link_directories(test_version PUBLIC ${PROJECT_BINARY_DIR}/../build)
target_include_directories(test_version PUBLIC ${PROJECT_BINARY_DIR}/..)
target_include_directories(test_version PUBLIC ${PROJECT_BINARY_DIR}/../build)

add_executable(test_sha1 test_sha1.c)
target_link_libraries(test_sha1 mike ssl crypto curl)
target_link_directories(test_sha1 PUBLIC ${PROJECT_BINARY_DIR}/../build)
target_include_directories(test_sha1 PUBLIC ${PROJECT_BINARY_DIR}/..)
target_include_directories(test_sha1 PUBLIC ${PROJECT_BINARY_DIR}/../build)

add_executable(test_msplit test_msplit.c)
target_link_libraries(test_msplit mike ssl crypto curl)
target_link_directories(test_msplit PUBLIC ${PROJECT_BINARY_DIR}/../build)
target_include_directories(test_msplit PUBLIC ${PROJECT_BINARY_DIR}/..)
target_include_directories(test_msplit PUBLIC ${PROJECT_BINARY_DIR}/../build)

add_executable(test_encrypt_decrypt test_encrypt_decrypt.c)
target_link_libraries(test_encrypt_decrypt mike ssl crypto curl)
target_link_directories(test_encrypt_decrypt PUBLIC ${PROJECT_BINARY_DIR}/../build)
target_include_directories(test_encrypt_decrypt PUBLIC ${PROJECT_BINARY_DIR}/..)
target_include_directories(test_encrypt_decrypt PUBLIC ${PROJECT_BINARY_DIR}/../build)

add_executable(test_b64_enc_dec test_b64_enc_dec.c)
target_link_libraries(test_b64_enc_dec mike ssl crypto curl)
target_link_directories(test_b64_enc_dec PUBLIC ${PROJECT_BINARY_DIR}/../build)
target_include_directories(test_b64_enc_dec PUBLIC ${PROJECT_BINARY_DIR}/..)
target_include_directories(test_b64_enc_dec PUBLIC ${PROJECT_BINARY_DIR}/../build)

add_executable(test_b64_enc_dec_openssl test_b64_enc_dec_openssl.c)
target_link_libraries(test_b64_enc_dec_openssl mike ssl crypto curl)
target_link_directories(test_b64_enc_dec_openssl PUBLIC ${PROJECT_BINARY_DIR}/../build)
target_include_directories(test_b64_enc_dec_openssl PUBLIC ${PROJECT_BINARY_DIR}/..)
target_include_directories(test_b64_enc_dec_openssl PUBLIC ${PROJECT_BINARY_DIR}/../build)

add_executable(test_dstnow test_dstnow.c)
target_link_libraries(test_dstnow mike ssl crypto curl)
target_link_directories(test_dstnow PUBLIC ${PROJECT_BINARY_DIR}/../build)
target_include_directories(test_dstnow PUBLIC ${PROJECT_BINARY_DIR}/..)
target_include_directories(test_dstnow PUBLIC ${PROJECT_BINARY_DIR}/../build)
