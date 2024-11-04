#include <stdio.h>
#include <assert.h>

#include "mutil.h"

int test_sfib(void) {
    uint64_t first = sfibonacci(1);
    uint64_t second = sfibonacci(0);
    uint64_t third = sfibonacci(0);
    uint64_t fourth = sfibonacci(0);
    uint64_t fifth = sfibonacci(0);
    assert( first  == 0 );
    assert( second == 1 );
    assert( third  == 1 );
    assert( fourth == 2 );
    assert( fifth  == 3 );
    return 0;
}

int test_fib(void) {
    assert( fibonacci(0) == 0 );
    assert( fibonacci(1) == 1 );
    assert( fibonacci(2) == 1 );
    assert( fibonacci(3) == 2 );
    assert( fibonacci(4) == 3 );
    return 0;
}

int
main(void) {
    int rv1 = test_sfib();
    int rv2 = test_fib();

    if ((rv1 == 0) && (rv2 == 0)) {
        return 0;
    } else {
        return 1;
    }
}
