
#ifndef __IPCLINUX_LOG_H__
#define __IPCLINUX_LOG_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define OPTION_LOG_STYLE_PRINTF     0
#define OPTION_LOG_STYLE_GLOG       1

#ifdef USE_STD_LOG
#define CONFIG_LOG_STYLE OPTION_LOG_STYLE_PRINTF
#else
#define CONFIG_LOG_STYLE OPTION_LOG_STYLE_GLOG
#endif

#define CONFIG_LOG_LEVEL    OPTION_LOG_LEVEL_WARN
// option for debug level.
#define OPTION_LOG_LEVEL_CLOSE      0
#define OPTION_LOG_LEVEL_ERROR      1
#define OPTION_LOG_LEVEL_WARN       2
#define OPTION_LOG_LEVEL_DEBUG      3
#define OPTION_LOG_LEVEL_VERBOSE    4

#define COLOR_LOG                   1
#define FORMAT_LOG                  1

#ifndef CONFIG_LOG_LEVEL
  #ifdef LOG_NDEBUG
    #if(LOG_NDEBUG == 0)
      #define CONFIG_LOG_LEVEL    OPTION_LOG_LEVEL_VERBOSE
    #endif
  #endif
#endif

#ifndef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL    OPTION_LOG_LEVEL_DEBUG
#endif

#ifndef LOG_TAG
#define LOG_TAG ""
#endif

#define StripFileName(s) (strrchr(s, '/')?(strrchr(s, '/')+1):s)

#if (CONFIG_LOG_STYLE == OPTION_LOG_STYLE_GLOG)
    #include <log/log.h>

    #define PLAT_LOG_LEVEL_INFO   _GLOG_INFO
    #define PLAT_LOG_LEVEL_WARN   _GLOG_WARN
    #define PLAT_LOG_LEVEL_ERROR  _GLOG_ERROR
    #define PLAT_LOG_LEVEL_FATAL  _GLOG_FATAL

#if CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_CLOSE

#define aloge(fmt, arg...)
#define alogw(fmt, arg...)
#define alogd(fmt, arg...)
#define alogv(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_ERROR

#define aloge(fmt, arg...) GLOG_PRINT(PLAT_LOG_LEVEL_ERROR, fmt, ##arg)
#define alogw(fmt, arg...)
#define alogd(fmt, arg...)
#define alogv(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_WARN

#define aloge(fmt, arg...) GLOG_PRINT(PLAT_LOG_LEVEL_ERROR, fmt, ##arg)
#define alogw(fmt, arg...) GLOG_PRINT(PLAT_LOG_LEVEL_WARN, fmt, ##arg)
#define alogd(fmt, arg...)
#define alogv(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_DEBUG

#define aloge(fmt, arg...) GLOG_PRINT(PLAT_LOG_LEVEL_ERROR, fmt, ##arg)
#define alogw(fmt, arg...) GLOG_PRINT(PLAT_LOG_LEVEL_WARN, fmt, ##arg)
#define alogd(fmt, arg...) GLOG_PRINT(PLAT_LOG_LEVEL_INFO, fmt, ##arg)
#define alogv(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_VERBOSE

#define aloge(fmt, arg...) GLOG_PRINT(PLAT_LOG_LEVEL_ERROR, fmt, ##arg)
#define alogw(fmt, arg...) GLOG_PRINT(PLAT_LOG_LEVEL_WARN, fmt, ##arg)
#define alogd(fmt, arg...) GLOG_PRINT(PLAT_LOG_LEVEL_INFO, fmt, ##arg)
#define alogv(fmt, arg...) GLOG_PRINT(PLAT_LOG_LEVEL_INFO, fmt, ##arg)

#else
    #error "invalid configuration of debug level."
#endif

#elif (CONFIG_LOG_STYLE == OPTION_LOG_STYLE_PRINTF)
    #include <stdio.h>
    #include <string.h>
    #include <log/std_log.h>

    #define PLAT_LOG_LEVEL_ERROR     FMT_COLOR_BG(BLACK, RED) " E " FMT_DEFAULT
    #define PLAT_LOG_LEVEL_WARN      FMT_COLOR_BG(BLACK, YELLOW) " W " FMT_DEFAULT
    #define PLAT_LOG_LEVEL_DEBUG     FMT_COLOR_BG(BLACK, BLUE) " D " FMT_DEFAULT
    #define PLAT_LOG_LEVEL_VERBOSE   FMT_COLOR_BG(BLACK, WRITE) " V " FMT_DEFAULT

    #define PLATLOG(level, fmt, arg...) \
    do { \
        printf("\t%s %s ", StripFileName(__FILE__), \
                XPOSTO(50) level); \
        printf(fmt "\n", __FUNCTION__, __LINE__, ##arg); \
    } while(0)

#if CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_CLOSE

#define aloge(fmt, arg...)
#define alogw(fmt, arg...)
#define alogd(fmt, arg...)
#define alogv(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_ERROR

#define aloge(fmt, arg...) PLATLOG(PLAT_LOG_LEVEL_ERROR, FMT_COLOR_FG(LIGHT, RED) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, ##arg)
#define alogw(fmt, arg...)
#define alogd(fmt, arg...)
#define alogv(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_WARN

#define aloge(fmt, arg...) PLATLOG(PLAT_LOG_LEVEL_ERROR, FMT_COLOR_FG(LIGHT, RED) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, ##arg)
#define alogw(fmt, arg...) PLATLOG(PLAT_LOG_LEVEL_WARN, FMT_COLOR_FG(LIGHT, YELLOW) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, ##arg)
#define alogd(fmt, arg...)
#define alogv(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_DEBUG

#define aloge(fmt, arg...) PLATLOG(PLAT_LOG_LEVEL_ERROR, FMT_COLOR_FG(LIGHT, RED) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, ##arg)
#define alogw(fmt, arg...) PLATLOG(PLAT_LOG_LEVEL_WARN, FMT_COLOR_FG(LIGHT, YELLOW) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, ##arg)
#define alogd(fmt, arg...) PLATLOG(PLAT_LOG_LEVEL_DEBUG, FMT_COLOR_FG(LIGHT, BLUE) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, ##arg)
#define alogv(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_VERBOSE

#define aloge(fmt, arg...) PLATLOG(PLAT_LOG_LEVEL_ERROR, FMT_COLOR_FG(LIGHT, RED) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, ##arg)
#define alogw(fmt, arg...) PLATLOG(PLAT_LOG_LEVEL_WARN, FMT_COLOR_FG(LIGHT, YELLOW) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, ##arg)
#define alogd(fmt, arg...) PLATLOG(PLAT_LOG_LEVEL_DEBUG, FMT_COLOR_FG(LIGHT, BLUE) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, ##arg)
#define alogv(fmt, arg...) PLATLOG(PLAT_LOG_LEVEL_VERBOSE, FMT_COLOR_FG(LIGHT, WRITE) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, ##arg)

#else
    #error "invalid configuration of debug level."
#endif

#else
    #error "invalid configuration of os."
#endif

#ifdef __cplusplus
}
#endif

#endif

