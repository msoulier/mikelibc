#include <stdio.h>
#include <assert.h>

#include "mutil.h"

int test_dstnow(void) {
    // Sask doesn't observe DST
    setenv("TZ", "America/Regina", 1);
    tzset();
    printf("dstnow for America/Regina is %d\n", dstnow());
    assert( dstnow() == 0 );
    // Difficult to reliably test this in other zones without
    // changing the system clock.
    return 0;
}

int
main(void) {
    return test_dstnow();
}
