/* *******************************************************************************
 * Copyright (C), 2001-2018, Allwinner Tech. Co., Ltd.
 * *******************************************************************************/
/**
 * @file dd_mqtt.h
 * @brief MQTT接口定义文件
 * 
 *
 * @version v0.1
 * @date 2018-05-14
 */

#ifndef __DD_MQTT_H__
#define __DD_MQTT_H__

#include <stdio.h>
#include <string>
#include <memory.h>
#include <pthread.h>
#include "mqtt/MQTTLinux.h"
#include "mqtt/MQTTClient.h"
#include "dd_platform.h"
#include "dd_queue.h"
#include "../source/bll_presenter/AdapterLayer.h"


#define MQTT_COMMAND_TIMEOUT_MS         (5000)
#define MQTT_PRODUCTKEY                 "G6CGkEWp8q"  //"Pndx5xMbT2"        //fixed value
#define MQTT_CLIENTID_FORMAT            "%s|signmethod=hmacsha1,timestamp=%s|"
#define MQTT_CLIENTID                   MQTT_DEVICENAME
#define MQTT_PREFIX                     "-"
#define MQTT_SUB_PREFIX                 "-get"
#define MQTT_PUB_PREFIX                 "-update"
#define MQTT_SUB_TOPIC_FORMAT           "%s%s%s%s"
#define MQTT_PUB_TOPIC                  MQTT_PRODUCTKEY MQTT_PUB_PREFIX
#define MQTT_HOST                       "iothub.xiaojukeji.com" //"116.85.38.210" //offline tcp
//#define MQTT_PORT                       (1883)  //TCP port  //(21471)
//#define MQTT_HOST                       "iotpre.xiaojukeji.com"     //online tls
#define MQTT_PORT                       (1884)    // TLS port

#define NET_INTERFACE               "eth0"
#define USE_SUBSCRIPT_QUEUE         (1)


#define USE_TEST_API    (0)
#if USE_TEST_API
#define FILE_TEST_ALL                   "/mnt/extsd/test_all"
#define FILE_TEST_VIDEO                 "/mnt/extsd/test_lite_video"
#define FILE_TEST_PIC_CAPTURE           "/mnt/extsd/test_pic_capture"
#define FILE_TEST_PLACE_ORDER           "/mnt/extsd/test_place_order"
#define FILE_TEST_CHANGE_PROPERTY       "/mnt/extsd/test_change_property"
#define FILE_TEST_SHOOTING_STATUS       "/mnt/extsd/test_shooting_status"
#define FILE_TEST_DEV_STATUS            "/mnt/extsd/test_dev_status"
#define FILE_TEST_FILE_LIST             "/mnt/extsd/test_file_list"
#define FILE_TEST_SPECIFY_FILE          "/mnt/extsd/test_specify_file"
#define FILE_TEST_FACE_DETECT           "/mnt/extsd/test_face_detect"
#define FILE_TEST_BULK_LOCK_FILES       "/mnt/extsd/test_bulk_lock_files"
#define FILE_TEST_BULK_UNLOCK_FILES     "/mnt/extsd/test_bulk_unlock_files"
#define FILE_TEST_RECORDING_AUDIO       "/mnt/extsd/test_recording_audio"
#define FILE_TEST_DATA_UPLOAD           "/mnt/extsd/test_data_upload"
#define FILE_TEST_FORMAT_SD             "/mnt/extsd/test_format_sd"
#define FILE_TEST_SINGLE_LOCK_FILE      "/mnt/extsd/test_single_lock_file"
#define FILE_TEST_SINGLE_UNLOCK_FILE    "/mnt/extsd/test_single_unlock_file"
#define FILE_TEST_BIND_FILE             "/mnt/extsd/test_bind_file"
#define FILE_TEST_UNBIND_FILE           "/mnt/extsd/test_unbind_file"
#endif

#define ORDER_STATE_REBOOT_CHECK (1)

class DD_MQTT_Serv
{
public:
	//DD_MQTT_Serv();
	DD_MQTT_Serv();
	~DD_MQTT_Serv();
	static DD_MQTT_Serv* GetInstance();
    bool isGotIP(const char *inface, char *IP);
    bool netIsOk(const char *interface, char *IP);
    int MQTT_GetLoginInfo(char *user, int ulen, char *passwd, int plen, char *topic, int tlen,
        char *clitid, int clen, char *hst, int hlen, int* pport);
	int MQTT_Init(const char *user, const char *passwd, const char *topic, const char *clitid, const char *hst, int pport);
	int MQTT_DeInit();
	int MQTT_Connect();
	int MQTT_Post(MQTTMessage *pubMsg);
#if USE_TEST_API
	int MQTT_Run_Test();
#endif
	int MQTT_Run();
	void MQTT_Stop();
#if USE_SUBSCRIPT_QUEUE
    int DD_DealSubscriptMsg(void);
#endif
	static void* MqttThread(void *arg);
    static void* EvevtDealThread(void *arg);
#ifdef USE_IPC
    static void* DD_MQTT_Serv::MsgDealThread(void *arg);
#endif
#ifdef USE_REPORT_INFO
    static void* MsgReportThread(void *arg);
#endif
	int handleCommand(int Type, const char *data, int iswakedata);
    int MQTT_RUN_THREAD();
    bool Is_connect;

private:	
	int MQTT_GetStatus();
    int MQTT_GetTimeStamp(char *time_stamp, int len);
    int MQTT_GetUserSecret(char *user, int ulen, char *device_secret, int dlen);
    int MQTT_GetDeviceName(char *user, char *device_name);
    int MQTT_GetSubTopic(char* device_name, int dlen, char *sub_topic, int slen);
    int MQTT_Getclientid(char* device_name, char *time_stamp, char *clientid, int clen);
    int MQTT_GetPasswdClientid(const char* device_name, int dlen, char *clientid, int clen);
    int MQTT_GetServerHost(char *host);
    int MQTT_GetPort(int *port);
    int MQTT_HMACSHA1(char *device_secret, int ds_len, const unsigned char *data, int data_len, unsigned char *digest, unsigned int *digest_len);
    int MQTT_GetPasswd(const char* device_name, char *device_secret, char *time_stamp, char *passwd);
    int loadConfigInit(int type);
    int MqttSetWaitInterval(int recon_interval, int max);

	dd_Bg_Serv *dd_serv_;
	std::string username;
	std::string password;
	std::string topics;
	std::string clientid;
	std::string host;
	int port;
	QoS qos;
	int mqtt_status;
	int old_net_status;// 0 bad,1 good
	int net_status; // 0 bad,1 good
    int event_thread_status;
    int msg_thread_status;
    int msgreport_thread_status;
	unsigned char *read_buf;
	unsigned char *send_buf;
	Network net;
	MQTTClient mqttClient;
    MQTTMessage pubMsg;
    dd_Bg_Serv *dd_bg_serv;
    unsigned short g_pub_id;
	pthread_t mqtt_thread_id;
	pthread_t event_thread_id;
	pthread_t msg_thread_id;
	pthread_t report_thread_id;
	pthread_mutex_t mqtt_lock;
};

#endif

