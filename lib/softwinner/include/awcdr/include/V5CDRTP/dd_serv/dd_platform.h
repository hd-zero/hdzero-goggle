/* *******************************************************************************
 * Copyright (C), 2001-2018, Allwinner Tech. Co., Ltd.
 * *******************************************************************************/
/**
 * @file dd_platform.h
 * @brief dd_platform接口定义文件
 * 
 *
 * @version v0.1
 * @date 2018-05-14
 */
#ifndef __DD_PLATFORM_H__
#define __DD_PLATFORM_H__
  
#include "../source/bll_presenter/AdapterLayer.h"
#include "dd_common.h"

#define FILE_ORDERID            "/data/order_state.txt"
#define FILE_SHOOTING_STATUS            "/data/shooting_status.txt"
#define FILE_GSENSOR            "/data/gsensor_state.txt"

//#define USE_IPC
//#define USE_REPORT_INFO

#define QLIMIT_REPORT_MAX           (30)
#define QLIMIT_REPORT_LIMIT         (20)

#define NETSTATUS_OK    (1)
#define NETSTATUS_FAIL  (0)
#define REPORT_TIMEOUT  (600)   // second

#define CTRL_FLAG_PERMIT_ALL    (0)
#define CTRL_FLAG_PERMIT_IN     ((~(1<<0))&0x7fffffff) //permit inside camera action
#define CTRL_FLAG_PERMIT_OUT    ((~(1<<1))&0x7fffffff) //permit outside camera action
#define CTRL_FLAG_FORBID_IN     (1<<0)  //forbid inside camera action
#define CTRL_FLAG_FORBID_OUT    (1<<1)  //forbid outside camera action
#define CTRL_FLAG_FORBID_ALL    (CTRL_FLAG_FORBID_IN | CTRL_FLAG_FORBID_OUT) //forbid dual camera action

#define CAMID_NONE              (-1) //shoot type invalid camera
#define CAMID_IN                (0) //shoot type inside camera
#define CAMID_OUT               (1) //shoot type outside camera

#ifdef USE_REPORT_INFO
extern GlobalInfo globalInfo;
#endif

typedef struct _publish_package {
    int size;
    char *data;
} PUB_PACKAGE;

typedef struct __ctrl_flag__
{
    int flag_takepic;
    int flag_filelist;
    int flag_formatsd;
    int flag_facedetect;
    int flag_litevideo;
    int flag_recordaudio;
    int flag_trafficdata;
} CtrlFlag;

typedef struct __SHOOTING_STATUS__
{
    int a_cam;
    int audio;
}ShootingStatus;
class dd_Bg_Serv
{
	public:
		dd_Bg_Serv();
		~dd_Bg_Serv();
		static dd_Bg_Serv* GetInstance();
        int v_bind_flag;

	public:
	/** ddGetCtrlFlag - get flag of disable cmd reentry
	 *  @param type - cmd type; camid - camera id to get flag
	 *  @return 0 success, return -1 failed
	 */
     int ddGetCtrlFlag(int type, int camid);

	/** ddSetCtrlFlag - set flag of disable cmd reentry
	 *  @param type - cmd type; flag - flag value
	 *  @return 0 success 
	 */
     int ddSetCtrlFlag(int type, int flag);

	/** ddGetCtrlFlag_FormatSD - get flag of disable FormatSD when relative function running
	 *  @param 
	 *  @return 0 success 
	 */
     int ddGetCtrlFlag_FormatSD(void);

	/** ddSetCtrlFlag_FormatSD_Permit - set permit flag of permit FormatSD
	 *  @param 
	 *  @return 0 success 
	 */
     int ddSetCtrlFlag_FormatSD_Permit(void);

	/** ddSetCtrlFlag_FormatSD_Forbid - set forbid flag of forbid FormatSD
	 *  @param 
	 *  @return 0 success 
	 */
     int ddSetCtrlFlag_FormatSD_Forbid(void);

	/** ddSetFilePushInfoPoolNode - set node to vector
	 *  @param node - set var value to node
	 *  @return 0 success 
	 */
     int ddSetFilePushInfoPoolNode(FilePushInfoPoolNode  &node,
               int chunk_size, int shoot_type, std::string file_path, std::string serialcode, std::string callbackurl, int iswakedata);

	/** ddSetFileListPushInfoPoolNode - set node to vector
	 *  @param node - set var value to node
	 *  @return 0 success 
	 */
     int ddSetFileListPushInfoPoolNode(FileListPushInfoPoolNode  &node,
               int chunk_size, int shoot_type, std::string file_path, std::string serialcode, std::string callbackurl,std::string order_id,std::string timestamp_start,std::string timestamp_end, int iswakedata);

