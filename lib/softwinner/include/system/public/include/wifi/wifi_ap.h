/* *******************************************************************************
 * Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 * *******************************************************************************/
/**
 * @file wifi_ap.h
 * @brief HAL for wifi ap control
 * @author id: KP0356
 * @version v0.1
 * @date 2016-08-28
 */

#ifndef _WIFI_AP_H_
#define _WIFI_AP_H_


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
#define MAX_SSID_LEN       256
#define MAX_BSSID_LEN      32
#define MAX_PSWD_LEN       64

#define AP_DB_PRT(fmt, args...) \
do{ \
    printf("[FUN]%s [LINE]%d  "fmt, __FUNCTION__, __LINE__, ##args); \
}while(0)


#define AP_ERR_PRT(fmt, args...) \
do{ \
    printf("\033[0;32;31m ERROR! [FUN]%s [LINE]%d  "fmt"\033[0m", __FUNCTION__, __LINE__, ##args); \
}while(0)


typedef enum tag_WIFI_AP_SECURITY_E
{
    WIFI_AP_SECURITY_OPEN = 0,
    WIFI_AP_SECURITY_WEP,
    WIFI_AP_SECURITY_WPA_WPA2_EAP,
    WIFI_AP_SECURITY_WPA_WPA2_PSK,
    WIFI_AP_SECURITY_WAPI_CERT,
    WIFI_AP_SECURITY_WAPI_PSK,
    WIFI_AP_SECURITY_BOTTON
} WIFI_AP_SECURITY_E;

typedef enum tag_WIFI_AP_ALG_TYPE_E
{
    WIFI_AP_ALG_CCMP = 0,
    WIFI_AP_ALG_TKIP,
    WIFI_AP_ALG_CCMP_TKIP,
    WIFI_AP_ALG_BOTTON,
} WIFI_AP_ALG_TYPE_E;


/******************************************************************************/
/*                         Structure Declarations                             */
/******************************************************************************/
typedef struct tag_WIFI_AP_CFG_S {
    char  ssid[MAX_SSID_LEN];
    char  bssid[MAX_BSSID_LEN];
    char  pswd[MAX_PSWD_LEN+1];
    int   channel;
    int   frequency;    //0-2.4GHz, 1-5GHz
    int   hidden_ssid;
    WIFI_AP_SECURITY_E security;
} WIFI_AP_CFG_S;


/******************************************************************************/
/*                            Global Variables                                */
/******************************************************************************/
/* None */


/******************************************************************************/
/*                          Function Declarations                             */
/******************************************************************************/
int wifi_ap_init(void);
int wifi_ap_exit(void);

int wifi_ap_open(const char *wifi_name);
int wifi_ap_close(const char *wifi_name);

int wifi_ap_start(const char *wifi_name, WIFI_AP_CFG_S *ap_cfg);
int wifi_ap_stop(const char *wifi_name);

int wifi_ap_get_config(const char *wifi_name, WIFI_AP_CFG_S *ap_cfg);


/******************************************************************************/
/*                          Function Definitions                              */
/******************************************************************************/
/* None */


#ifdef __cplusplus
#if    __cplusplus
}
#endif
#endif

#endif /* _WIFI_AP_H_ */
