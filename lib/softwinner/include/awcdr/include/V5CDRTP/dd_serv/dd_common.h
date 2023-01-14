/* *******************************************************************************
 * Copyright (C), 2001-2018, Allwinner Tech. Co., Ltd.
 * *******************************************************************************/
/**
 * @file dd_common.h
 * @brief DD共用头文件
 * 
 *
 * @version v0.1
 * @date 2018-05-14
 */
#ifndef __DD_COMMON_H__
#define __DD_COMMON_H__
#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <vector>
#include <queue>
#include "dd_debug.h"
#include "dd_queue.h"
#include "dd_encrypt.h"

#define SIZE_1K         (1024)
#define SIZE_1M         (1024*1024)

#define STRUE           "true"
#define SFALSE          "false"

#define CMD_BIND        "true"
#define CMD_UNBIND      "false"
#define CMD_NULL        "null"
#define ICMD_BIND       (1)
#define ICMD_UNBIND     (0)
#define ICMD_NULL       (-1)

#define FLAG_WAKEDATA       (1)
#define FLAG_NONWAKEDATA    (0)

#define GPSCollectQue_MAX   (60)


enum PushType
{
	PLACE_ORDER = 0,		//下发 结束顺风车订单
	CHANGE_PROPERTY,		//更改设备属性
	SHOOTING_STATUS,		//更改拍摄状态
	SILENT_TAKE_PIC,		//静默拍摄并上传
	TRIGGER_DEV_STATUS,		//触发设备状态监测
	TRIGGER_FILE_LIST,		//触发文件列表上传
	TRIGGER_SPECIFY_FILE,	//触发指定文件上传
	TRIGGER_FACE_DETECT,	//触发人脸侦测并上传
	TRIGGER_LITE_VIDEO,		//触发小视频拍摄
	BULK_LOCK_FILES,		//批量锁定文件
	BULK_UNLOCK_FILES,		//批量解锁文件
	RECORDING_AUDIO,		//触发录音命令
	TRAFFIC_DATA_UPLOAD,	//行车数据上传
	TRIGGER_FORMAT_SD,		//触发SD卡格式化
	SINGLE_LOCK_FILE,		//单个文件锁定推送
	SINGLE_UNLOCK_FILE,		//单个文件解锁推送
	TRIGGER_BIND_DEVICE,    //绑定设备
    PUSHTYPE_MAX,
};

enum CmdNo
{
    CMDNO_DEVSTATUS = 1100021,
    CMDNO_DEVABNORMAL = 1100022,
    CMDNO_REQDELETEFILES = 1100023,
    CMDNO_UPGRADESTATUS = 1100024,
};

enum FileAction
{
	UPLOAD_FILE_LIST = 1,	//文件列表上传
	UPLOAD_IM_TAKE_PIC,		//立即拍照相片上传
	UPLOAD_SPECIFY_FILE,	//指定文件上传
	UPLOAD_AUDIO_FILE,		//单独录音文件上传
	UPLOAD_FACE_PIC,		//人脸截图上传
	UPLOAD_FACE_VIDEO,		//人脸采样视频上传
	UPLOAD_LITE_VIDEO,		//小视频上传
	UPLOAD_LOG_FILE,		//日志文件上传
};

enum FilePushType
{
    TYPE_PUSH_FILE_UPLOAD = 0,
    TYPE_PUSH_FILE_LOCK,

};

typedef struct __REMOTE_DEVICE_STATUS_INFO__
{
	std::string imei;
	int equipment_action;
    int cmd; //fixed value: 1100021
	std::string systemVersion;
	int versionCode;
	std::string sdcard_capacity;
    float lock_file_capacity;
	std::string net_state;
	int gps_signal;
	std::string after_camera_resolution;
	std::string before_camera_resolution;
	int microphone;
	int hardware_state;
    std::string bind_flag;
	std::string serialcode;
	std::string longitude;
	std::string latitude;
	std::string timestamp;
    std::string runtimes;
}RemoteDeviceStatusInfo;			//cmd 1

typedef struct __REMOTE_DEVICE_ABNORMAL_INFO__
{
	std::string imei;
	std::string order_id;
    int cmd; //fixed value: 1100022
	std::string systemVersion;
	int versionCode;
	int dismount;
	int tf_state;
	int g_sensor;
	std::string video0_name;
	std::string video1_name;
	std::string latitude;
	std::string longitude;
	std::string timestamp;
}RemoteDeviceAbnormalInfo;			//cmd 2

