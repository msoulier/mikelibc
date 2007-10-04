#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#include "logger.h"

/* The type of logger, defaulting to stdout. */
int loggertype = LOGGER_STDOUT;
/* The current log severity level. */
int loggerseverity = LOGSEV_INFO;
/* Whether to show timestamps. */
int timestamplevel = 1;
/* The FILE pointer of the logfile. */
FILE *logfile = NULL;

void setloggertime(int level)
{
    /* Right now, valid levels are 0 and 1. */
    if ((level != 0) && (level != 1) && (level != 2))
        return;
    timestamplevel = level;
}

int setloggertype(int type, char* path)
{
    FILE *lfile;

    loggertype = type;
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
            }
            else
            {
                logfile = lfile;
            }
        }
        else
        {
            fprintf(stderr, "Warning: Requested file logger, but path is NULL\n");
            return -1;
        }
    }
    else
    {
        logfile = stdout;
    }
    return 1;
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
    case LOGTIME_LOCNOZONE:
        tf = tf1;
        break;
    case LOGTIME_LOCWZONE:
        tf = tf2;
        break;
    default:
        /* LOGTIME_NONE */
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
    case LOGGER_NONE:
        return;
    case LOGGER_SYSLOG:
        fprintf(stderr, "Warning: syslog support not implemented.\n");
        /* fall through for now */
    case LOGGER_STDOUT:
    case LOGGER_FILE:
        // FIXME - use hires timer and provide milliseconds?
        if (timestamplevel)
            fprintf(logfile, "%s ", timebuf);
        switch (severity) {
        case LOGSEV_ALL:
            fprintf(logfile, "ALL: ");
            break;
        case LOGSEV_DEBUG:
            fprintf(logfile, "DEBUG: ");
            break;
        case LOGSEV_INFO:
            fprintf(logfile, "INFO: ");
            break;
        case LOGSEV_WARNING:
            fprintf(logfile, "WARNING: ");
            break;
        case LOGSEV_ERROR:
            fprintf(logfile, "ERROR: ");
            break;
        case LOGSEV_CRITICAL:
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
}

void logmsg(int severity, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vlogmsg(severity, fmt, argp);
    va_end(argp);
}
