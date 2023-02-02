/** log.c **/

#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <pthread.h>
#include "log.h"

#define MAXLEVELNUM (3)
#define MAX_pathLEN 128

typedef struct
{
	char logtime[20];
	FILE *logfile;
	pthread_mutex_t mutex;
} LogContext_t;

typedef struct
{
	char filepath[MAXFILEPATH];
	unsigned int maxfilelen;
	uint8_t  loglevel;
	uint32_t period;
} LogSettings_t;

LogSettings_t logsetting;
LogContext_t  loging;

const static char _logLevelText[4][10]={
    "INFO",
    "DEBUG",
    "WARN",
    "ERROR"
};


/*
 *获取日期
 * */
static char * log_getdate(char *date)
{
	time_t timer=time(NULL);
	strftime(date,11,"%Y-%m-%d",localtime(&timer));
	return date;
}

/*
 *获取时间
 * */
static void log_settime(void)
{
	time_t timer=time(NULL);
	strftime(loging.logtime,20,"%Y-%m-%d %H:%M:%S",localtime(&timer));
}

static LOGLEVEL log_getcode(char *path)
{
	LOGLEVEL code = ALL;
	if(strcmp("INFO",path)==0)
		code=INFO;
	else if(strcmp("WARN",path)==0)
		code=WARN;
	else if(strcmp("ERROR",path)==0)
		code=ERROR;
	else if(strcmp("NONE",path)==0)
		code=NONE;
	else if(strcmp("DEBUG",path)==0)
		code=DEBUG;
	return code;
}

long log_getfilesize(char* filename)
{
    long length = 0;
    FILE *fp = NULL;

    fp = fopen(filename, "rb");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
    }

    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    return length;
}

/***********************************************************
 * check the path
 * create the path if not exist
***********************************************************/
bool log_checkpath(char* sPath)
{
	struct stat st;

	//printf("log_checkpath: %s\n", sPath);

	stat(sPath, &st);
	if(!S_ISDIR(st.st_mode))
	{
		mkdir(sPath, 0777);

		memset(&st, 0, sizeof(st));
		stat(sPath, &st);
		if(!S_ISDIR(st.st_mode))
		{
            //printf("log_checkpath: failed");
			return false;
		}
	}

	return true;
}

static uint8_t log_getconfig(char *path)
{
	char value[MAXFILEPATH]={0x0};
	char data[50]={0x0};

	FILE *fpath=fopen(path,"r");
	if(fpath==NULL)
		return -1;

	memset(value,0,sizeof(value));
	fscanf(fpath,"path=%s\n",value);
	if(strlen(value) <= 0)
    {
        strcpy(value, LOG_defPATH);
    }
	log_checkpath(value);

	strcat(value,"/");
	log_getdate(data);
	strcat(data,".log");
	strcat(value,data);
	if(strcmp(value,logsetting.filepath)!=0)
		memcpy(logsetting.filepath,value,strlen(value));

	memset(value,0,sizeof(value));
	fscanf(fpath,"level=%s\n",value);
	logsetting.loglevel=log_getcode(value);

	logsetting.period = LOG_defPERIOD;
	fscanf(fpath,"period=%d\n", &logsetting.period);

	fclose(fpath);
	return 0;
}

static void log_getcwd(char* confFile)
{
    char  sTemp[MAX_pathLEN];
    char* p = sTemp;
    readlink("/proc/self/exe", sTemp, MAX_pathLEN);
    p = strrchr(sTemp,'/');
    *p = '\0';
    sprintf(confFile, "%s/%s", sTemp, LOG_defCONF);
}

/*
 *日志设置信息
 * */
static LogSettings_t *log_getsetting(void)
{
    if( logsetting.maxfilelen == 0 ) {
        char path[MAXFILEPATH]={0x0};
        log_getcwd(path);

        //printf("getcwd: %s\n", path);
        if(access(path,F_OK)==0){
            if(log_getconfig(path)!=0){
                logsetting.loglevel=LOG_defLEVEL;
                logsetting.maxfilelen=4096;
                logsetting.period = LOG_defPERIOD;
            }
        }else{
            log_checkpath(LOG_defPATH);
            logsetting.loglevel=LOG_defLEVEL;
            logsetting.maxfilelen=4096;
            logsetting.period = LOG_defPERIOD;
        }
    }

	return &logsetting;
}

/*
 *不定参打印
 * */
static inline void log_print(char* format,va_list args)
{
#if(0)
	int d;
	char c,*s;
	while(*format)
	{
		switch(*format){
			case 's':{
				s = va_arg(args, char *);
				fprintf(loging.logfile,"%s",s);
				break;}
			case 'd':{
				d = va_arg(args, int);
				fprintf(loging.logfile,"%d",d);
				break;}
			case 'c':{
				c = (char)va_arg(args, int);
				fprintf(loging.logfile,"%c",c);
				break;}
			default:{
				if(*format!='%'&&*format!='\n')
					fprintf(loging.logfile,"%c",*format);
				break;}
		}
		format++;
	}
#else
    vfprintf(loging.logfile, format, args);
#endif
	fprintf(loging.logfile, "\n");
}

static int log_prefix(LOGLEVEL loglevel)
{
	LogSettings_t *setting;
	//获取日志配置信息
	if((setting=log_getsetting())==NULL){
		perror("Get Log Set Fail!");
		return -1;
	}
	if((loglevel&(setting->loglevel))!=loglevel)
		return -1;

	//获取日志时间
	log_settime();

	//打开日志文件
	if(loging.logfile==NULL) {
		loging.logfile=fopen(setting->filepath,"a+");
	}
	if(loging.logfile==NULL){
		perror("Open Log File Fail!");
		return -1;
	}
	//写入日志级别，日志时间
	fprintf(loging.logfile,"[%s] [%s] ",_logLevelText[loglevel-1],loging.logtime);
	return 0;
}

int log_open(void)
{
	char strdate[30]={0x0};
    LogSettings_t* setting;

	memset(&logsetting,0,sizeof(LogSettings_t));
	memset(&loging,0,sizeof(LogContext_t));

	if((setting=log_getsetting())==NULL){
		perror("Get Log Settings Failed!");
		return -1;
	}

    //char *path=getenv("HOME");
    char* path = LOG_defPATH;
    memcpy(setting->filepath,path,strlen(path));

    log_getdate(strdate);
    strcat(strdate,".log");
    strcat(setting->filepath,"/");
    strcat(setting->filepath,strdate);

    pthread_mutex_init(&loging.mutex, NULL);
    return 0;
}

void log_close(void)
{
    logsetting.loglevel = 0;
    pthread_mutex_destroy(&loging.mutex);
}

/*
 *日志写入
 * */
int log_write(LOGLEVEL loglevel,char *fromat,...)
{
	int  ret = 0;
	va_list args;

    pthread_mutex_lock(&loging.mutex);
	do{
		//初始化日志
		if(log_prefix(loglevel) != 0)
		{
			ret = -1;
			break;
		}
		//打印日志信息
		va_start(args,fromat);
		log_print(fromat,args);
		va_end(args);
		//文件刷出
		fflush(loging.logfile);
		//日志关闭
		if(loging.logfile!=NULL)
			fclose(loging.logfile);
		loging.logfile=NULL;
	}while(0);
    pthread_mutex_unlock(&loging.mutex);

	return ret;
}

uint32_t log_period(void)
{
    return logsetting.period;
}
