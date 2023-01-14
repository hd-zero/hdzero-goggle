/* *******************************************************************************
 * Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 * *******************************************************************************/
/**
 * @file wifi_sta.h
 * @brief HAL for wifi control
 * @author id:
 * @version v0.1
 * @date 2016-08-28
 */

#ifndef _WIFI_STA_H_
#define _WIFI_STA_H_


/******************************************************************************/
/*                             Include Files                                  */
/******************************************************************************/


#ifdef __cplusplus
#if    __cplusplus
extern "C" {
#endif
#endif

/******************************************************************************/
/*                           Macros & Typedefs                                */
/******************************************************************************/
#define MAX_SSID_SIZE       256
#define MAX_BSSID_SIZE      32
#define MAX_IPV4_ADDR       14
#define MAX_PWD_SIZE        64
#define MAX_AP_LIST_SIZE    32

#define STA_DB_PRT(fmt, args...) \
do{ \
    printf("[FUN]%s [LINE]%d  "fmt, __FUNCTION__, __LINE__, ##args); \
}while(0)


#define STA_ERR_PRT(fmt, args...) \
do{ \
    printf("\033[0;32;31m ERROR! [FUN]%s [LINE]%d  "fmt"\033[0m", __FUNCTION__, __LINE__, ##args); \
}while(0)


typedef enum tag_WIFI_STA_SECURITY_E
{
    WIFI_STA_SECURITY_OPEN = 0,
    WIFI_STA_SECURITY_WEP,
    WIFI_STA_SECURITY_WPA_WPA2_EAP,
    WIFI_STA_SECURITY_WPA_WPA2_PSK,
    WIFI_STA_SECURITY_WAPI_CERT,
    WIFI_STA_SECURITY_WAPI_PSK,
    WIFI_STA_SECURITY_BOTTON
} WIFI_STA_SECURITY_E;

typedef enum tag_WIFI_STA_ALG_TYPE_E
{
    WIFI_STA_ALG_CCMP = 0,
    WIFI_STA_ALG_TKIP,
    WIFI_STA_ALG_CCMP_TKIP,
    WIFI_STA_ALG_BOTTON,
} WIFI_STA_ALG_TYPE_E;

typedef enum tag_WIFI_STA_EVENT_E
{
    WIFI_STA_EVENT_UP = 0,
    WIFI_STA_EVENT_DOWN,
    WIFI_STA_EVENT_CONNECTING,
    WIFI_STA_EVENT_CONNECTED,
    WIFI_STA_EVENT_DISCONNECTED,
    WIFI_STA_EVENT_SCANING,
    WIFI_STA_EVENT_SCAN_END,
    WIFI_STA_EVENT_SUPP_STOPPED,
    WIFI_STA_EVENT_BOTTON,
} WIFI_STA_EVENT_E;

typedef enum tag_WIFI_STA_STATUS_E
{
    WIFI_STA_STATUS_UNKNOW = 0,
    WIFI_STA_STATUS_INACTIVE,
    WIFI_STA_STATUS_SCANING,
    WIFI_STA_STATUS_SCAN_END,
    WIFI_STA_STATUS_CONNECTING,
    WIFI_STA_STATUS_CONNECTED,
    WIFI_STA_STATUS_DISCONNECTED,
    WIFI_STA_STATUS_COMPLETED,
    WIFI_STA_STATUS_BOTTON,
} WIFI_STA_STATUS_E;

typedef void (*WIFI_STA_EVENT_CALLBACK)(WIFI_STA_EVENT_E event, void *pdata);


/******************************************************************************/
/*                         Structure Declarations                             */
/******************************************************************************/
typedef struct tag_WIFI_STA_AP_INFO_S {
    char         ssid[MAX_SSID_SIZE];
    char         bssid[MAX_BSSID_SIZE];
    char         psswd[MAX_PWD_SIZE+1];
    unsigned int frequency;
    unsigned int db;
    unsigned int hidden_ssid;
    WIFI_STA_ALG_TYPE_E alg_type;
    WIFI_STA_SECURITY_E security;
} WIFI_STA_AP_INFO_S;

typedef struct tag_WIFI_STA_AP_LIST_S {
    int ap_list_num;
    WIFI_STA_AP_INFO_S ap_list[MAX_AP_LIST_SIZE];
} WIFI_STA_AP_LIST_S;

typedef struct tag_WIFI_STA_CONNECT_STATUS_S
{
    WIFI_STA_STATUS_E    state;
    WIFI_STA_AP_INFO_S   ap_info;
} WIFI_STA_CONNECT_STATUS_S;


/******************************************************************************/
/*                            Global Variables                                */
/******************************************************************************/
/* None */


/******************************************************************************/
/*                          Function Declarations                             */
/******************************************************************************/
int wifi_sta_init(void);
int wifi_sta_exit(void);

int wifi_sta_open(const char *wifi_name);
int wifi_sta_close(const char *wifi_name);

int wifi_sta_start(const char *wifi_name);
int wifi_sta_stop(const char *wifi_name);

int wifi_sta_start_scan(const char *wifi_name);
int wifi_sta_get_scan_status(const char *wifi_name, WIFI_STA_STATUS_E *pscan_status);
int wifi_sta_get_scan_results(const char *wifi_name, WIFI_STA_AP_LIST_S *pap_list);

int wifi_sta_connect(const char *wifi_name, WIFI_STA_AP_INFO_S *pap_info);
int wifi_sta_disconnect(const char *wifi_name);
int wifi_sta_get_connect_status(const char *wifi_name, WIFI_STA_CONNECT_STATUS_S *pconnect_status);

int wifi_sta_do_dhcp(const char *wifi_name);

int wifi_sta_register_eventcall(const char *wifi_name, WIFI_STA_EVENT_CALLBACK event_callback, void *pdata);
int wifi_sta_unregister_eventcall(const char *wifi_name);
int wifi_sta_get_event(const char *wifi_name, WIFI_STA_EVENT_E *psta_event);


/******************************************************************************/
/*                          Function Definitions                              */
/******************************************************************************/
/* None */


#ifdef __cplusplus
#if    __cplusplus
}
#endif
#endif

#endif /* _WIFI_STA_H_ */
