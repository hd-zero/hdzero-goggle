#ifndef STD_LOG_H
#define STD_LOG_H

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>

#include <sys/uio.h>

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------
/* color */
#define LIGHT   "1"
#define DARK    "0"

#define FG      "3"
#define BG      "4"

#define BLACK   "0"
#define RED     "1"
#define GREEN   "2"
#define YELLOW  "3"
#define BLUE    "4"
#define PURPLE  "5"
#define CYAN    "6"
#define WRITE   "7"

#define FG_COLOR(color)    FG color
#define BG_COLOR(color)    BG color

#if COLOR_LOG
#define FMT_DEFAULT                 "\033[0m"
#define FMT_COLOR_FG(light, color)  "\033[" light ";" FG_COLOR(color) "m"
#define FMT_COLOR_BG(fg, color)     "\033[" BG_COLOR(color) ";" FG_COLOR(fg) "m"
#else
#define FMT_DEFAULT
#define FMT_COLOR_FG(light, color)
#define FMT_COLOR_BG(fg, color)
#endif

/* position */
#if FORMAT_LOG
#define XPOSTO(x)   "\033[" #x "D\033[" #x "C"
#else
#define XPOSTO(x)
#endif

#define PRINTF_LOG_ERROR   FMT_COLOR_BG(BLACK, RED) " E " FMT_DEFAULT
#define PRINTF_LOG_WARN    FMT_COLOR_BG(BLACK, YELLOW) " W " FMT_DEFAULT
#define PRINTF_LOG_DEBUG   FMT_COLOR_BG(BLACK, BLUE) " D " FMT_DEFAULT
#define PRINTF_LOG_INFO    FMT_COLOR_BG(BLACK, GREEN) " I " FMT_DEFAULT
#define PRINTF_LOG_VERBOSE FMT_COLOR_BG(BLACK, WRITE) " V " FMT_DEFAULT

// ---------------------------------------------------------------------

/*
 * Normally we strip ALOGV (VERBOSE messages) from release builds.
 * You can modify this (for example with "#define LOG_NDEBUG 0"
 * at the top of your source file) to change that behavior.
 */
#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG 1
#else
#define LOG_NDEBUG 0
#endif
#endif

/*
 * This is the local tag used for the following simplified
 * logging macros.  You can change this preprocessor definition
 * before using the other macros to change the tag.
 */
#ifndef LOG_TAG
#define LOG_TAG "Unknown"
#endif


// ---------------------------------------------------------------------

/*
 * Simplified macro to send a verbose log message using the current LOG_TAG.
 */
#ifndef ALOGV
#if LOG_NDEBUG
#define ALOGV(...)   ((void)0)
#else
#define ALOGV(...) ALOG(LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#endif
#endif

/*
 * Simplified macro to send a debug log message using the current LOG_TAG.
 */
#ifndef ALOGD
#define ALOGD(...) ALOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif

/*
 * Simplified macro to send an info log message using the current LOG_TAG.
 */
#ifndef ALOGI
#define ALOGI(...) ALOG(LOG_INFO, LOG_TAG, __VA_ARGS__)
#endif

/*
 * Simplified macro to send a warning log message using the current LOG_TAG.
 */
#ifndef ALOGW
#define ALOGW(...) ALOG(LOG_WARN, LOG_TAG, __VA_ARGS__)
#endif
/*
 * Simplified macro to send an error log message using the current LOG_TAG.
 */
#ifndef ALOGE
#define ALOGE(...) ALOG(LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

// ---------------------------------------------------------------------
/*
 * Basic log message macro.
 *
 * Example:
 *  ALOG(LOG_WARN, NULL, "Failed with error %d", errno);
 *
 * The second argument may be NULL or "" to indicate the "global" tag.
 */
#ifndef ALOG
#define ALOG(priority, tag, ...) \
do { \
    LOG_PRI(PRINTF_##priority, tag); \
    printf(__VA_ARGS__); \
    printf("\n"); \
} while(0)
#endif

/*
 * Log macro that allows you to specify a number for the priority.
 */
#ifndef LOG_PRI
#define LOG_PRI(priority, tag) \
    printf("\t%s %s ", tag, \
            XPOSTO(50) priority)
#endif

#ifdef __cplusplus
}
#endif

#endif // STD_LOG_H
