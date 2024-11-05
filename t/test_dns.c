#include <assert.h>
#include <stddef.h>

#include "mnet.h"

/*
 * Test the dns functions.
 */
int test_dns(void) {
    int rv = maddrlookup("google.com", "80", TCP);
    assert( rv == 0 );
    rv = maddrlookup("www.cbc.ca", "80", TCP);
    assert( rv == 0 );
    rv = maddrlookup("does.not.exist.foo", "22", TCP);
    assert( rv != 0 );
    rv = maddrlookup("amazon.com", NULL, TCP);
    assert( rv == 0 );

    return 0;
}

int
main(void) {
    return test_dns();
}
