#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <unistd.h>

#include "log_print.h"

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

#define GLOG_PRINT(level, fmt, arg...) \
do { \
    log_printf(__FILE__, __func__, __LINE__, level, fmt, ##arg); \
} while (0)

#define ALOGE(fmt, arg...) GLOG_PRINT(_GLOG_ERROR, fmt, ##arg)
#define ALOGD(fmt, arg...) GLOG_PRINT(_GLOG_INFO, fmt, ##arg)
#define ALOGV(fmt, arg...) GLOG_PRINT(_GLOG_INFO, fmt, ##arg)
#define ALOGI(fmt, arg...) GLOG_PRINT(_GLOG_INFO, fmt, ##arg)
#define ALOGW(fmt, arg...) GLOG_PRINT(_GLOG_WARN, fmt, ##arg)

#endif