	/** ddFindFilePushInfoPoolNode - find node from file_push_pool,with file_name, output to out_node
	 *  @param node - set var value to node
	 *  @return 0 success 
	 */
     int ddFindFilePushInfoPoolNode(std::vector<FilePushInfoPoolNode> &file_push_pool, std::string file_name, FilePushInfoPoolNode &out_node);

	/** ddFindFileListPushInfoPoolNode - find node from file_push_pool,with file_name, output to out_node
	 *  @param node - set var value to node
	 *  @return 0 success 
	 */
     int ddFindFileListPushInfoPoolNode(std::vector<FileListPushInfoPoolNode> &file_push_pool, std::string file_name, FileListPushInfoPoolNode &out_node);

	//for device related api
	//=======================================================================
	/** ddGetDevStatusInfo - 获取设备硬件状态信息
	 *  @param devInfo, out - 获取结果保存到RemoteDeviceStatusInfo结构指针
	 *  @return 0 success 
	 */
	int ddGetDevStatusInfo(RemoteDeviceStatusInfo &devInfo);

	/** ddGetDevAbnormalInfo - 获取设备异常状态信息
	 *  @param devInfo, out - 获取结果保存到RemoteDeviceAbnormalInfo结构指针
	 *  @return 0 success 
	 */
	int ddGetDevAbnormalInfo(RemoteDeviceAbnormalInfo *devInfo);

	/** ddSetPlaceOrder - 设置下发、结束顺风车订单
	 *  @param order_id, in - 订单号
	 *  @param order_state, in - 订单状态：1开始，0结束
	 *  @return 0 success 
	 */
	int ddSetPlaceOrder(std::string order_id, int order_state);

	/** ddSetDevPorperty - 设置前后摄分辨率、帧率、g-sensor阀值属性
	 *  @param a_cam, b_cam in - 前后录像分辨率、帧率,0-4
	 *  @param g_sensor, in - gsensor阀值
	 *  @return 0 success 
	 */
	int ddSetDevPorperty(int a_cam, int b_cam, int g_sensor);
	
	/** ddSetDevPorpertyEx - 设置g-sensor阀值属性
	 *  @param g_sensor, in - gsensor阀值
	 *  @return 0 success 
	 */
	int ddSetDevPorpertyEx(int g_sensor);
	
	/** ddSetShootingStatus - 设置设备拍摄状态，开关前后路、车内录音
	 *  @param a_cam, b_cam in - 前后录像拍摄, 0开，1关
	 *  @param audio, in - 车内录音状态，0开，1关，2只录音
	 *  @return 0 success 
	 */
	int ddSetShootingStatus(int a_cam, int b_cam, int audio);

	/** ddSetSilentTakePic - 设置设备静默拍照，照片名，拍哪一路
	 *  @param shoot_type, in - 哪路录像拍摄, 0车内，1前置
	 *  @param pic_name, in - 指定文件名
	 *  @return 0 success 
	 */
	int ddSetSilentTakePic(int shoot_type, const char *pic_name);

	/** ddGetSilentTakePic - 获取静默拍照结果信息
	 *  @param fileInfo, out - 保存静默拍照文件信息到该结构体
	 *  @param pic_name, in - 指定文件名
	 *  @return 0 success 
	 */
	int ddGetSilentTakePic(FilePushInfo &fileInfo);

	/** ddSetFileList - 设置生成文件列表(按文档格式，并加密)
	 *  @param NULL
	 *  @return 0 success 
	 */
	int ddSetFileList(std::string p_startTime, std::string p_stopTime);

	/** ddGetFileList - 获取文件列表信息
	 *  @param fileInfo, out - 保存文件列表信息到该结构体
	 *  @return 0 success 
	 */
	int ddGetFileList(FilePushInfo *fileInfo);

	/** ddSetFaceDetect - 设置启动人脸侦测扣图
	 *  @param NULL
	 *  @return 0 success 
	 */
	int ddSetFaceDetect(int frame_rate, int frame_count);

	/** ddGetFaceDetect - 获取人脸扣图压缩包信息
	 *  @param fileInfo, out -保存扣图压缩包信息到该结构体
	 *  @return 0 success 
	 */
	int ddGetFaceDetect(FilePushInfo *fileInfo);

	/** ddSetLiteVideo - 设置拍摄小视频
	 *  @param type, in - 0车内，1前置
	 *  @param sec, in -拍摄时间多少秒
	 *  @param name, in -文件名
	 *  @return 0 success 
	 */
	int ddSetLiteVideo(const TriggerLiteVideoMsg *lite);

