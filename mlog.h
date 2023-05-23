#ifndef MLOGGER_H
#define MLOGGER_H

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TIMEBUF 128

/*
 * A handle to a particular logger. Using an int for now.
 */
typedef int mlog_handle_t;

/*
 * The global handler used by this library. -1 is unset.
 */
extern mlog_handle_t g_handle;

// This needs to go away in favour of a proper logger handler system.
typedef enum {
    MLOG_NONE   = -1,
    MLOG_STDOUT = 0,
    MLOG_SYSLOG = 1,
    MLOG_FILE   = 2,
    MLOG_STDERR = 3
} loggertype_t;

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
void setloggersev(mlog_handle_t handle, logseverity_t severity);

// Log timestamp type.
typedef enum {
    NONE        = 0,
    LOCNOZONE   = 1,
    LOCWZONE    = 2,
    UTC         = 3
} logtime_t;

/*
 * Encapsulation of a logger to permit multiple loggers.
 */
typedef struct mlog {
    loggertype_t    loggertype;
    logseverity_t   severity;
    logtime_t       logtime;
    mlog_handle_t   handle;
    char            *path;
    FILE            *logfile;
    struct mlog     *next;
} mlog_t;

/* This function sets the type of the logger.
 * type is a loggertype_t.
 * If the type is MLOG_FILE, then the path argument is required, otherwise
 * it can be NULL.
 * On any errors, the logger type will be set to MLOG_NONE, and 0 will be
 * returned. Otherwise, 1 is returned.
 */
int setloggertype(mlog_handle_t handle, loggertype_t type, char *path);

/* This function sets the level of the timestamps used by the logger.
 * NONE      - no timestamps
 * LOCNOZONE - standard timestamps in localtime without zone
 * LOCWZONE  - standard timestamps in localtime with timezone
 * UTC       - standard timestamps in UTC
 */
void setloggertime(mlog_handle_t handle, logtime_t tstype);

/* This function sends a message to the logger, with a given priority. 
 * This function is thread-safe if built with the MIKELIBC_THREADS macro
 * defined. The others are not, call them from the main thread.
 */
void mlogf(mlog_handle_t handle, logseverity_t severity, const char *fmt, ...);

// Convenience macros.
#define mdebugf(...)    mlogf(g_handle, MLOG_DEBUG, __VA_ARGS__)
#define minfof(...)     mlogf(g_handle, MLOG_INFO, __VA_ARGS__)
#define mwarningf(...)  mlogf(g_handle, MLOG_WARNING, __VA_ARGS__)
#define merrorf(...)    mlogf(g_handle, MLOG_ERROR, __VA_ARGS__)
#define mcriticalf(...) mlogf(g_handle, MLOG_CRITICAL, __VA_ARGS__)

/*
 * Create a new logger and return the handle for it. Return -1 on error.
 */
mlog_handle_t
get_mlogger(loggertype_t loggertype, logseverity_t severity, logtime_t logtime);

/*
 * Shut down a logger. 1 on success, 0 on failure.
 */
int
shutdown_mlogger(mlog_handle_t handle);

/*
 * Shut down all loggers.
 */
void
shutdown_all_mloggers();

#ifdef __cplusplus
};
#endif

#endif
