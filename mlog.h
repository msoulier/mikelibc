#ifndef MLOGGER_H
#define MLOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TIMEBUF 128
#define MAX_LOG 4096

/**
 * A handle to a particular logger. Using an int for now.
 */
typedef int mlog_handle_t;

/**
 * The global handler used by this library. -1 is unset.
 */
extern mlog_handle_t g_handle;

/**
 * This needs to go away in favour of a proper logger handler system.
 */
typedef enum {
    MLOG_NONE   = -1,
    MLOG_STDOUT = 0,
    MLOG_SYSLOG = 1,
    MLOG_FILE   = 2,
    MLOG_STDERR = 3
} loggertype_t;

/**
 * Log severity levels
 */
typedef enum {
    MLOG_TRACE       = 0,
    MLOG_DEBUG       = 10,
    MLOG_INFO        = 20,
    MLOG_WARNING     = 30,
    MLOG_ERROR       = 40,
    MLOG_CRITICAL    = 50
} logseverity_t;

/** This function sets the current severity of the logger.
 */
void setloggersev(mlog_handle_t handle, logseverity_t severity);

/** Log timestamp type.
 */
typedef enum {
    NONE        = 0,
    LOCNOZONE   = 1,
    LOCWZONE    = 2,
    UTC         = 3
} logtime_t;

/**
 * A logger callback that the client of the library can use so that the default
 * logger is not used and the client's logger is used instead. The parameters are:
 *
 * @param[in] severity
 *      The log severity of the message.
 *
 * @param[in] message
 *      The message to be written to the log.
 *
 * @param cb[in]_ptr
 *      Extra logger data to be passed in, set only once when registering
 *      a logger. Can be NULL.
 */
typedef void (*mlog_cb_t)(logseverity_t severity,
                          const char *message,
                          void *cb_ptr);

/**
 * Encapsulation of a logger to permit multiple loggers.
 */
typedef struct mlog {
    loggertype_t    loggertype;
    logseverity_t   severity;
    logtime_t       logtime;
    mlog_handle_t   handle;
    char            *path;
    FILE            *logfile;
    mlog_cb_t       callback;
    void            *cb_data;
    struct mlog     *next;
} mlog_t;

/**
 * This function sets the type of the logger.
 * type is a loggertype_t.
 * If the type is MLOG_FILE, then the path argument is required, otherwise
 * it can be NULL.
 * On any errors, the logger type will be set to MLOG_NONE, and 0 will be
 * returned. Otherwise, 1 is returned.
 */
int setloggertype(mlog_handle_t handle, loggertype_t type, char *path);

/**
 * This function sets the level of the timestamps used by the logger.
 * NONE      - no timestamps
 * LOCNOZONE - standard timestamps in localtime without zone
 * LOCWZONE  - standard timestamps in localtime with timezone
 * UTC       - standard timestamps in UTC
 */
void setloggertime(mlog_handle_t handle, logtime_t tstype);

/**
 * This function sends a message to the logger, with a given priority. 
 * This function is thread-safe if built with the MIKELIBC_THREADS macro
 * defined. The others are not, call them from the main thread.
 */
void mlogf(mlog_handle_t handle, logseverity_t severity, const char *fmt, ...);

/** Convenience macros.
 */
#define mdebugf(...)    mlogf(g_handle, MLOG_DEBUG, __VA_ARGS__)
#define minfof(...)     mlogf(g_handle, MLOG_INFO, __VA_ARGS__)
#define mwarningf(...)  mlogf(g_handle, MLOG_WARNING, __VA_ARGS__)
#define merrorf(...)    mlogf(g_handle, MLOG_ERROR, __VA_ARGS__)
#define mcriticalf(...) mlogf(g_handle, MLOG_CRITICAL, __VA_ARGS__)

/**
 * Create a new logger and return the handle for it. Return -1 on error.
 */
mlog_handle_t
get_mlogger(loggertype_t loggertype, logseverity_t severity, logtime_t logtime);

/**
 * Shut down a logger. 1 on success, 0 on failure.
 */
int
shutdown_mlogger(mlog_handle_t handle);

/**
 * Shut down all loggers.
 */
void
shutdown_all_mloggers();

/**
 * Register your own logger with mikelibc.
 *
 * @param[in] mlog_handle_t
 *      The handle of the logger to set the callback to.
 *
 * @param[in] cb_fn_ptr
 *      A mlog_cb_t function type which is invoked when logging.
 *
 * @return[out] none
 */
void register_mlog_callback(mlog_handle_t handle, mlog_cb_t cb_fn_ptr, void *cb_ptr);

#ifdef __cplusplus
};
#endif

#endif
