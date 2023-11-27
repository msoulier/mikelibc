#ifndef MIKELIB_H
#define MIKELIB_H

#include "mdebug.h"
#include "mlog.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Return a stack allocated buffer containing a version string.
 * This is currently only useful for debugging.
 */
char *
get_mikelib_version_s(void);

/*
 * Return a stack allocated array of three integers containing a version,
 * major, minor, subminor. Useful for comparison.
 */
int *
get_mikelib_version_i(void);

#ifdef __cplusplus
}
#endif

#endif
