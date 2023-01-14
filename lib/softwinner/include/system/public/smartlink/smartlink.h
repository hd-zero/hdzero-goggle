/************************************************************************************************/
/* Copyright (C), 2001-2017, Allwinner Tech. Co., Ltd.                                          */
/************************************************************************************************/
/**
 * @file smartlink.h
 * @brief 
 * @author id: guixing
 * @version v0.1
 * @date 2017-02-14
 */

#ifndef _SMARTLINK_H_
#define _SMARTLINK_H_

/************************************************************************************************/
/*                                      Include Files                                           */
/************************************************************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/************************************************************************************************/
/*                                     Macros & Typedefs                                        */
/************************************************************************************************/
//#define SMARTLINK_DEBUG
#define SMARTLINK_ERROR


/************************************************************************************************/
/*                                    Structure Declarations                                    */
/************************************************************************************************/
typedef enum tag_SMARTLINK_AP_SECURITY_E
{
    AP_SECURITY_OPEN = 0,
    AP_SECURITY_WEP,
    AP_SECURITY_WPA,
    AP_SECURITY_BOTTON
} SMARTLINK_AP_SECURITY_E;


typedef struct tag_SMARTLINK_AP_S {
    char  ssid[32];
    char  psswd[64];
    SMARTLINK_AP_SECURITY_E security;
    unsigned char  random_num;
} SMARTLINK_AP_S;

/************************************************************************************************/
/*                                      Global Variables                                        */
/************************************************************************************************/
/* None */

/************************************************************************************************/
/*                                    Function Declarations                                     */
/************************************************************************************************/

/**
 * @brief 设置wifi芯片为monitor模式或normal模式
 * @param  
 * - wifi_name 无线网卡设备名
 * - monitor_enable 是否使能monitor模式. 1:使能monitor 2:关闭monitor
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int set_wifi_monitor(const char *wifi_name, int monitor_enable);


/**
 * @brief 锁定Smartlink发送wifi信道操作
 * @param 
 * - wifi_name 无线网卡设备名
 * - lock_time_out   锁定操作整体超时时间,单位ms,一般取值30*1000ms
 * - chn_parser_time 轮训每个信道的停留搜索时间,单位ms, 一般取值100ms
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int lock_smartlink_wifi_chn(const char *wifi_name, int lock_time_out, int chn_parser_time);


/**
 * @brief 解析Smartlink发送过来的数据包，得出ssid, psswd以及用于结果返回的随机数值
 * @param 
 * - wifi_name 无线网卡设备名
 * - time_out  解析的超时时间,单位ms,一般取值20*1000ms
 * - ap_info   解析出数据后的返回结果
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int parser_smartlink_config(const char *wifi_name, int time_out, SMARTLINK_AP_S *ap_info);


/**
 * @brief 将成功接收到的smartlink数据中的随机数返回,通知手机发送端,注意:该函数用于成功连接AP后才能调用.
 * @param
 * - random_num 成功接收到的smartlink数据中的随机数
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int send_broadcast_val(unsigned char random_num);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _SMARTLINK_H_ */
