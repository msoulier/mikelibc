#ifndef MDEBUG_H
#define MDEBUG_H

#include <stdarg.h>
#include <stdio.h>
#ifdef MIKELIBC_THREADS
#include <pthread.h>
extern pthread_mutex_t mdebug_mutex;
#endif

/* A simple software assertion that does not terminate the executable.
 * 0 is good, 1 is bad.
 */
#define swerr(f) ((f) ? 0 : \
    fprintf(stderr, "libesdb SWERR: File -> %s, Line -> %d\n", __FILE__, __LINE__) > 0)

void
dbg_printf(const char *fmt, ...);

/*
 * A debug macro that compiles out if MDEBUG is not set, and compiles to an
 * fprintf out stderr otherwise.
 */
#ifdef MDEBUG
#define mdebugf(...) dbg_printf (__VA_ARGS__)
#else
#define mdebugf(...)
#endif

#endif
