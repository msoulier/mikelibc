#include <stdarg.h>
#include <stdio.h>

/* A simple software assertion that does not terminate the executable.
 * 0 is good, 1 is bad.
 */
#define swerr(f) ((f) ? 0 : \
    fprintf(stderr, "libesdb SWERR: File -> %s, Line -> %d\n", __FILE__, __LINE__) > 0)

void
dbg_printf(const char *fmt, ...);

/*
 * A debug macro that compiles out if DEBUG is not set, and compiles to an
 * fprintf out stderr otherwise.
 */
#ifdef DEBUG
#define debug(args) dbg_printf args
#else
#define debug(args)
#endif
