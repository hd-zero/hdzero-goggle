/* *******************************************************************************
 * Copyright (C), 2001-2018, Allwinner Tech. Co., Ltd.
 * *******************************************************************************/
/**
 * @file dd_platform.h
 * @brief dd_platform�ӿڶ����ļ�
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
	/** ddGetDevStatusInfo - ��ȡ�豸Ӳ��״̬��Ϣ
	 *  @param devInfo, out - ��ȡ������浽RemoteDeviceStatusInfo�ṹָ��
	 *  @return 0 success 
	 */
	int ddGetDevStatusInfo(RemoteDeviceStatusInfo &devInfo);

	/** ddGetDevAbnormalInfo - ��ȡ�豸�쳣״̬��Ϣ
	 *  @param devInfo, out - ��ȡ������浽RemoteDeviceAbnormalInfo�ṹָ��
	 *  @return 0 success 
	 */
	int ddGetDevAbnormalInfo(RemoteDeviceAbnormalInfo *devInfo);

	/** ddSetPlaceOrder - �����·�������˳�糵����
	 *  @param order_id, in - ������
	 *  @param order_state, in - ����״̬��1��ʼ��0����
	 *  @return 0 success 
	 */
	int ddSetPlaceOrder(std::string order_id, int order_state);

	/** ddSetDevPorperty - ����ǰ����ֱ��ʡ�֡�ʡ�g-sensor��ֵ����
	 *  @param a_cam, b_cam in - ǰ��¼��ֱ��ʡ�֡��,0-4
	 *  @param g_sensor, in - gsensor��ֵ
	 *  @return 0 success 
	 */
	int ddSetDevPorperty(int a_cam, int b_cam, int g_sensor);
	
	/** ddSetDevPorpertyEx - ����g-sensor��ֵ����
	 *  @param g_sensor, in - gsensor��ֵ
	 *  @return 0 success 
	 */
	int ddSetDevPorpertyEx(int g_sensor);
	
	/** ddSetShootingStatus - �����豸����״̬������ǰ��·������¼��
	 *  @param a_cam, b_cam in - ǰ��¼������, 0����1��
	 *  @param audio, in - ����¼��״̬��0����1�أ�2ֻ¼��
	 *  @return 0 success 
	 */
	int ddSetShootingStatus(int a_cam, int b_cam, int audio);

	/** ddSetSilentTakePic - �����豸��Ĭ���գ���Ƭ��������һ·
	 *  @param shoot_type, in - ��·¼������, 0���ڣ�1ǰ��
	 *  @param pic_name, in - ָ���ļ���
	 *  @return 0 success 
	 */
	int ddSetSilentTakePic(int shoot_type, const char *pic_name);

	/** ddGetSilentTakePic - ��ȡ��Ĭ���ս����Ϣ
	 *  @param fileInfo, out - ���澲Ĭ�����ļ���Ϣ���ýṹ��
	 *  @param pic_name, in - ָ���ļ���
	 *  @return 0 success 
	 */
	int ddGetSilentTakePic(FilePushInfo &fileInfo);

	/** ddSetFileList - ���������ļ��б�(���ĵ���ʽ��������)
	 *  @param NULL
	 *  @return 0 success 
	 */
	int ddSetFileList(std::string p_startTime, std::string p_stopTime);

	/** ddGetFileList - ��ȡ�ļ��б���Ϣ
	 *  @param fileInfo, out - �����ļ��б���Ϣ���ýṹ��
	 *  @return 0 success 
	 */
	int ddGetFileList(FilePushInfo *fileInfo);

	/** ddSetFaceDetect - ����������������ͼ
	 *  @param NULL
	 *  @return 0 success 
	 */
	int ddSetFaceDetect(int frame_rate, int frame_count);

	/** ddGetFaceDetect - ��ȡ������ͼѹ������Ϣ
	 *  @param fileInfo, out -�����ͼѹ������Ϣ���ýṹ��
	 *  @return 0 success 
	 */
	int ddGetFaceDetect(FilePushInfo *fileInfo);

	/** ddSetLiteVideo - ��������С��Ƶ
	 *  @param type, in - 0���ڣ�1ǰ��
	 *  @param sec, in -����ʱ�������
	 *  @param name, in -�ļ���
	 *  @return 0 success 
	 */
	int ddSetLiteVideo(const TriggerLiteVideoMsg *lite);

	/** ddGetLiteVideo - ��ȡС��Ƶ��Ϣ
	 *  @param fileInfo, out -����С��Ƶ��Ϣ���ýṹ��
	 *  @return 0 success 
	 */
	int ddGetLiteVideo(FilePushInfo *fileInfo);

	/** ddSetFileLockByTime - ���ð�ʱ��������ļ�
	 *  @param sTime, in -��ʼʱ��
	 *  @param eTime, in -����ʱ��
	 *  @param lockTime, in -����ʱ��
	 *  @return 0 success 
	 */
	int ddSetFileLockByTime(const char *sTime, const char *eTime, const char *lockTime);

	/** ddSetFileLockByTimeEx - ���ð�ʱ��������ļ�
	 *  @param sTime, in -��ʼʱ��
	 *  @param eTime, in -����ʱ��
	 *  @param lockTime, in -����ʱ��
	 *  @param p_CamId = 1
	 *  @param p_OrderId in 
	 *  @return 0 success 
	 */
    int ddSetFileLockByTimeEx(const std::string p_StartTime, const std::string p_StopTime, const std::string p_LockTime, int p_CamId = 1, std::string p_OrderId="");

	/** ddGetFileLockByTime - ��ȡ��ʱ��������ļ��б�
	 *  @param fileLockInfo, out -���������ļ��б���Ϣ���ýṹ��
	 *  @return 0 success 
	 */
	int ddGetFileLockByTime(std::vector<LockFileInfo> &FileInfoVector);

	/** ddSetFileUnlockByTime - ���� ��ʱ��ν������ļ�
	 *  @param sTime, in -��ʼʱ��
	 *  @param eTime, in -����ʱ��
	 *  @return 0 success 
	 */
	int ddSetFileUnlockByTime(const char *sTime, const char *eTime);

	/** ddSetFileUnlockByTimeEx - ���� ��ʱ��ν������ļ�
	 *  @param sTime, in -��ʼʱ��
	 *  @param eTime, in -����ʱ��
	 *  @return 0 success 
	 */
    int ddSetFileUnlockByTimeEx(const std::string p_StartTime, const std::string p_StopTime, int p_CamId = 1, std::string p_OrderId="");

	/** ddGetFileUnlockByTime - ��ȡ��ʱ��ν������ļ��б�
	 *  @param fileLockInfo, out -����������ļ��б���Ϣ���ýṹ��
	 *  @return 0 success 
	 */
	int ddGetFileUnlockByTime(std::vector<LockFileInfo> &FileInfoVector);

	/** ddSetFileLockByName - ���ð��ļ��������ļ�
	 *  @param name, in -�ļ���
	 *  @param lockTime, in -����ʱ��
	 *  @return 0 success 
	 */
	int ddSetFileLockByName(const char *name, const char *lockTime);


    /** ddGetFileLockByName -  ��ȡ���ļ��������ļ���Ϣ
      * @param p_fileLockInfo - LockFileInfo
      * @return 0 success
    */
    int ddGetFileLockByName(LockFileInfo &p_fileLockInfo);

	/** ddSetFileUnlockByName - ���ð��ļ����������ļ�
	 *  @param name, in -�ļ���
	 *  @return 0 success 
	 */
	int ddSetFileUnlockByName(const char *name);

    /** ddGetFileUnLockByName -  ��ȡ���ļ��������ļ���Ϣ
      * @param p_fileLockInfo - LockFileInfo
      * @return 0 success
    */
    int ddGetFileUnLockByName(LockFileInfo &p_fileLockInfo);

    /** ddSetBindFlag - �����豸��״̬
      * @param flag - "ture" or "false"
      * @return 0 success
    */
    int ddSetBindFlag(std::string flag);

    /** ddGetBindFlag - ��ȡ�豸��״̬
      * @param flag - to receive result of "ture" or "false"
      * @return 0 success
    */
    int ddGetBindFlag(int *flag);

	/** ddSetRecordingAudio - ���ÿ�������¼��
	 *  @param name, in -�ļ���
	 *  @param sec, in -¼��ʱ�������
	 *  @return 0 success 
	 */
	int ddSetRecordingAudio(const char *name, int sec);

	/** ddGetRecordingAudio - ��ȡ¼���ļ���Ϣ
	 *  @param fileInfo, out -����¼���ļ���Ϣ���ýṹ��
	 *  @return 0 success 
	 */
	int ddGetRecordingAudio(FilePushInfo &fileInfo);

	/** ddSetTrafficData - ���������г�����ѹ���ļ�(���ĵ���ʽ������)
	 *  @param NULL
	 *  @return 0 success 
	 */
	int ddSetTrafficData(const TrafficDataMsg *traffMsg);

	/** ddGetTrafficData - ��ȡ�г������ļ���Ϣ
	 *  @param fileInfo, out -�����г������ļ���Ϣ���ýṹ��
	 *  @return 0 success 
	 */
	int ddGetTrafficData(FilePushInfo &fileInfo);

	/** ddSetFormatSD - ���ÿ�����ʽ��SD��
	 *  @param NULL
	 *  @return 0 success 
	 */
	int ddSetFormatSD();

	/** ddGetFileInfo - ��ȡָ���ļ�����Ӧ����Ϣ����������key��MD5
	 *  @param name, in -�ļ���
	 *  @param fileInfo, out -�����ȡ��key��md5
	 *  @return 0 success 
	 */
	int ddGetFileInfo(const char *name, FilePushInfo &fileInfo);

	/** ddGetOrderID - ��ȡ������
	 *  @param order_id, out -����񶩵���
	 *  @return 0 success 
	 */
	int ddGetOrderID(std::string &order_id);

    int ddGetOrderIDByName(const std::string &p_FileName, std::string &p_OrderId);

	/** ddGetDBasicHWinfo - ��ȡ����Ӳ����Ϣ��IMEI,����γ��
	 *  @param HWinfo, out -�������Ӳ����Ϣ���ýṹ��
	 *  @return 0 success 
	 */
	int ddGetBasicHWinfo(RemoteBasicHWinfo *HWinfo);

	int ddGetLockFileList(std::vector<LockFileInfo> &FileInfoVector);

	int ddGetDeleteFileList(std::vector<LockFileInfo> &FileInfoVector,int p_CamId);

	int ddDeleteFile(const std::string filename);

	public:
	//basic dd POST api
	//=======================================================================
	/** ddDevStatusInfo - �ϴ��豸��Ϣ����̨,����json���ݣ���post�ϴ�
	 *  @param devInfo - RemoteDeviceStatusInfo�ṹָ��
	 *  @return 0 success 
	 */
	int ddDevStatusInfoPost(RemoteDeviceStatusInfo *devInfo);
	/** ddDevAbnormalInfo - �豸�쳣��Ϣ�ϱ���̨,����json���ݣ���post�ϴ�
	 *  @param devInfo - RemoteDeviceAbnormalInfo�ṹָ��
	 *  @return 0 success 
	 */
	int ddDevAbnormalInfoPost(RemoteDeviceAbnormalInfo *devInfo);
	/** ddDevFileUpload - �ļ������ϴ��ӿ�,�ϴ�ָ���ļ�����̨����Ƭ����,����json���ݣ���post�ϴ�
	 *  @param fileUpload - RemoteFileUpload�ṹָ��
	 *  @param url - post URL
	 *  @return 0 success 
	 */
	int ddDevFileUploadPost(RemoteFileUpload *fileUpload);
	/** ddDevFileLock - �ļ������������ӿڣ��ϴ������������ļ���Ϣ����̨,����json���ݣ���post�ϴ�
	 *  @param fileUpload - RemoteFileUpload�ṹָ��
	 *  @return 0 success 
	 */
	int ddDevFileLockPost(RemoteFileLock *lockFile    );

	

	//for background CMD process 
	//==========================================================================
	/** ProcessPlaceOrder - �·�������˳�糵�����ӿڣ��Ѷ�����Ϣ�����豸��ؽӿ�
	 *  @param Order - PlaceOrderMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessPlaceOrder(PlaceOrderMsg *Order);
	
	/** ProcessChangeProperty - �����豸���Խӿڣ������豸��ؽӿ� �趨ǰ������ȹ���
	 *  @param Property - ChangePropertyMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessChangeProperty(ChangePropertyMsg *Property);
	
	/** ProcessShootingStatus - �����豸����״̬�ӿڣ������豸��ؽӿ� ����ǰ�����༰¼���ȹ���
	 *  @param Shooting - ShootingStatusMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessShootingStatus(ShootingStatusMsg *Shooting);
	
	/** ProcessSilentTakePic - ��Ĭ���սӿڣ������豸���սӿ����ղ� ��ָ��URL�����ļ��ϴ��ӿڣ�����������ϴ�
	 *  @param Silent - SilentTakePicMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessSilentTakePic(SilentTakePicMsg *Silent);
	
	/** ProcessTriggerDevStatus - �����豸״̬��⣬��ȡ�豸״̬��Ϣ������ddDevStatusInfo�ϴ��豸��Ϣ����̨
	 *  @param com - PushComMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessTriggerDevStatus(PushComMsg *com);

	/** ProcessTriggerFileList - �����ļ��б��ϴ������������ļ��б�ӿڣ� �ٵ��� ddDevFileUpload�ϴ�����̨
	 *  @param list - TriggerFileListMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessTriggerFileList(TriggerFileListMsg *list);

	/** ProcessTriggerSpecifyFile - ����ָ���ļ��ϴ������� ddDevFileUpload�ϴ�ָ���ļ�����̨
	 *  @param Specify - TriggerSpecifyFileMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessTriggerSpecifyFile(TriggerSpecifyFileMsg *Specify);

	/** ProcessTriggerFaceDetect - ��������ץ�Ĳ��ϴ����������������ץͼ����ָ��URL�����ļ��ϴ��ӿڣ�����������ϴ�
	 *  @param face - TriggerFaceDetectMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessTriggerFaceDetect(TriggerFaceDetectMsg *face);

	/** ProcessTriggerLiteVideo - ����С��Ƶ���㲢�ϴ�������С��Ƶ���㣬�ٰ�ָ��URL�����ļ��ϴ��ӿڣ���������ϴ�
	 *  @param lite - TriggerLiteVideoMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessTriggerLiteVideo(TriggerLiteVideoMsg *lite);
	
	/** ProcessBulkLockFiles - ���������ļ����ϱ��������豸�����ļ��ӿڣ���ȡ�����ļ��б� �ٵ��� ddDevFileLock�ϱ���̨
	 *  @param files - BulkLockMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessBulkLockFiles(BulkLockMsg *files);
	
	/** ProcessBulkUnlockFiles - ���ͽ������ļ����ϱ��������豸�������ļ��ӿڣ���ȡ�������ļ��б� �ٵ��� ddDevFileLock�ϱ���̨
	 *  @param files - BulkUnLockMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessBulkUnlockFiles(BulkUnLockMsg *files);
	
	/** ProcessRecordingAudio - ��������¼�����ϴ��������豸¼���ӿڣ� �ٰ�ָ��URL�����ļ��ϴ��ӿڣ�����ɺ��ϴ�
	 *  @param audio - RecordingAudioMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessRecordingAudio(RecordingAudioMsg *audio);

	/** ProcessTrafficDataUpload - �����г������ϴ������û�ȡ�г������ļ��ӿڣ� �ٵ��� ddDevFileUpload�ϴ���̨
	 *  @param com - PushComMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessTrafficDataUpload(TrafficDataMsg *log);

	/** ProcessTriggerFormatSD - ������ʽ��SD�����ϱ��������豸��ʽ��SD���ӿڣ��ٻ�ȡ�豸״̬��Ϣ�� �ٵ��� ddDevStatusInfo�ϴ���̨
	 *  @param com - PushComMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessTriggerFormatSD(PushComMsg *com);

	/** ProcessSingleLockFile - ���͵����ļ��������ϱ��������豸�����ļ��ӿڣ� �ٵ��� ddDevFileLock�ϴ���̨
	 *  @param file - SingleLockMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessSingleLockFile(SingleLockMsg *file);
	
	/** ProcessSingleUnlockFile - ���͵����ļ����������ϱ��������豸�������ļ��ӿڣ� �ٵ��� ddDevFileLock�ϴ���̨
	 *  @param file - SingleLockMsg�ṹָ��
	 *  @return 0 success 
	 */
	int ProcessSingleUnlockFile(SingleUnlockMsg *file);

	/** ProcessBindDevice - ���Ͱ󶨻����豸״̬����
	 *  @param bindmsg - BindDeviceMsg�ṹָ��
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
	 *  @param event - EventReportMsg�ṹ
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

