#ifndef __DD_LOG_H__
#define __DD_LOG_H__
#include <stdarg.h>

#define DD_SERV_LOG_FILE_PATH		"/mnt/extsd/.tmp/.log/"
#define LOG_SIZE			10 * 1024

void log_msg(const char *log_type, const char * file, int line_no, const char *fmt, ...);
void watch_log_file_is_timeout();

#endif