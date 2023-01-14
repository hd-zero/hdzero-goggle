#ifndef LOG_PRINT_H_
#define LOG_PRINT_H_

#define _GLOG_INFO  0
#define _GLOG_WARN  1
#define _GLOG_ERROR 2
#define _GLOG_FATAL 3

#ifdef __cplusplus
extern "C" {
#endif
typedef struct GLogConfig
{
    //"log messages go to stderr instead of logfiles"
    int FLAGS_logtostderr;  // = false;  //--logtostderr=1, GLOG_logtostderr=1
    //"color messages logged to stderr (if supported by terminal)"
    int FLAGS_colorlogtostderr; // = true;
    //"log messages at or above this level are copied to stderr in addition to logfiles.  This flag obsoletes --alsologtostderr."
    int FLAGS_stderrthreshold; // = google::GLOG_WARNING;
    //"Messages logged at a lower level than this don't actually get logged anywhere"
    int FLAGS_minloglevel; // = google::GLOG_INFO;
    //"Buffer log messages logged at this level or lower (-1 means don't buffer; 0 means buffer INFO only;...)"
    int FLAGS_logbuflevel; // = -1;
    //"Buffer log messages for at most this many seconds"
    int FLAGS_logbufsecs; // = 0;
    //"approx. maximum log file size (in MB). A value of 0 will be silently overridden to 1."
    int FLAGS_max_log_size; // = 25;
    //"Stop attempting to log to disk if the disk is full."
    int FLAGS_stop_logging_if_full_disk; // = true;

    //e.g., "/tmp/log/LOG-"
    char LogDir[128];   //e.g., "/tmp/log"
    char InfoLogFileNameBase[128];  //e.g., "LOG-"
    char LogFileNameExtension[128];    //e.g., "SDV-"
}GLogConfig;
void log_init(const char *program, GLogConfig *pConfig);
void log_quit();
int log_printf(const char *file, const char *func, int line, const int level, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