typedef struct __LOCK_FILE_INFO__
{
	std::string file_name;
	std::string file_path;
	std::string unlock_time;
}LockFileInfo;

typedef struct __REMOTE_FILE_LOCK__
{
	int type;
	std::string imei;
	std::string serialcode;
	std::string order_id;
	std::string timestamp;
	std::string callbackurl;
	std::vector<LockFileInfo> FileInfoVector;
}RemoteFileLock;		//cmd 4

typedef struct __REMOTE_FILE_UPLOAD__
{
	int file_action;
	std::string file_content_key;   //format: [key1_iv2]
	std::string imei;
	std::string file_name;
	std::string file_content;
	std::string order_id;
	std::string md5;
	std::string longitude;
	std::string latitude;
	int chunk_nums;
	int chunk_order_num;
	int iReadLength;
	int is_deleted;
	std::string serialcode;
	std::string timestamp;
	std::string callbackurl;
    int iswakedata;
}RemoteFileUpload;			//cmd 3

typedef struct __REMOTE_BASIC_HW_INFO__
{
	std::string imei;
	std::string longitude;
	std::string latitude;
}RemoteBasicHWinfo;	

typedef struct __FILE_PUSH_INFO_POOL_NODE__
{
    int chunk_size;
    int shoot_type;
    std::string file_path;
    std::string serialcode;
    std::string callbackurl;
    int iswakedata;
}FilePushInfoPoolNode;

typedef struct __FILE_LIST_PUSH_INFO_POOL_NODE__
{
    int chunk_size;
    int shoot_type;
    std::string file_path;
    std::string serialcode;
    std::string callbackurl;
    std::string order_id;
    std::string timestamp_start;
    std::string timestamp_end;
    int iswakedata;
}FileListPushInfoPoolNode;

typedef struct __FILE_PUSH_INFO__
{
    std::string file_name;
    std::string md5;
    std::string key;
}FilePushInfo;

typedef struct __FILE_INFO_NODE__
{
    int file_type;
    std::string file_name;
    int file_size;
    std::string file_path;
    std::string creat_time;
    int duration;
    std::string md5;
    int shoot_type;
    std::string key;
}FileInfoNode;

typedef struct __PUSH_COM_MSG__
{
	int push_type;
	std::string serialcode;
    int iswakedata;
}PushComMsg;		// cmd 17, 18, 9

typedef struct __PLACE_ORDER_MSG__
{
	PushComMsg comMsg;
	std::string order_id;
	int order_state;
}PlaceOrderMsg;			//cmd 5

typedef struct __CHANGE_PROPERTY_MSG__
{
	PushComMsg comMsg;
//	int after_camera_resolution;
//	int before_camera_resolution;
	int g_sensor;
}ChangePropertyMsg;			//cmd 6

typedef struct __SHOOTING_STATUS_MSG__
{
	PushComMsg comMsg;
	int before_camera_state;
	int after_camera_state;
	int car_audio;
}ShootingStatusMsg;			//cmd 7

typedef struct __SILENT_TAKE_PIC_MSG__
{
	PushComMsg comMsg;
	int shoot_type;		// 0 for in cam, 1 for out cam
	std::string callbackurl;
    int chunk_size;
	std::string pic_name;
}SilentTakePicMsg;			//cmd 8

typedef struct __FILES_LIST_NODE__
{
    int file_type;
    std::string file_name;
    int file_size;
    std::string file_path;
    std::string creat_time;
    int duration;
    std::string md5;
    int shoot_type;
    //std::string key;
}FileListNode;

typedef struct __FILES_LIST_PARA__
{
    int key;
    std::string file_path;
}FilesListPara; //used for sdk to pass encryto key, and the files list txt file path. Used for cmd 10 sub interface.

typedef struct __TRIGGER_FILE_LIST_MSG__
{
	PushComMsg comMsg;
	std::string callbackurl;
    std::string timestamp_start;
    std::string timestamp_end;
	std::string order_id;
    int iswakedata;
}TriggerFileListMsg;			//cmd 10

typedef struct __TRAFFIC_DATA_MSG__
{
	int chunk_size;
	int file_type;
	std::string start_time;
	std::string end_time;
	std::string callbackurl;
	PushComMsg comMsg;
}TrafficDataMsg;				//cmd 17

