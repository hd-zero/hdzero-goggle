/* *******************************************************************************
 * Copyright (C), 2001-2018, Allwinner Tech. Co., Ltd.
 * *******************************************************************************/
/**
 * @file dd_json.h
 * @brief dd_json接口定义头文件
 * 
 *
 * @version v0.1
 * @date 2018-05-16
 */
#ifndef __DD_JSON_H__
#define __DD_JSON_H__
#include <string>
#include "dd_common.h"
#include <json/json.h> 

#define BIND_STATUS_BIND        "true"
#define BIND_STATUS_UNBIND      "false"
#define BIND_STATUS_BINDED      "binded"
#define BIND_STATUS_NOTBINDED   "unbind"

// for paser background json data
int PaserPushType(const char *data);
int PaserPlaceOrder(const char *data, PlaceOrderMsg *Order);
int PaserChangeProperty(const char *data, ChangePropertyMsg *Property);
int PaserShootingStatus(const char *data, ShootingStatusMsg *Shooting);
int PaserSilentTakePic(const char *data, SilentTakePicMsg *Silent);
int PaserTriggerDevStatus(const char *data, PushComMsg *com);
int PaserTriggerFileList(const char *data, TriggerFileListMsg *list);
int PaserTriggerSpecifyFile(const char *data, TriggerSpecifyFileMsg *Specify);
int PaserTriggerFaceDetect(const char *data, TriggerFaceDetectMsg *face);
int PaserTriggerLiteVideo(const char *data, TriggerLiteVideoMsg *lite);
int PaserBulkLockFiles(const char *data, BulkLockMsg *files);
int PaserBulkUnlockFiles(const char *data, BulkUnLockMsg *files);
int PaserRecordingAudio(const char *data, RecordingAudioMsg *audio);
int PaserTrafficDataUpload(const char *data, TrafficDataMsg *log);
int PaserTriggerFormatSD(const char *data, PushComMsg *com);
int PaserSingleLockFile(const char *data, SingleLockMsg *file);
int PaserSingleUnlockFile(const char *data, SingleUnlockMsg *file);
int PaserBindDevice(const char *data, BindDeviceMsg *bindmsg);
int GetDidiRetMsg(const char *data, DidiRetMsg *RetMsg);

//for generate push json data
#if 0
int GenDevStatusInfo(RemoteDeviceStatusInfo *devInfo, char *push);
int GenDevAbnormalInfo(RemoteDeviceAbnormalInfo *devInfo, char *push);
int GenDevFileUpload2(RemoteFileUpload *fileUpload);
int GenDevFileLock(RemoteFileLock *lockFile, char *push);

#else
int GenDevStatusInfo(const RemoteDeviceStatusInfo *devInfo, std::string &push);
int GenDevAbnormalInfo(const RemoteDeviceAbnormalInfo *devInfo, std::string &push);
int GenDevEventReqDeleteFiles(const EventSDReqDeleteFilesData *evdata, std::string &push);
int GenDevFileUpload(const RemoteFileUpload *fileUpload, std::string &push);

int GenDevFileUploadwww(const RemoteFileUpload *fileUpload, std::string &push);
int GenDevFileLockNoJson(const RemoteFileLock *lockFile, std::string &push);

int GenDevFileLock(const RemoteFileLock *lockFile, std::string &push);

#endif
#endif
