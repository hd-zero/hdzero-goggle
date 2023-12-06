/** log.h **/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>

#define MAXLEN (2048)
#define MAXFILEPATH (256)
#define MAXFILENAME (50)

#define LOG_defCONF     "log.conf"
#define LOG_defPATH     "/mnt/app/log"
#define LOG_defLEVEL    ALL//INFO
#define LOG_defPERIOD   30

typedef enum{
	ERROR_1=-1,
	ERROR_2=-2,
	ERROR_3=-3
}ERROR0;

typedef enum{
	NONE=0,
	INFO=1,
	DEBUG=2,
	WARN=3,
	ERROR=4,
	ALL=255
}LOGLEVEL;

int  log_open(void);
int  log_write(LOGLEVEL loglevel,char *fromat, ...);
uint32_t log_period(void);
void log_close(void);

#ifdef __cplusplus
}
#endif
