#include <stdio.h>
#include <assert.h>

#include "mutil.h"

int test_encrypt_decrypt(void) {
    char *key = "e^SXXaI^W0dBoC688#GU";
    char *iv = "1234567";

    char *plaintexts[] = {
        "secret password", "another secret", "lorem ipsem",
        "river soft", "deal rational", "innovation sport"
        };
    for (int i = 0; i < 6; ++i) {
        printf("encrypting '%s'\n", plaintexts[i]);
        char *ciphertext = (char *)encrypt_ssl((unsigned char *)key,
                                               (unsigned char *)iv,
                                               EVP_aes_128_cfb8(),
                                               (unsigned char *)plaintexts[i],
                                               strlen(plaintexts[i]));
        assert( ciphertext != NULL );
        char *decrypted = (char *)decrypt_ssl((unsigned char *)key,
                                              (unsigned char *)iv,
                                              EVP_aes_128_cfb8(),
                                              (unsigned char *)ciphertext,
                                              strlen(ciphertext));
        assert( decrypted != NULL );
        printf("decrypted is '%s'\n", decrypted);

        assert( strcmp(plaintexts[i], decrypted) == 0 );

        free(ciphertext);
        free(decrypted);
    }
    return 0;
}

int
main(void) {
    return test_encrypt_decrypt();
}
