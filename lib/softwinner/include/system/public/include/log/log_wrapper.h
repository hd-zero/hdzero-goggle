#ifndef LOG_WRAPPER_H_
#define LOG_WRAPPER_H_

#define COLOR_LOG                   1
#define FORMAT_LOG                  1

#ifdef USE_STD_LOG
#include "std_log.h"
#else
#include "log.h"
#endif

#ifndef NDEBUG
#define DB_ERROR
#define DB_WARN
#define DB_INFO
#define DB_DEBUG
#define DB_MSG
#else
#define DB_ERROR
#define DB_WARN
#endif

#ifdef DB_ERROR
#define LOGE(fmt, arg...) \
    do { \
        ALOGE(FMT_COLOR_FG(LIGHT, RED) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, \
                __FUNCTION__, __LINE__, ##arg); \
    } while(0)
#else
#define LOGE(fmt, arg...)
#endif

#ifdef DB_WARN
#define LOGW(fmt, arg...) \
    do { \
        ALOGW(FMT_COLOR_FG(LIGHT, YELLOW) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, \
                __FUNCTION__, __LINE__, ##arg); \
    } while(0)
#else
#define LOGW(fmt, arg...)
#endif

#ifdef DB_INFO
#define LOGI(fmt, arg...) \
    do { \
        ALOGI(FMT_COLOR_FG(LIGHT, GREEN) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, \
                __FUNCTION__, __LINE__, ##arg); \
    } while(0)
#else
#define LOGI(fmt, arg...)
#endif

#ifdef DB_DEBUG
#define LOGD(fmt, arg...) \
    do { \
        ALOGD(FMT_COLOR_FG(LIGHT, BLUE) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, \
                __FUNCTION__, __LINE__, ##arg); \
    } while(0)
#else
#define LOGD(fmt, arg...)
#endif

#ifdef DB_MSG
#define LOGV(fmt, arg...) \
    do { \
        ALOGV(FMT_COLOR_FG(LIGHT, WRITE) "<%s:%d> " XPOSTO(90) FMT_DEFAULT fmt, \
                __FUNCTION__, __LINE__, ##arg); \
    } while(0)
#else
#define LOGV(fmt, arg...)
#endif

#endif

