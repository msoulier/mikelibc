#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#ifdef MIKELIBC_THREADS
#include <sys/syscall.h>
#endif

#include "mdebug.h"

#ifdef MIKELIBC_THREADS
pthread_mutex_t mdebug_mutex;
#endif

#define TSTAMP_SIZE 1024

/* Ideas...
 * - Splitting a string by a char, default to whitespace.
 * - My own getopt for posix command-line args
 * - My own getline()
 */

void dbg_printf(const char *fname, const int lineno, const char *fmt, ...)
{
#ifdef MIKELIBC_THREADS
    pthread_mutex_lock(&mdebug_mutex);
#endif
    va_list args;
    va_start(args, fmt);
    time_t now = time(NULL);
    struct tm *nowtm = localtime(&now);
    char ts[TSTAMP_SIZE];
    strftime(ts, TSTAMP_SIZE, "%c", nowtm);
#ifdef MIKELIBC_THREADS
    pid_t tid;
    tid = syscall(SYS_gettid);
    //fprintf(stderr, "[%d][%d] [DEBUG]: ", getpid(), tid);
    fprintf(stderr, "%s [%d] [DEBUG] ", ts, tid);
#else
    //fprintf(stderr, "[%d] [DEBUG]: ", getpid());
    fprintf(stderr, "%s [DEBUG] ", ts);
#endif
    fprintf(stderr, "[%s:%d] ", fname, lineno);
    vfprintf(stderr, fmt, args);
    va_end(args);
#ifdef MIKELIBC_THREADS
    pthread_mutex_unlock(&mdebug_mutex);
#endif
}
