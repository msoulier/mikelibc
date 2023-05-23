#include <time.h>

#include <sys/time.h>

#ifdef MIKELIBC_THREADS
#include <pthread.h>
#endif

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

#include "mlog.h"
#include "madt.h"

#ifdef MIKELIBC_THREADS
pthread_mutex_t logging_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

mlog_handle_t g_handle = -1;

void vmlogf(mlog_t *logger, logseverity_t severity, const char *fmt, va_list argp) {
    assert( logger != NULL );
    char timebuf[TIMEBUF];
    struct timeval tv;
    const char *tf0 = "";
    const char *tf1 = "%a %b %e %H:%M:%S";
    const char *tf2 = "%a %b %e %H:%M:%S %Z (UTC%z) %Y";
    const char *tf;

    switch (logger->logtime) {
        case LOCNOZONE:
        case UTC:
            tf = tf1;
            break;
        case LOCWZONE:
            tf = tf2;
            break;
        default:
            /* NONE */
            tf = tf0;
            break;
    }

    if (logger->logtime)
    {
        gettimeofday(&tv, NULL);
        if (logger->logtime == UTC) {
            strftime(timebuf, TIMEBUF, tf, gmtime(&tv.tv_sec));
        } else {
            strftime(timebuf, TIMEBUF, tf, localtime(&tv.tv_sec));
        }
    }

    if (severity < logger->severity) {
        goto CLEANUP_AND_RETURN;
    }

    switch (logger->loggertype) {
        case MLOG_NONE:
            goto CLEANUP_AND_RETURN;
        case MLOG_SYSLOG:
            fprintf(stderr, "Warning: syslog support not implemented.\n");
            /* fall through for now */
        case MLOG_STDOUT:
        case MLOG_STDERR:
        case MLOG_FILE:
            // FIXME - use hires timer and provide milliseconds?
            assert( logger->logfile != NULL );
            if (logger->logtime)
                fprintf(logger->logfile, "%s ", timebuf);
            if (logger->logtime == UTC)
                fprintf(logger->logfile, "UTC ");
            switch (severity) {
                case MLOG_ALL:
                    fprintf(logger->logfile, "ALL: ");
                    break;
                case MLOG_DEBUG:
                    fprintf(logger->logfile, "DEBUG: ");
                    break;
                case MLOG_INFO:
                    fprintf(logger->logfile, "INFO: ");
                    break;
                case MLOG_WARNING:
                    fprintf(logger->logfile, "WARNING: ");
                    break;
                case MLOG_ERROR:
                    fprintf(logger->logfile, "ERROR: ");
                    break;
                case MLOG_CRITICAL:
                    fprintf(logger->logfile, "CRITICAL: ");
                    break;
                default:
                    fprintf(logger->logfile, "LOG: ");
                    break;
            }
            vfprintf(logger->logfile, fmt, argp);
            fprintf(logger->logfile, "\n");
            break;
        default:
            fprintf(stderr, "Unsupported logger type: %d\n", logger->loggertype);
    }

CLEANUP_AND_RETURN:
    return;
}

/*
 * The linked-list of loggers.
 */
mlog_t *mloggers = NULL;

/*
 * The global counter for logger handlers.
 */
#ifdef MIKELIBC_THREADS
volatile mlog_handle_t next_handle = 0;
#else
mlog_handle_t next_handle = 0;
#endif

/*
 * Create a new logger and return the handle for it.
 */
mlog_handle_t
get_mlogger(loggertype_t loggertype, logseverity_t severity, logtime_t logtime) {
    mlog_t *new_logger = (mlog_t *)malloc(sizeof(mlog_t));
    mlog_t *current = mloggers;
    if (new_logger == NULL) {
        fprintf(stderr, "FATAL: malloc returned NULL\n");
        return -1;
    }

#ifdef MIKELIBC_THREADS
    pthread_mutex_lock(&logging_mutex);
#endif

    mlog_handle_t handle = next_handle;

    new_logger->loggertype = loggertype;
    new_logger->severity = severity;
    new_logger->logtime = logtime;
    new_logger->handle = handle;
    new_logger->path = NULL;
    new_logger->next = NULL;
    new_logger->logfile = NULL;

    next_handle++;

    mlinked_list_add(mloggers, new_logger, current);

    // FIXME: terrible workaround
    setloggertype(handle, loggertype, NULL);

#ifdef MIKELIBC_THREADS
    pthread_mutex_unlock(&logging_mutex);
#endif

    return handle;
}

