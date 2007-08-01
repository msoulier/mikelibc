#include <time.h>
#include <sys/time.h>

#include "logger.h"

/* The type of logger, defaulting to stdout. */
int loggertype = LOGGER_STDOUT;
/* The current log severity level. */
int loggerseverity = LOGSEV_INFO;
/* Whether to show timestamps. */
int timestamplevel = 1;

void setloggertime(int level)
{
    /* Right now, valid levels are 0 and 1. */
    if ((level != 0) && (level != 1) && (level != 2))
        return;
    timestamplevel = level;
}

void setloggertype(int type)
{
    loggertype = type;
}

void setloggersev(int severity)
{
    loggerseverity = severity;
}

void vlogmsg(int severity, const char *fmt, va_list argp)
{
    char timebuf[TIMEBUF];
    time_t result;
    struct timeval tv;
    char *tf0 = "";
    char *tf1 = "%a %b %e %H:%M:%S";
    char *tf2 = "%a %b %e %H:%M:%S %Z (UTC%z) %Y";
    char *tf;

    switch (timestamplevel) {
    case 1:
        tf = tf1;
        break;
    case 2:
        tf = tf2;
        break;
    default:
        tf = tf0;
        break;
    }

    result = time(NULL);
    gettimeofday(&tv, NULL);
    strftime(timebuf, TIMEBUF, tf, localtime(&tv.tv_sec));
    // "%a %b %e %H:%M:%S.%%li %Z (UTC%z) %Y",
    // sprintf(timebuf, timebuf, tv.tv_usec);

    if (severity < loggerseverity)
        return;

    switch (loggertype) {
    case LOGGER_SYSLOG:
        fprintf(stderr, "Warning: syslog support not implemented.\n");
        /* fall through for now */
    case LOGGER_STDOUT:
        // FIXME - use hires timer and provide milliseconds?
        if (timestamplevel)
            fprintf(stderr, "%s ", timebuf);
        switch (severity) {
        case LOGSEV_ALL:
            fprintf(stderr, "ALL: ");
            break;
        case LOGSEV_DEBUG:
            fprintf(stderr, "DEBUG: ");
            break;
        case LOGSEV_INFO:
            fprintf(stderr, "INFO: ");
            break;
        case LOGSEV_WARNING:
            fprintf(stderr, "WARNING: ");
            break;
        case LOGSEV_ERROR:
            fprintf(stderr, "ERROR: ");
            break;
        case LOGSEV_CRITICAL:
            fprintf(stderr, "CRITICAL: ");
            break;
        default:
            fprintf(stderr, "LOG: ");
            break;
        }
        vfprintf(stderr, fmt, argp);
        fprintf(stderr, "\n");
        break;
    default:
        fprintf(stderr, "Unsupported logger type: %d\n", loggertype);
    }
}

void logmsg(int severity, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vlogmsg(severity, fmt, argp);
    va_end(argp);
}
