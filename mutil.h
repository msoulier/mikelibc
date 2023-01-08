#ifndef MUTIL_H
#define MUTIL_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

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

#endif
