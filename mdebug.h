#ifndef MDEBUG_H
#define MDEBUG_H

#include <stdarg.h>
#include <stdio.h>
#ifdef MIKELIBC_THREADS
#include <pthread.h>
extern pthread_mutex_t mdebug_mutex;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* A simple software assertion that does not terminate the executable.
 * 0 is good, 1 is bad.
 */
#define swerr(f) ((f) ? 0 : \
    fprintf(stderr, "libesdb SWERR: File -> %s, Line -> %d\n", __FILE__, __LINE__) > 0)

void
dbg_printf(const char *fname, const int lineno, const char *fmt, ...);

/*
 * A debug macro that compiles out if MDEBUG is not set, and compiles to an
 * fprintf out stderr otherwise.
 */
#ifdef MDEBUG
#define mdbgf(...) dbg_printf (__FILE__, __LINE__, __VA_ARGS__)
#else
#define mdbgf(...)
#endif

#ifdef __cplusplus
}
#endif

#endif