	/** ddGetLiteVideo - 获取小视频信息
	 *  @param fileInfo, out -保存小视频信息到该结构体
	 *  @return 0 success 
	 */
	int ddGetLiteVideo(FilePushInfo *fileInfo);

	/** ddSetFileLockByTime - 设置按时间段锁定文件
	 *  @param sTime, in -开始时间
	 *  @param eTime, in -结束时间
	 *  @param lockTime, in -锁定时间
	 *  @return 0 success 
	 */
	int ddSetFileLockByTime(const char *sTime, const char *eTime, const char *lockTime);

	/** ddSetFileLockByTimeEx - 设置按时间段锁定文件
	 *  @param sTime, in -开始时间
	 *  @param eTime, in -结束时间
	 *  @param lockTime, in -锁定时间
	 *  @param p_CamId = 1
	 *  @param p_OrderId in 
	 *  @return 0 success 
	 */
    int ddSetFileLockByTimeEx(const std::string p_StartTime, const std::string p_StopTime, const std::string p_LockTime, int p_CamId = 1, std::string p_OrderId="");

	/** ddGetFileLockByTime - 获取按时间段锁定文件列表
	 *  @param fileLockInfo, out -保存锁定文件列表信息到该结构体
	 *  @return 0 success 
	 */
	int ddGetFileLockByTime(std::vector<LockFileInfo> &FileInfoVector);

	/** ddSetFileUnlockByTime - 设置 按时间段解锁定文件
	 *  @param sTime, in -开始时间
	 *  @param eTime, in -结束时间
	 *  @return 0 success 
	 */
	int ddSetFileUnlockByTime(const char *sTime, const char *eTime);

	/** ddSetFileUnlockByTimeEx - 设置 按时间段解锁定文件
	 *  @param sTime, in -开始时间
	 *  @param eTime, in -结束时间
	 *  @return 0 success 
	 */
    int ddSetFileUnlockByTimeEx(const std::string p_StartTime, const std::string p_StopTime, int p_CamId = 1, std::string p_OrderId="");

	/** ddGetFileUnlockByTime - 获取按时间段解锁定文件列表
	 *  @param fileLockInfo, out -保存解锁定文件列表信息到该结构体
	 *  @return 0 success 
	 */
	int ddGetFileUnlockByTime(std::vector<LockFileInfo> &FileInfoVector);

	/** ddSetFileLockByName - 设置按文件名锁定文件
	 *  @param name, in -文件名
	 *  @param lockTime, in -锁定时间
	 *  @return 0 success 
	 */
	int ddSetFileLockByName(const char *name, const char *lockTime);


    /** ddGetFileLockByName -  获取按文件名锁定文件信息
      * @param p_fileLockInfo - LockFileInfo
      * @return 0 success
    */
    int ddGetFileLockByName(LockFileInfo &p_fileLockInfo);

	/** ddSetFileUnlockByName - 设置按文件名解锁定文件
	 *  @param name, in -文件名
	 *  @return 0 success 
	 */
	int ddSetFileUnlockByName(const char *name);

    /** ddGetFileUnLockByName -  获取按文件名锁定文件信息
      * @param p_fileLockInfo - LockFileInfo
      * @return 0 success
    */
    int ddGetFileUnLockByName(LockFileInfo &p_fileLockInfo);

    /** ddSetBindFlag - 设置设备绑定状态
      * @param flag - "ture" or "false"
      * @return 0 success
    */
    int ddSetBindFlag(std::string flag);

    /** ddGetBindFlag - 获取设备绑定状态
      * @param flag - to receive result of "ture" or "false"
      * @return 0 success
    */
    int ddGetBindFlag(int *flag);

	/** ddSetRecordingAudio - 设置开启单独录音
	 *  @param name, in -文件名
	 *  @param sec, in -录音时间多少秒
	 *  @return 0 success 
	 */
	int ddSetRecordingAudio(const char *name, int sec);

	/** ddGetRecordingAudio - 获取录音文件信息
	 *  @param fileInfo, out -保存录音文件信息到该结构体
	 *  @return 0 success 
	 */
	int ddGetRecordingAudio(FilePushInfo &fileInfo);

	/** ddSetTrafficData - 设置生成行车数据压缩文件(按文档格式并加密)
	 *  @param NULL
	 *  @return 0 success 
	 */
	int ddSetTrafficData(const TrafficDataMsg *traffMsg);

	/** ddGetTrafficData - 获取行车数据文件信息
	 *  @param fileInfo, out -保存行车数据文件信息到该结构体
	 *  @return 0 success 
	 */
	int ddGetTrafficData(FilePushInfo &fileInfo);