typedef struct __TRIGGER_SPECIFY_FILE_MSG__
{
	PushComMsg comMsg;
    int chunk_size;
	std::string file_name;
    int is_deleted; //上传成功是否设备需要删除该文件 0：不删除; 1: 删除
	std::string callbackurl;
}TriggerSpecifyFileMsg;			//cmd 11

typedef struct __TRIGGER_FACE_DETECT_MSG__
{
	PushComMsg comMsg;
    int frame_rate;
    int frame_count;
    int chunk_size;
	std::string callbackurl;
}TriggerFaceDetectMsg;			//cmd 12

typedef struct __TRIGGER_LITE_VIDEO_MSG__
{
	PushComMsg comMsg;
	int shoot_type;
    int audio_type;
	int camera_resolution;
	std::string video_name;
	int video_length;
    int chunk_size;
	std::string callbackurl;
}TriggerLiteVideoMsg;			//cmd 13

typedef struct __BULK_LOCK_MSG__
{
	PushComMsg comMsg;
	std::string order_id;
	std::string lock_start_time;
	std::string lock_end_time;
	std::string unlock_time;
	std::string callbackurl;
}BulkLockMsg;		//cmd 14

typedef struct __BULK_UNLOCK_MSG__
{
	PushComMsg comMsg;
	std::string order_id;
	std::string unlock_start_time;
	std::string unlock_end_time;
	std::string callbackurl;
}BulkUnLockMsg;			//cmd 15


typedef struct __RECORDING_AUDIO_MSG__
{
	PushComMsg comMsg;
	std::string audio_name;
	int audio_length;
    int chunk_size;
	std::string callbackurl;
}RecordingAudioMsg;			//cmd 16

typedef struct __SINGLE_LOCK_MSG__
{
	PushComMsg comMsg;
	std::string order_id;
	std::string file_name;
	std::string file_path;
	std::string unlock_time;
	std::string callbackurl;
}SingleLockMsg;			// cmd 19, 20

typedef struct __SINGLE_UNLOCK_MSG__
{
	PushComMsg comMsg;
	std::string order_id;
	std::string file_name;
	std::string file_path;
	std::string callbackurl;
}SingleUnlockMsg;			// cmd 19, 20

typedef struct __BIND_DEVICE_MSG__
{
	PushComMsg comMsg;
    std::string bind_flag;
}BindDeviceMsg;			// cmd 22

typedef struct __DRIVING_DATA_ACTION__
{
    int driving_mode;
    int after_camera_state;
    int before_camera_state;
    int microphone_state;
    int timestamp;
}DrivingDataAction;

typedef struct __DRIVING_DATA_FACE_LIST__
{
    //int age;
    //int sex;
    std::string face_pic_name;
    std::string face_pic_original;
    //int seat_location;
    std::string md5;
    std::string description;
}DrivingDataFaceList;

typedef struct __DRIVING_DATA_FACE__
{
    std::string order_id;
    std::string face_list_json;
    int timestamp;
}DrivingDataFace;

typedef FileListNode DrivingDataFilesList;

typedef struct __DRIVING_DATA_GPS__
{
    std::string longitude;
    std::string latitude;
    std::string altitude;
    int car_speed;
    int net_status;
    int timestamp;
}DrivingDataGPS;

typedef struct __DRIVING_DATA_USER__
{
    std::string driver_id;
    std::string imei;
    std::string sn;
    std::string sim_serial;
    std::string license_plate;
    int timestamp;
}DrivingDataUser;

enum EventType
{
    //#########################################################
    //event equipment_action, with event_data type: RemoteDeviceStatusInfo
    //TODO //TODO //TODO Dont modify this values
    EVENT_POWER_ON = 0,
    EVETN_POWER_SUSPEND,
    EVENT_POWER_OFF,
    EVENT_BATTERY_LEVEL_RESUME,
    EVENT_NET_RECOVERY,
    EVENT_TRIGGER_DEV_STATUS,
    EVENT_SD_FORMAT_FINISH,
    EVENT_SD_NO_SPACE_THRESHHOLD,
    //TODO //TODO //TODO Dont modify above values
    //#########################################################

    //event abnormal action, with event_data type: RemoteDeviceAbnormalInfo
    EVENT_SD_REINSTALLED,
    EVENT_SD_NO_EXISTING,
    EVENT_SD_CHANGED,
    EVENT_GSENSOR_ABNORMAL,

