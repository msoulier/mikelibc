#include "mikelib.h"

char mikelib_version_s[] = "1.0.5";
int mikelib_version_i[] = { 1, 0, 5 };

char *
get_mikelib_version_s(void)
{
    return mikelib_version_s;
}

int *
get_mikelib_version_i(void)
{
    return mikelib_version_i;
}