	/** ddSetFormatSD - 设置开启格式化SD卡
	 *  @param NULL
	 *  @return 0 success 
	 */
	int ddSetFormatSD();

	/** ddGetFileInfo - 获取指定文件名对应的信息，包括加密key和MD5
	 *  @param name, in -文件名
	 *  @param fileInfo, out -保存获取的key和md5
	 *  @return 0 success 
	 */
	int ddGetFileInfo(const char *name, FilePushInfo &fileInfo);

	/** ddGetOrderID - 获取订单号
	 *  @param order_id, out -保存获订单号
	 *  @return 0 success 
	 */
	int ddGetOrderID(std::string &order_id);

    int ddGetOrderIDByName(const std::string &p_FileName, std::string &p_OrderId);

	/** ddGetDBasicHWinfo - 获取基本硬件信息，IMEI,经、纬度
	 *  @param HWinfo, out -保存基本硬件信息到该结构体
	 *  @return 0 success 
	 */
	int ddGetBasicHWinfo(RemoteBasicHWinfo *HWinfo);

	int ddGetLockFileList(std::vector<LockFileInfo> &FileInfoVector);

	int ddGetDeleteFileList(std::vector<LockFileInfo> &FileInfoVector,int p_CamId);

	int ddDeleteFile(const std::string filename);

	public:
	//basic dd POST api
	//=======================================================================
	/** ddDevStatusInfo - 上传设备信息到后台,生成json数据，再post上传
	 *  @param devInfo - RemoteDeviceStatusInfo结构指针
	 *  @return 0 success 
	 */
	int ddDevStatusInfoPost(RemoteDeviceStatusInfo *devInfo);
	/** ddDevAbnormalInfo - 设备异常信息上报后台,生成json数据，再post上传
	 *  @param devInfo - RemoteDeviceAbnormalInfo结构指针
	 *  @return 0 success 
	 */
	int ddDevAbnormalInfoPost(RemoteDeviceAbnormalInfo *devInfo);
	/** ddDevFileUpload - 文件内容上传接口,上传指定文件到后台，分片处理,生成json数据，再post上传
	 *  @param fileUpload - RemoteFileUpload结构指针
	 *  @param url - post URL
	 *  @return 0 success 
	 */
	int ddDevFileUploadPost(RemoteFileUpload *fileUpload);
	/** ddDevFileLock - 文件锁定、解锁接口，上传锁定、解锁文件信息给后台,生成json数据，再post上传
	 *  @param fileUpload - RemoteFileUpload结构指针
	 *  @return 0 success 
	 */
	int ddDevFileLockPost(RemoteFileLock *lockFile    );

	

