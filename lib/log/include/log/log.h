#ifndef LOG_H
#define LOG_H

#define LOG_LEVEL_DEBUG   -2
#define LOG_LEVEL_VERBOSE -1
#define LOG_LEVEL_INFO    0
#define LOG_LEVEL_WARN    1
#define LOG_LEVEL_ERROR   2
#define LOG_LEVEL_FATAL   3

#define LOGE(...) log_printf(__FILE__, __func__, __LINE__, LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOGW(...) log_printf(__FILE__, __func__, __LINE__, LOG_LEVEL_WARN, __VA_ARGS__)
#define LOGI(...) log_printf(__FILE__, __func__, __LINE__, LOG_LEVEL_INFO, __VA_ARGS__)
#define LOGV(...) log_printf(__FILE__, __func__, __LINE__, LOG_LEVEL_VERBOSE, __VA_ARGS__)
#define LOGD(...) log_printf(__FILE__, __func__, __LINE__, LOG_LEVEL_DEBUG, __VA_ARGS__)

int log_printf(const char *file, const char *func, int line, const int level, const char *fmt, ...);

#endif