#include <time.h>

#ifdef LINUX
#include <sys/time.h>
#elif WIN32
#include <windows.h>
#endif

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
#ifdef LINUX
    struct timeval tv;
    char *tf0 = "";
    char *tf1 = "%a %b %e %H:%M:%S";
    char *tf2 = "%a %b %e %H:%M:%S %Z (UTC%z) %Y";
    char *tf;
#elif WIN32
	SYSTEMTIME systemtime;
    char *day_of_the_week[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    char *month_name[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };
#endif

#ifdef LINUX
    switch (timestamplevel) {
    case LOGTIME_LOCNOZONE:
    case LOGTIME_UTC:
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
#endif

    if (timestamplevel)
    {
#ifdef LINUX
        gettimeofday(&tv, NULL);
        if (timestamplevel == LOGTIME_UTC)
        {
            strftime(timebuf, TIMEBUF, tf, gmtime(&tv.tv_sec));
        }
        else
        {
            strftime(timebuf, TIMEBUF, tf, localtime(&tv.tv_sec));
        }
#elif WIN32
        // Lets try to get basic timestamps working on windows before using anything fancy.
        if (timestamplevel == LOGTIME_UTC)
        {
            GetSystemTime(&systemtime);
            sprintf(timebuf, "%s %s %d %02d:%02d:%02d", 
                day_of_the_week[systemtime.wDayOfWeek],
                month_name[systemtime.wMonth-1],
                systemtime.wDay, 
                systemtime.wHour,
                systemtime.wMinute,
                systemtime.wSecond);
        }
        else
        {
            fprintf(logfile, "Warning: Localtime not yet implemented on win32.\n");
            timebuf[0] = '\0';
        }
#endif
    }

    if (severity < loggerseverity)
        return;

    switch (loggertype) {
    case LOGGER_NONE:
        return;
    case LOGGER_SYSLOG:
        fprintf(logfile, "Warning: syslog support not implemented.\n");
        /* fall through for now */
    case LOGGER_STDOUT:
    case LOGGER_FILE:
        // FIXME - use hires timer and provide milliseconds?
        if (timestamplevel)
            fprintf(logfile, "%s ", timebuf);
        if (timestamplevel == LOGTIME_UTC)
            fprintf(logfile, "UTC ");
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
