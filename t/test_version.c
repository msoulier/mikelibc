#include <stdio.h>

#include "mikelib.h"

int
main(void) {
    printf("Current version of mikelibc is %s\n",
        get_mikelib_version_s());

    int *ver_pieces = get_mikelib_version_i();
    printf("Version in pieces: %d:%d:%d\n",
        ver_pieces[0], ver_pieces[1], ver_pieces[2]);

    return 0;
}
