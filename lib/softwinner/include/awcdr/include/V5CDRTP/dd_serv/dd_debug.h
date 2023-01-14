/* *******************************************************************************
* Copyright (C), 2001-2018, xiaoyi. Co., Ltd.
* *******************************************************************************/
/**
* @file dd_debug.h
* @brief dd_debug 
*
*
* @version v0.1
* @date 2018-05-28
*/
#ifndef __DD_DEBUG_H__
#define __DD_DEBUG_H__

#include "dd_log.h"
#include <string.h>

#define _DEBUG_         1

#define DEBUG_DBG       3
#define DEBUG_INFO      2
#define DEBUG_WARN      1
#define DEBUG_ERROR     0

#define LOG_NONE	"\033[0m"
#define LOG_RED		"\033[0;31m"
#define LOG_GREEN	"\033[0;32m"
#define LOG_YELLOW	"\033[1;33m"
#define LOG_INFO	"\033[1;35m"

#define filename(x) strrchr(x,'/')?strrchr(x,'/')+1:x
extern int g_debug_level;
//get the g_debug_level from /mnt/extsd/dd_serv.log ,if not exist ,use the default value
int GET_dd_log_level(void);

#if _DEBUG_
#define DBG_DEBUG(fmt, arg...)   \
						do {\
							if(g_debug_level >= DEBUG_DBG) {\
								printf(LOG_GREEN"[%s] [%s] [%d] ",filename(__FILE__), __func__, __LINE__);\
								printf(fmt, ##arg);\
								printf(LOG_NONE"\n");\
								log_msg("DEBUG", __func__, __LINE__, fmt, ##arg);\
							}\
						} while(0)
#define DBG_INFO(fmt, arg...)    \
						do {\
							if(g_debug_level >= DEBUG_INFO) {\
								printf(LOG_INFO"[%s] [%s] [%d] ",filename(__FILE__), __func__,__LINE__);\
								printf(fmt, ##arg);\
								printf(LOG_NONE"\n");\
								log_msg("INFO",  __func__, __LINE__, fmt, ##arg);\
							}\
						} while(0)
#define DBG_WARN(fmt, arg...)    \
						do {\
							if(g_debug_level >= DEBUG_WARN) {\
								printf(LOG_YELLOW"[%s] [%s]  [%d] ",filename(__FILE__), __func__,__LINE__);\
								printf(fmt, ##arg);\
								printf(LOG_NONE"\n");\
								log_msg("WARN",  __func__, __LINE__, fmt, ##arg);\
							}\
						} while(0)
#define DBG_ERROR(fmt, arg...)   \
						do {\
							if(g_debug_level >= DEBUG_ERROR) {\
								printf(LOG_RED"[%s] [%s]  [%d] ",filename(__FILE__), __func__,__LINE__);\
								printf(fmt, ##arg);\
								printf(LOG_NONE"\n");\
								log_msg("ERROR",  __func__, __LINE__, fmt, ##arg);\
							}\
						} while(0)
#else
#define DBG_DEBUG(x)
#define DBG_INFO(x) 
#define DBG_WARN(x) 
#define DBG_ERROR(x)
#endif

#endif

