#ifndef __HTTPD_H__
#define __HTTPD_H__

enum httpdStat {
	POST_FILE_START,
	POST_FILE_END,
	POST_FILE_ERROR,
};

typedef struct __httpdCallback {
	int (*httpdStatCallback)(int stat, void *argv, int argvLen, void *handler);
	int (*httpdDataCallback)(void *argv, int argvLen, void *handler);
	void *handler;
}httpdCallback;

#ifdef __cplusplus
extern "C" {
#endif

int startHttpd(httpdCallback *callback);
int stopHttpd();

//обтьочкы
//#define DOWNLOAD_SPEED_SEC 0
//#define DOWNLOAD_SPEED_MICRO_SEC 0

#ifdef __cplusplus	
}
#endif
#endif
