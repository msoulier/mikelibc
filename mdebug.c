#include "mdebug.h"

/* Ideas...
 * - Splitting a string by a char, default to whitespace.
 * - My own getopt for posix command-line args
 * - My own getline()
 */

void dbg_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "debug libinipy.c: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}
