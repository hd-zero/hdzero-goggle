/* *******************************************************************************
 * Copyright (C), 2001-2018, xiaoyi Tech. Co., Ltd.
 * *******************************************************************************/
/**
 * @file dd_global.h
 * @brief global info
 * 
 *
 * @version v0.1
 * @date 2018-09-05
 */

#ifndef __DD_GLOBALINFO_H__
#define __DD_GLOBALINFO_H__

#include <stdio.h>
#include <string>
#include <memory.h>
#include <pthread.h>
#include <utils/Mutex.h>
#include "dd_platform.h"

using namespace EyeseeLinux;

#define IFALSE       (0)
#define ITRUE        (1)
#define IREPOET      (2)


class DD_GLOBALINFO
{
public:
	DD_GLOBALINFO();
	~DD_GLOBALINFO();
	static DD_GLOBALINFO* GetInstance();

    int getDevInfoReport(void);
    void setDevInfoReport(int report);
    std::string getIMEI(void);
    int setIMEI(std::string imei);
    std::string getSIM(void);
    int setSIM(std::string sim);
    std::string getSN(void);
    int setSN(std::string sn);
    std::string getUserName(void);
    int setUserName(std::string username);
    std::string getMqttStatus(void);
    int setMqttStatus(std::string status);
    std::string getNetStatus(void);
    int setNetStatus(std::string status);
    std::string getSystemVersion(void);
    std::string getBindflag(void);
    int setBindflag(std::string flag);
    InfoGPS getGPS(void);
    int setGPSCfg(GpsConfigNode cfg);
    UpgradeConf getUpgradeConf(void);
    int setUpgradeConf(UpgradeConf cfg);
    int setTimestampPowerOn(int timestamp);
    int getTimestampPowerOn(void);
    int setTimestampPowerOff(int timestamp);
    int getTimestampPowerOff(void);
    int getRuntimes(int ts_poweroff);

private:
    int LoadUpdateConf(UpgradeConf &cfg);
    int SaveLoadUpdateConf(UpgradeConf cfg);

    Mutex m_mutex;
    GlobalInfo gInfo;
};

#endif