int
mlogger_check(mlog_handle_t handle, mlog_t *current) {
    if (current->handle == handle) {
        return 1;
    } else {
        return 0;
    }
}

int
shutdown_mlogger(mlog_handle_t handle) {
    mlog_t *current = NULL;
    mlog_t *previous = NULL;
    mlog_t *freenode = NULL;

#ifdef MIKELIBC_THREADS
    pthread_mutex_lock(&logging_mutex);
#endif

    mlinked_list_remove(mloggers, current, previous, freenode, mlogger_check, handle);

#ifdef MIKELIBC_THREADS
    pthread_mutex_unlock(&logging_mutex);
#endif

    if (freenode == NULL) {
        return 0;
    } else {
        free(freenode);
        return 1;
    }
}

void
shutdown_all_mloggers() {
    mlog_t *current = NULL;
    mlog_t *previous = NULL;

#ifdef MIKELIBC_THREADS
    pthread_mutex_lock(&logging_mutex);
#endif

    current = previous = mloggers;

    while (current != NULL) {
        current = current->next;
        free(previous);
        previous = current;
    }

    mloggers = NULL;

#ifdef MIKELIBC_THREADS
    pthread_mutex_unlock(&logging_mutex);
#endif
}

mlog_t*
find_mlogger(mlog_handle_t handle) {
    mlog_t *current = NULL;
    mlinked_list_find(mloggers, current, mlogger_check, handle);
    return current;
}

void mlogf(mlog_handle_t handle, logseverity_t severity, const char *fmt, ...) {
    if (handle < 0) {
        // Unset
        return;
    }
    va_list argp;
#ifdef MIKELIBC_THREADS
    pthread_mutex_lock(&logging_mutex);
#endif
    mlog_t *logger = find_mlogger(handle);
    if (logger != NULL) {
        va_start(argp, fmt);
        vmlogf(logger, severity, fmt, argp);
        va_end(argp);
    } else {
        fprintf(stderr, "Bad logging handle: %d\n", handle);
    }
#ifdef MIKELIBC_THREADS
    pthread_mutex_unlock(&logging_mutex);
#endif
}

void setloggertime(mlog_handle_t handle, logtime_t tstype) {
    mlog_t *logger = find_mlogger(handle);
    if (logger != NULL) {
        logger->logtime = tstype;
    } else {
        fprintf(stderr, "Invalid logger handle: %d\n", handle);
    }
}

int setloggertype(mlog_handle_t handle, loggertype_t type, char* path) {
    mlog_t *logger = find_mlogger(handle);
    if (logger != NULL) {
        logger->loggertype = type;

        if (type == MLOG_FILE)
        {
            if (path != NULL)
            {
                /* FIXME: We should have to specify appending or not. */
                if ((logger->logfile = fopen(path, "w")) == NULL)
                {
                    perror("logfile");
                    logger->loggertype = MLOG_NONE;
                    return 0;
                }
            } else {
                fprintf(stderr, "Warning: Requested file logger, but path is NULL\n");
                return 0;
            }
        } else if (type == MLOG_STDERR) {
            logger->logfile = stderr;
        } else {
            logger->logfile = stdout;
        }
    } else {
        fprintf(stderr, "Invalid logger handle %d\n", handle);
        return 0;
    }
    return 1;
}

void setloggersev(mlog_handle_t handle, logseverity_t severity) {
    mlog_t *logger = find_mlogger(handle);
    if (logger != NULL) {
        logger->severity = severity;
    } else {
        fprintf(stderr, "Invalid logger handle %d\n", handle);
    }
}
