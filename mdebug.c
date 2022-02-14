#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>

#ifdef PTHREADS
#include <sys/syscall.h>
#endif

#include "mdebug.h"

/* Ideas...
 * - Splitting a string by a char, default to whitespace.
 * - My own getopt for posix command-line args
 * - My own getline()
 */

void dbg_printf(const char *fmt, ...)
{
#ifdef PTHREADS
    pthread_mutex_lock(&mdebug_mutex);
#endif
    va_list args;
    va_start(args, fmt);
#ifdef PTHREADS
    pid_t tid;
    tid = syscall(SYS_gettid);
    fprintf(stderr, "[%d][%d] mdebug: ", getpid(), tid);
#else
    fprintf(stderr, "[%d] mdebug: ", getpid());
#endif
    vfprintf(stderr, fmt, args);
    va_end(args);
#ifdef PTHREADS
    pthread_mutex_unlock(&mdebug_mutex);
#endif
}
