#include <string.h>
#include "mikelib.h"

char version_buffer[256];
int mikelib_version_i[] = {
    MIKELIBC_VERSION_MAJOR,
    MIKELIBC_VERSION_MINOR,
    MIKELIBC_VERSION_PATCH
};

char *
get_mikelib_version_s(void)
{
    sprintf(version_buffer,
            "%d.%d.%d",
            MIKELIBC_VERSION_MAJOR,
            MIKELIBC_VERSION_MINOR,
            MIKELIBC_VERSION_PATCH);
    return version_buffer;
}

int *
get_mikelib_version_i(void)
{
    return mikelib_version_i;
}
