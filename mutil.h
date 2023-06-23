#ifndef MUTIL_H
#define MUTIL_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

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
 * Taking a plaintext string, populate a crypttext, base64-encoded version.
 * The resulting string is passed back. It is also heap allocated and should
 * be freed when done. Not thread-safe.
 */
char *base64_encode(const char *plaintext, size_t input_size);

/*
 * Taking a crypttext string, populate a plaintext, base64-decoded version.
 * The resulting string is passed back. It is also heap allocated and should
 * be freed when done. Not thread-safe.
 */
char *base64_decode(const char *crypttext, size_t input_size);

unsigned char *encrypt_aes(unsigned char *key,
                           unsigned char *iv, 
                           unsigned char *plaintext,
                           int input_size);

unsigned char *decrypt_aes(unsigned char *key,
                           unsigned char *iv, 
                           unsigned char *ciphertext,
                           int input_size);


#ifdef __cplusplus
}
#endif

#endif
