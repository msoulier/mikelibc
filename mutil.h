#ifndef MUTIL_H
#define MUTIL_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <openssl/evp.h>

#include "mbase64.h"

#ifdef __cplusplus
extern "C" {
#endif

uint64_t fibonacci(int n);

uint64_t sfibonacci(int reset);

// Return 1 or 0 based on whether the current timezone is currently in
// daylight savings time.
int dstnow();

/**
 * Source: Bart Trojanowski
 *
 * bidirectional popen() call
 *
 * @param rwepipe - int array of size three
 * @param exe - program to run
 * @param argv - argument list
 * @return pid or -1 on error
 *
 * The caller passes in an array of three integers (rwepipe), on successful
 * execution it can then write to element 0 (stdin of exe), and read from
 * element 1 (stdout) and 2 (stderr).
 */
int popenRWE(int *rwepipe, const char *exe, const char *const args[]);

/*
 * Taking an unsigned char of data, populate a string of base64-encoded
 * data. The resulting string is passed back. It is also heap allocated and
 * should be freed when done. Not thread-safe. Returns NULL on error.
 */
char *base64_encode_openssl(const unsigned char *data, size_t input_size, size_t *output_size);

/*
 * Taking a base64-encoded string, populate an unsigned char* of decoded data.
 * The resulting string is passed back. It is also heap allocated and should be
 * freed when done. Not thread-safe. Note that the output string could contain
 * embedded NULL characters, so trust the output_size, and not string-handling
 * functions. Returns NULL on error.
 */
unsigned char *base64_decode_openssl(const char *b64string, size_t input_size, size_t *output_size);

/*
 * Taking a secret key and an initialization vector, plus the plaintext to
 * encrypt and its size, encrypt the plaintext using the supplied cipher.
 * Return the encrypted string, or NULL on error.
 * The cipher_type can be any EVP_CIPHER for AES, like
 *  EVP_aes_256_cfb8()
 *  EVP_aes_128_cfb8()
 * See EVP_CIPHER(3ssl) for all cipher types.
 * If cipher_type is NULL, EVP_aes_256_cfb8() will be used.
 */
unsigned char *encrypt_ssl(unsigned char *key,
                           unsigned char *iv, 
                           const EVP_CIPHER *cipher_type,
                           unsigned char *plaintext,
                           int input_size);

/*
 * Taking a secret key and an initialization vector, plus the ciphertext to
 * decrypt and its size, decrypt the ciphertext using the supplied cipher.
 * Return the decrypted string, or NULL on error.
 * The cipher_type can be any EVP_CIPHER for AES, like
 *  EVP_aes_256_cfb8()
 *  EVP_aes_128_cfb8()
 * See EVP_CIPHER(3ssl) for all cipher types.
 * If cipher_type is NULL, EVP_aes_256_cfb8() will be used.
 */
unsigned char *decrypt_ssl(unsigned char *key,
                           unsigned char *iv, 
                           const EVP_CIPHER *cipher_type,
                           unsigned char *ciphertext,
                           int input_size);


/**
 * Compute a sha1 digest of the input. Return an unsigned char* with the
 * digest.  The function populates the digest using OPENSSL_malloc on it first,
 * so OPENSSL_free should be called on digest when done with it. Include the
 * openssl/evp.h header and link with -lssl.
 */
unsigned char *digest_sha1(unsigned char *in,
                           size_t in_length,
                           unsigned int *digest_len);

/**
 * Given the input string in, and the length of it in_length, dynamically
 * compute the hexidecimal of the characters in the input string, and return
 * them as a new string. The new string will be twice the length of the input
 * string, and should be freed by the caller when done with it.
 */
char *tohex(unsigned char *in,
                size_t in_length);

#ifdef __cplusplus
}
#endif

#endif
