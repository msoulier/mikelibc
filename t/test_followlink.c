#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "mutil.h"
#include "mdebug.h"

int test_followlink() {
    char buf[4096];
    // FIXME: create test symlinks for portability
    ssize_t nbytes = followlink("./link", buf, 4096);
    if (nbytes > 0) {
        mdbgf("followlink returned a buf of %s\n", buf);
    }
    assert( nbytes > 0 );
    assert( strncmp(buf, "./target", 4096) == 0 );

    return 0;
}

int
main(void) {
    return test_followlink();
}