	//for background CMD process 
	//==========================================================================
	/** ProcessPlaceOrder - 下发、结束顺风车订单接口，把订单信息传给设备相关接口
	 *  @param Order - PlaceOrderMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessPlaceOrder(PlaceOrderMsg *Order);
	
	/** ProcessChangeProperty - 更改设备属性接口，调用设备相关接口 设定前后摄相等功能
	 *  @param Property - ChangePropertyMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessChangeProperty(ChangePropertyMsg *Property);
	
	/** ProcessShootingStatus - 更改设备拍摄状态接口，调用设备相关接口 开关前后摄相及录音等功能
	 *  @param Shooting - ShootingStatusMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessShootingStatus(ShootingStatusMsg *Shooting);
	
	/** ProcessSilentTakePic - 静默拍照接口，调用设备拍照接口拍照并 把指定URL传给文件上传接口，供拍照完后上传
	 *  @param Silent - SilentTakePicMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessSilentTakePic(SilentTakePicMsg *Silent);
	
	/** ProcessTriggerDevStatus - 触发设备状态监测，获取设备状态信息并调用ddDevStatusInfo上传设备信息到后台
	 *  @param com - PushComMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessTriggerDevStatus(PushComMsg *com);

	/** ProcessTriggerFileList - 触发文件列表上传，调用生成文件列表接口， 再调用 ddDevFileUpload上传至后台
	 *  @param list - TriggerFileListMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessTriggerFileList(TriggerFileListMsg *list);

	/** ProcessTriggerSpecifyFile - 触发指定文件上传，调用 ddDevFileUpload上传指定文件至后台
	 *  @param Specify - TriggerSpecifyFileMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessTriggerSpecifyFile(TriggerSpecifyFileMsg *Specify);

	/** ProcessTriggerFaceDetect - 触发人脸抓拍并上传，调用人脸侦测与抓图，把指定URL传给文件上传接口，供拍照完后上传
	 *  @param face - TriggerFaceDetectMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessTriggerFaceDetect(TriggerFaceDetectMsg *face);

	/** ProcessTriggerLiteVideo - 触发小视频拍摄并上传，调用小视频拍摄，再把指定URL传给文件上传接口，供拍完后上传
	 *  @param lite - TriggerLiteVideoMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessTriggerLiteVideo(TriggerLiteVideoMsg *lite);
	
	/** ProcessBulkLockFiles - 推送锁定文件与上报，调用设备锁定文件接口，获取锁定文件列表 再调用 ddDevFileLock上报后台
	 *  @param files - BulkLockMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessBulkLockFiles(BulkLockMsg *files);
	
	/** ProcessBulkUnlockFiles - 推送解锁定文件与上报，调用设备解锁定文件接口，获取解锁定文件列表 再调用 ddDevFileLock上报后台
	 *  @param files - BulkUnLockMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessBulkUnlockFiles(BulkUnLockMsg *files);
	
	/** ProcessRecordingAudio - 触发单独录音并上传，调用设备录音接口， 再把指定URL传给文件上传接口，供完成后上传
	 *  @param audio - RecordingAudioMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessRecordingAudio(RecordingAudioMsg *audio);

	/** ProcessTrafficDataUpload - 触发行车数据上传，调用获取行车数据文件接口， 再调用 ddDevFileUpload上传后台
	 *  @param com - PushComMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessTrafficDataUpload(TrafficDataMsg *log);

	/** ProcessTriggerFormatSD - 触发格式化SD卡并上报，调用设备格式化SD卡接口，再获取设备状态信息， 再调用 ddDevStatusInfo上传后台
	 *  @param com - PushComMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessTriggerFormatSD(PushComMsg *com);

	/** ProcessSingleLockFile - 推送单个文件锁定并上报，调用设备锁定文件接口， 再调用 ddDevFileLock上传后台
	 *  @param file - SingleLockMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessSingleLockFile(SingleLockMsg *file);
	
	/** ProcessSingleUnlockFile - 推送单个文件角锁定并上报，调用设备解锁定文件接口， 再调用 ddDevFileLock上传后台
	 *  @param file - SingleLockMsg结构指针
	 *  @return 0 success 
	 */
	int ProcessSingleUnlockFile(SingleUnlockMsg *file);

	/** ProcessBindDevice - 推送绑定或解绑设备状态命令
	 *  @param bindmsg - BindDeviceMsg结构指针
	 *  @return 0 success 
	 */
    int ProcessBindDevice(BindDeviceMsg *bindmsg);

    int ddGetMQTTUserPasswd(std::string &user, std::string &passwd);

	/** EvevtReportDeal - deal report event
	 *  @param 
	 *  @return 0 success 
	 */
    int EvevtReportDeal(int *iswakedata);

	/** ProcessEventReport - report event, call this function if any event need to report;
	 *  @param event - EventReportMsg结构
	 *  @return 0 success 
	 */
	int ProcessEventReport(EventReportMsg event);

	/** ddSetRemoteActionDone - if this cmd is from 4G mqtt cmd, then notify system to need suspend;
	 *  @param value - flag:1 to notify
	 *  @return 0 success 
	 */
    int ddSetRemoteActionDone(int value);

#ifdef USE_IPC
    int ipc_init(void);
    int ipc_deinit(void);
    int MsgDeal(void);
#endif
#ifdef USE_REPORT_INFO
    int MsgReportDeal(int flag);
#endif

	private:
		int EnqueueFaceDectData(RemoteFileUpload &fileUpload, std::string file_name);
		int EnqueueTrafficData(RemoteFileUpload &fileUpload, EventReportMsg event);
		int EnqueueRcordingAudioData(RemoteFileUpload &fileUpload, const std::string filename);
		int GenSDReqDeleteFileData(EventSDReqDeleteFilesData &SDReqDeleteFileData);
#ifdef USE_IPC
        int ipc_msg_creat(int msgId, char **ppContent);
        int ipc_msg_parse(int fieldId, char *pContent, char *pValue);
        int ipc_msg_send(char cmd);
#endif
        int is_timeout(time_t *t_start, time_t *t_end, int timeout);
        std::vector<FilePushInfoPoolNode> file_push_pool;
        std::vector<FileListPushInfoPoolNode> file_list_push_pool;
        std::vector<FilePushInfo> file_push_info;
        CtrlFlag ctrl_flag;
        int msgid;
        int smsgid;
        time_t t_start;
        time_t t_end;
		pthread_mutex_t dd_serv_lock;
};

#endif

