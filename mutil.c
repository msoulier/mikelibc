#include <stdio.h>

#include "mutil.h"

long long int fibonacci(int n) {
    if (n < 1) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return fibonacci(n-1) + fibonacci(n-2);
    }
}

long long int sfibonacci(int initialize) {
    static long long int first = 0;
    static long long int second = 1;
    if (initialize) {
        first = 0;
        second = 1;
    }
    long long int out = first + second;
    if (first == 0) {
        out = 0;
    }
    first = second;
    second = out;
    return out;
}
