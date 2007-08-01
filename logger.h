#include <stdio.h>
#include <stdarg.h>

#define LOGGER_STDOUT    0
#define LOGGER_SYSLOG    1
#define LOGSEV_ALL       0
#define LOGSEV_DEBUG    10
#define LOGSEV_INFO     20
#define LOGSEV_WARNING  30
#define LOGSEV_ERROR    40
#define LOGSEV_CRITICAL 50

#define TIMEBUF 128

/* This function sets the type of the logger.
 * - LOGGER_STDOUT
 * - LOGGER_SYSLOG
 */
void setloggertype(int type);

/* This function sets the current severity of the logger. */
void setloggersev(int severity);

/* This function sends a message to the logger, with a given priority. 
 */
void logmsg(int severity, const char *fmt, ...);

/* This function sets the level of the timestamps used by the logger.
 * 0 - no timestamps
 * 1 - standard timestamps in localtime without zone
 * 2 - standard timestamps in localtime with timezone
 */
void setloggertime(int level);
