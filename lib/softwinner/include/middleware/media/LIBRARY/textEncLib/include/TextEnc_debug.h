#ifndef __TEXTENC_DEBUG_H__ 
#define __TEXTENC_DEBUG_H__

#ifndef LOG_TAG
#define LOG_TAG "TextEncApi"
#endif


#define __EYESEE_LINUX__

#ifdef __EYESEE_LINUX__
    #include <cutils/log.h>
    #define LOG_LEVEL_ERROR     _GLOG_ERROR
    #define LOG_LEVEL_WARNING   _GLOG_WARN
    #define LOG_LEVEL_INFO      _GLOG_INFO
    #define LOG_LEVEL_VERBOSE   _GLOG_INFO
    #define LOG_LEVEL_DEBUG     _GLOG_INFO

    #define AWLOG(level, fmt, arg...)  \
        GLOG_PRINT(level, fmt, ##arg)

#define CC_LOG_ASSERT(e, fmt, arg...)                               \
        LOG_ALWAYS_FATAL_IF(                                        \
                !(e),                                               \
                "<%s:%d>check (%s) failed:" fmt,                    \
                __FUNCTION__, __LINE__, #e, ##arg)                  \

#elif defined(__LINUX__)

#include <stdio.h>
#include <string.h>

#define LOG_LEVEL_ERROR     "error  "
#define LOG_LEVEL_WARNING   "warning"
#define LOG_LEVEL_INFO      "info   "
#define LOG_LEVEL_VERBOSE   "verbose"
#define LOG_LEVEL_DEBUG     "debug  "

#define AWLOG(level, fmt, arg...)  \
    printf("%s: %s <%s:%u>: "fmt"\n", level, LOG_TAG, __FUNCTION__, __LINE__, ##arg)

#define CC_LOG_ASSERT(e, fmt, arg...)                                       \
                do {                                                        \
                    if (!(e))                                               \
                    {                                                       \
                        loge("check (%s) failed:"fmt, #e, ##arg);           \
                        assert(0);                                          \
                    }                                                       \
                } while (0)

#endif




#define LOGD(fmt, arg...) AWLOG(LOG_LEVEL_DEBUG, "debug  " fmt, ##arg)


#if CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_CLOSE

#define LOGE(fmt, arg...)
#define LOGW(fmt, arg...)
#define LOGI(fmt, arg...)
#define LOGV(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_ERROR

#define LOGE(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, "\033[40;31m" fmt "\033[0m", ##arg)
#define LOGW(fmt, arg...)
#define LOGI(fmt, arg...)
#define LOGV(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_WARNING

#define LOGE(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, "\033[40;31m" fmt "\033[0m", ##arg)
#define LOGW(fmt, arg...) AWLOG(LOG_LEVEL_WARNING, fmt, ##arg)
#define LOGI(fmt, arg...)
#define LOGV(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_DEFAULT

#define LOGE(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, "\033[40;31m" fmt "\033[0m", ##arg)
#define LOGW(fmt, arg...) AWLOG(LOG_LEVEL_WARNING, fmt, ##arg)
#define LOGI(fmt, arg...) AWLOG(LOG_LEVEL_INFO, fmt, ##arg)
#define LOGV(fmt, arg...)

#elif CONFIG_LOG_LEVEL == OPTION_LOG_LEVEL_DETAIL

#define LOGE(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, "\033[40;31m" fmt "\033[0m", ##arg)
#define LOGW(fmt, arg...) AWLOG(LOG_LEVEL_WARNING, fmt, ##arg)
#define LOGI(fmt, arg...) AWLOG(LOG_LEVEL_INFO, fmt, ##arg)
#define LOGV(fmt, arg...) AWLOG(LOG_LEVEL_VERBOSE, fmt, ##arg)

#else

#error "invalid configuration of debug level."

#endif  // CONFIG_LOG_LEVEL



#endif
