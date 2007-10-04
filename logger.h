#include <stdio.h>
#include <stdarg.h>

#define LOGGER_NONE     -1
#define LOGGER_STDOUT    0
#define LOGGER_SYSLOG    1
#define LOGGER_FILE      2

#define LOGSEV_ALL       0
#define LOGSEV_DEBUG    10
#define LOGSEV_INFO     20
#define LOGSEV_WARNING  30
#define LOGSEV_ERROR    40
#define LOGSEV_CRITICAL 50

#define LOGTIME_NONE      0
#define LOGTIME_LOCNOZONE 1
#define LOGTIME_LOCWZONE  2

#define TIMEBUF 128

/* This function sets the type of the logger.
 * type is one of
 * - LOGGER_STDOUT
 * - LOGGER_SYSLOG
 * - LOGGER_FILE
 * If the type is LOGGER_FILE, then the path argument is required, otherwise
 * it can be NULL.
 * On any errors, the logger type will be set to LOGGER_NONE, and -1 will be
 * returned.
 */
int setloggertype(int type, char *path);

/* This function sets the current severity of the logger. */
void setloggersev(int severity);

/* This function sends a message to the logger, with a given priority. 
 */
void logmsg(int severity, const char *fmt, ...);

/* This function sets the level of the timestamps used by the logger.
 * LOGTIME_NONE      - no timestamps
 * LOGTIME_LOCNOZONE - standard timestamps in localtime without zone
 * LOGTIME_LOCWZONE  - standard timestamps in localtime with timezone
 */
void setloggertime(int level);
