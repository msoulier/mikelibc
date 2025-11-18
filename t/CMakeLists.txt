function(create_test NAME)
    add_executable(test_${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/test_${NAME}.c)
    target_link_libraries(test_${NAME} mike)
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
create_test(dns)
create_test(popenRWE)
create_test(mqueue)
create_test(tcp_client)
create_test(followlink)

add_test(NAME setup_environment
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/followlink_setup.sh)

set_tests_properties(setup_environment PROPERTIES
    FIXTURES_SETUP my_fixture
)
set_tests_properties(test_followlink PROPERTIES
    FIXTURES_REQUIRED my_fixture
)

enable_testing()
