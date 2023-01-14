#ifndef __DD_HTTPS_H__
#define __DD_HTTPS_H__
#include "dd_common.h"
 
#define DD_DEFAULT_READLENGTH 512*1024
#define DD_HTTPS_UPLOAD_MAX_LENGTH	1024*1024
#define DD_HTTPS_UPLOAD_MIN_LENGTH   1*1024

#ifdef USE_REPORT_INFO
#define FUNCTION_DEVINFO        "devinfo"
#define FUNCTION_DEBUG          "loginfo"
#endif

class dd_https
{
	public:
		dd_https();
		~dd_https();
		static dd_https* GetInstance();
		
	public:
		
		int Https_RUN_THREAD();
        void https_set_iswakedata(int iswakedata);
        int https_get_iswakedata();
		void https_post_continue(int iFlag, int iswakedata);
		void https_post_file_byOrder(int iFlag, int iswakedata);
		int dd_https_filelock_post(RemoteFileLock *lockFile, const char *url);	
		
		int dd_https_fileupload_post(RemoteFileUpload *fileUpload);
		int dd_https_fileupload_post(RemoteFileUpload *fileUpload, int _iLength);
		void client_https_transfile_end(int is_finish, int iswakedata);
		int client_http_transfer_report(std::string jsondata, std::string functionvalue);
		
	private:	
		int iNetWorking;
		int iServerErrTimes;
		int iTransporting;
        int v_iswakedata;
        time_t t_start;
        time_t t_end;
		pthread_t https_thread_id;
		pthread_mutex_t https_lock;
		
		static void *HttpsThread(void *arg);
		int client_https_translockfile(int iswakedata);
		
        void get_time_post(int isFinish);
		int client_https_transfile_start(int iswakedata);
		int file_get_chunk_num(RemoteFileUpload *fileUpload);
};



#endif

