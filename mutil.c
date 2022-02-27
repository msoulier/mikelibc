#include <stdio.h>

#include "mutil.h"
#include "mdebug.h"

uint64_t fibonacci(int n) {
    mdebugf("fibonacci: n = %d\n", n);
    if (n < 1) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return fibonacci(n-1) + fibonacci(n-2);
    }
}

uint64_t sfibonacci(int reset) {
    static uint64_t first = 0;
    static uint64_t second = 1;
    static uint64_t count = 0;
    if (reset) {
        first = 0;
        second = 1;
        count = 0;
    }
    count++;
    if (count == 1) {
        return 0;
    } else if (count == 2) {
        return 1;
    } else {
        uint64_t out = first + second;
        first = second;
        second = out;
        return out;
    }
}
