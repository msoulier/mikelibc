#include <time.h>

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#include "taia.h"
#include "leapsecs.h"
#else
#include "taia.h"
#include "leapsecs.h"
#endif

#ifdef MIKELIBC_THREADS
#include <pthread.h>
#endif

#include <string.h>
#include <errno.h>

#include "mlog.h"

#define TAIN_SIZE 12

/* The type of logger, defaulting to stdout. */
int loggertype = LOGGER_STDOUT;
/* The current log severity level. */
logseverity_t loggerseverity = MLOG_INFO;
/* Whether to show timestamps. */
int timestamptype = LOCNOZONE;
/* The FILE pointer of the logfile. */
FILE *logfile = NULL;
#ifdef MIKELIBC_THREADS
pthread_mutex_t logging_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void
taia2ext(char *timebuf, struct taia *tnow)
{
    char packed[TAIA_PACK];
    char temp[5];
    int i;

    timebuf[0] = '@';
    timebuf++;
    taia_pack(packed, tnow);
    for (i = 0; i < TAIN_SIZE; ++i)
    {
        snprintf(temp, 5, "%2.2x", (unsigned char)packed[i]);
        *timebuf++ = temp[0];
        *timebuf++ = temp[1];
    }
    *timebuf = '\0';
}

int
gettimestamp(int format, char *timebuf)
{
    struct taia tnow;
    if (format != FORMAT_TAI64N)
        return -1;
    taia_now(&tnow);
    taia2ext(timebuf, &tnow);
    return 1;
}

void setloggertime(logtime_t tstype)
{
    timestamptype = tstype;
}

int setloggertype(int type, char* path)
{
    FILE *lfile;

    loggertype = type;
    // FIXME: clean this up
    if (type == LOGGER_FILE)
    {
        if (path != NULL)
        {
            /* FIXME: We should have to specify appending or not. */
            if ((lfile = fopen(path, "w")) == NULL)
            {
                perror("logfile");
                loggertype = LOGGER_NONE;
                return -1;
            } else {
                logfile = lfile;
            }
        } else {
            fprintf(stderr, "Warning: Requested file logger, but path is NULL\n");
            return -1;
        }
    } else if (type == LOGGER_STDERR) {
        logfile = stderr;
    } else {
        logfile = stdout;
    }
    return 1;
}

void setloggersev(logseverity_t severity)
{
    loggerseverity = severity;
}

void vlogmsg(logseverity_t severity, const char *fmt, va_list argp)
{
    char timebuf[TIMEBUF];
    struct taia tnow;
    struct timeval tv;
    char *tf0 = "";
    char *tf1 = "%a %b %e %H:%M:%S";
    char *tf2 = "%a %b %e %H:%M:%S %Z (UTC%z) %Y";
    char *tf;

    switch (timestamptype) {
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

    if (timestamptype)
    {
        gettimeofday(&tv, NULL);
        if (timestamptype == UTC)
        {
            strftime(timebuf, TIMEBUF, tf, gmtime(&tv.tv_sec));
        }
        else if (timestamptype == TAI64N)
        {
            leapsecs_init();
            taia_now(&tnow);
            taia2ext(timebuf, &tnow);
        }
        else
        {
            strftime(timebuf, TIMEBUF, tf, localtime(&tv.tv_sec));
        }
    }

    if (severity < loggerseverity)
        goto CLEANUP_AND_RETURN;

    switch (loggertype) {
    case LOGGER_NONE:
        goto CLEANUP_AND_RETURN;
    case LOGGER_SYSLOG:
        fprintf(logfile, "Warning: syslog support not implemented.\n");
        /* fall through for now */
    case LOGGER_STDOUT:
    case LOGGER_STDERR:
    case LOGGER_FILE:
        // FIXME - use hires timer and provide milliseconds?
        if (timestamptype)
            fprintf(logfile, "%s ", timebuf);
        if (timestamptype == UTC)
            fprintf(logfile, "UTC ");
        switch (severity) {
        case MLOG_ALL:
            fprintf(logfile, "ALL: ");
            break;
        case MLOG_DEBUG:
            fprintf(logfile, "DEBUG: ");
            break;
        case MLOG_INFO:
            fprintf(logfile, "INFO: ");
            break;
        case MLOG_WARNING:
            fprintf(logfile, "WARNING: ");
            break;
        case MLOG_ERROR:
            fprintf(logfile, "ERROR: ");
            break;
        case MLOG_CRITICAL:
            fprintf(logfile, "CRITICAL: ");
            break;
        default:
            fprintf(logfile, "LOG: ");
            break;
        }
        vfprintf(logfile, fmt, argp);
        fprintf(logfile, "\n");
        break;
    default:
        fprintf(stderr, "Unsupported logger type: %d\n", loggertype);
    }

CLEANUP_AND_RETURN:
    return;
}

void logmsg(logseverity_t severity, const char *fmt, ...)
{
    va_list argp;
#ifdef MIKELIBC_THREADS
    pthread_mutex_lock(&logging_mutex);
#endif
    va_start(argp, fmt);
    vlogmsg(severity, fmt, argp);
    va_end(argp);
#ifdef MIKELIBC_THREADS
    pthread_mutex_unlock(&logging_mutex);
#endif
}
#ifdef __cplusplus
};
#endif
