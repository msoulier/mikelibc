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

#define FORMAT_TAI64N     1

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
    UTC         = 3,
    TAI64N      = 4
} logtime_t;

/* This function sets the level of the timestamps used by the logger.
 * NONE      - no timestamps
 * LOCNOZONE - standard timestamps in localtime without zone
 * LOCWZONE  - standard timestamps in localtime with timezone
 * UTC       - standard timestamps in UTC
 * TAI64N    - timestamps in tai64n external format
 *                     (http://cr.yp.to/libtai/tai64.html)
 */
void setloggertime(logtime_t tstype);
/* Note, the tai64n handling will only handle leapseconds if you put
 * a leapsecs.dat file for it at /etc/leapsecs.dat. See libtai for full
 * details (http://cr.yp.to/libtai.html).
 * Note that this library makes use of libtai, which is in the public domain.
 */


/* This function returns a timestamp for the current time, with the intention
 * of providing multiple formats based on input.
 *
 * The type argument is equivalent to the levels in setloggertime. Currently
 * the only supported type is FORMAT_TAI64N.
 *
 * The timebuf argument is a char* to a buffer to populate, that should be at
 * least TIMEBUF in size.
 */
int
gettimestamp(int format, char *timebuf);

#endif
