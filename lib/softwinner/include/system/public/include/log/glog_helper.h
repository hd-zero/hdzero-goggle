#pragma once

#include <glog/logging.h>
#include <glog/raw_logging.h>

//配置输出日志的目录：
#define GLOG_FILE_PATH "/tmp/log"


class GLogHelper
{
public:
    class GLogConfig
    {
    public:
        std::string program;
        //"log messages go to stderr instead of logfiles"
        bool FLAGS_logtostderr;
        //"color messages logged to stderr (if supported by terminal)"
        bool FLAGS_colorlogtostderr;
        //"log messages at or above this level are copied to stderr in addition to logfiles.  This flag obsoletes --alsologtostderr."
        int FLAGS_stderrthreshold;
        //"Messages logged at a lower level than this don't actually get logged anywhere"
        int FLAGS_minloglevel;
        //"Buffer log messages logged at this level or lower (-1 means don't buffer; 0 means buffer INFO only;...)"
        int FLAGS_logbuflevel;
        //"Buffer log messages for at most this many seconds"
        int FLAGS_logbufsecs;
        //"Log file mode/permissions."
        int FLAGS_logfile_mode;
        //"approx. maximum log file size (in MB). A value of 0 will be silently overridden to 1."
        int FLAGS_max_log_size;
        //"Stop attempting to log to disk if the disk is full."
        bool FLAGS_stop_logging_if_full_disk;
        //"Show all VLOG(m) messages for m <= this. Overridable by --vmodule."
        int FLAGS_v;

        // e.g., filename prefix is "/tmp/log/LOG-SDV-"
        std::string LogDir; //e.g., "/tmp/log"
        std::string InfoLogFileNameBase;  //e.g., "LOG-"
        std::string LogFileNameExtension;    //e.g., "SDV-"

        GLogConfig();
        ~GLogConfig();
    };
    //GLOG配置：
    //GLogHelper(const char* program);
    GLogHelper(GLogConfig& config);

    //GLOG内存清理：
    ~GLogHelper();
};
