#include <stdlib.h>

#include "mikelib.h"

int
main(void)
{
    int *ver;

    printf("Version of mikelib is %s\n", get_mikelib_version_s());

    printf("Testing version array:\n");
    ver = get_mikelib_version_i();
    printf("\tmajor    = %d\n", ver[0]);
    printf("\tminor    = %d\n", ver[1]);
    printf("\tsubminor = %d\n", ver[2]);

    exit(0);
}