    //event other action, with event_data type: EventSDReqDeleteFilesData
    EVENT_SD_REQ_DELETE_FILES,

    //event request to https post the results
    EVENT_REQ_POST_PIC,
    EVENT_REQ_POST_LITEVIDEO,
    EVENT_REQ_POST_FACEDETECT,
    EVENT_REQ_POST_SEPARATEDAUDIO,
    EVENT_REQ_POST_TRAFFICDATA,
    EVENT_REQ_POST_FILELISTS,
    EVENT_REQ_POST_SPECIALFILE,
    EVENT_REQ_POST_GETFILEBYORDERID,
};

typedef struct __EVENT_SD_REQ_DELETE_FILES_DATA__
{
    std::string imei;
    int cmd; //fixed value: 1100023
    std::vector<LockFileInfo> deletefile;
    std::string timestamp;
}EventSDReqDeleteFilesData;

typedef struct __EVENT_REPORT_MSG__
{
	int err_no;
	EventType event_type;
    std::string file_name;
    std::string file_content_key;
    std::string md5;
    std::string serialcode;
    int chunk_size;
    std::string callbackurl;
}EventReportMsg;		// cmd 17, 18, 9

typedef struct __Subscript_CMD_MSG__
{
    int iswakedata;
    int payloadlen;
    std::string payload;
}SubscriptCmdMsg;

typedef struct __Didi_RET_MSG__
{
    std::string err_no;
    std::string err_msg;
}DidiRetMsg;

//////////////////////////////////////////////////

typedef struct __GPS_NODE_DD__
{
    float lng;
    float lat;
    int timestamp;
    int car_speed;
}GpsNodeDD;

typedef struct __GPS_CONFIG_NODE__
{
    int collect_interval;
    int report_interval;
}GpsConfigNode;

typedef struct __BINDFLAG_NODE__
{
    std::string bind_flag;
}BindflagNode;

typedef struct __DIDI_DATANODE__
{
    GpsConfigNode gps_conf;
    BindflagNode devbind;
}DIDIDataNode;

typedef struct __Didi_RET_GET_MSG__
{
    std::string err_no;
    std::string err_msg;
    DIDIDataNode data_node;
}DidiRetGetMsg;

typedef struct __INFO_GPS__
{
    GpsConfigNode gpscfg;
}InfoGPS;

typedef struct __INFO_REPORT_MSG__
{
    std::string reportmsg;
}InfoReportMsg;

typedef struct __GET_CONF_MSG__
{
    std::string imei;
    std::string sign;
}GetConfMsg;

typedef struct __UPGRADE_CONF__
{
    std::string old_version;
    std::string time_upgrade;
    std::string isReport;
}UpgradeConf;

typedef struct __UPGRADESTATUS_MSG__
{
    int cmd;
    std::string imei;
    std::string mqtt_user;
    std::string old_version;
    std::string new_version;
    std::string time_upgrade;
}UpgradeStatusMsg;

typedef struct __GLOBAL_INFO__
{
    int isReport;
    std::string sim;
    std::string imei;
    std::string sn;
    std::string username;
    std::string bindflag;
    std::string mqttstatus;
    std::string netstatus;
    std::string systemVersion;
    InfoGPS gps;
    UpgradeConf upgradeconf;
    int timestamp_poweron;
    int timestamp_poweroff;
}GlobalInfo;

typedef struct{
    int iType;
    RemoteFileUpload fileUploadHttps;
    RemoteFileLock fileLockHttps;
    std::string strurl;
    int iswakedata;
}RemoteHttpsUpload;

enum UPLOAD_STATUS
{
	START_UPLOAD = 0,
	PAUSE_UPLOAD,
	STOP_UPLOAD,
	AGAIN_UPLOAD,
};

enum UPLOAD_TYPE
{
	FILE_UPLOAD = 0,
	LOCK_FILE,
	REQCONF,
};

typedef struct __UPLOAD_FILE_DATA_ST__
{
	bool Is_Last_One;
	int upload_type;
	int Is_Deleted;
	int upload_status;
	int uploadTimes;
	int file_action;
	struct timeval time_start;
	std::string url;
	std::string file_name;
	std::string strjson;
}UPloadFileData;

#endif

