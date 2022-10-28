#ifndef MUTIL_H
#define MUTIL_H

#include <stdint.h>

uint64_t fibonacci(int n);

uint64_t sfibonacci(int reset);

// Return 1 or 0 based on whether the current timezone is currently in
// daylight savings time.
int dstnow();

#endif
