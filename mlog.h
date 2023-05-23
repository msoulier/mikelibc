#ifndef MLOGGER_H
#define MLOGGER_H

#include <stdio.h>
#include <stdarg.h>

// This needs to go away in favour of a proper logger handler system.
#define LOGGER_NONE     -1
#define LOGGER_STDOUT    0
#define LOGGER_SYSLOG    1
#define LOGGER_FILE      2
#define LOGGER_STDERR    3

#define TIMEBUF 128

/* This function sets the type of the logger.
 * type is one of
 * - LOGGER_NONE
 * - LOGGER_STDOUT
 * - LOGGER_SYSLOG
 * - LOGGER_FILE
 * - LOGGER_STDERR
 * If the type is LOGGER_FILE, then the path argument is required, otherwise
 * it can be NULL.
 * On any errors, the logger type will be set to LOGGER_NONE, and -1 will be
 * returned.
 */
int setloggertype(int type, char *path);

// Log severity levels
typedef enum {
    MLOG_ALL         = 0,
    MLOG_DEBUG       = 10,
    MLOG_INFO        = 20,
    MLOG_WARNING     = 30,
    MLOG_ERROR       = 40,
    MLOG_CRITICAL    = 50
} logseverity_t;

/* This function sets the current severity of the logger. */
void setloggersev(logseverity_t severity);

/* This function sends a message to the logger, with a given priority. 
 * This function is thread-safe if built with the MIKELIBC_THREADS macro
 * defined. The others are not, call them from the main thread.
 */
void logmsg(logseverity_t severity, const char *fmt, ...);

// Log timestamp type.
typedef enum {
    NONE        = 0,
    LOCNOZONE   = 1,
    LOCWZONE    = 2,
    UTC         = 3
} logtime_t;

/* This function sets the level of the timestamps used by the logger.
 * NONE      - no timestamps
 * LOCNOZONE - standard timestamps in localtime without zone
 * LOCWZONE  - standard timestamps in localtime with timezone
 * UTC       - standard timestamps in UTC
 */
void setloggertime(logtime_t tstype);

#endif
