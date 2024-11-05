#include <stddef.h>
#include <assert.h>
#include <stdio.h>

#include "mutil.h"
#include "mdebug.h"

/*
 * Test popenRWE
 */
int test_popenRWE(void) {
    int fds[3] = { 0, 0, 0 };
    const char * const args[2] = { "/bin", "/usr/bin" };
    int rv = popenRWE(fds, "/bin/ls", args);
    assert( rv > 0 );
    char buffer[1024];
    FILE *out = fdopen(fds[1], "r");
    assert( out != NULL );
    for (;;) {
        if (fgets(buffer, 1024, out) == NULL) {
            break;
        } else {
            mdbgf(buffer);
        }
    }

    return 0;
}

int
main(void) {
    return test_popenRWE();
}
